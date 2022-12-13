import click

import json
import shutil
import sys
import subprocess as subproc

from io             import BytesIO
from pathlib        import Path
from urllib.error   import HTTPError
from urllib.request import urlopen
from zipfile        import ZipFile


@click.group()
@click.pass_context
def premake(ctx):
    """
    Premake management section
    """
    ctx.ensure_object(dict)
    ctx.obj['cfg'] = ctx.obj['config']['premake']


@premake.command()
@click.option('-f', '--force', default=False, is_flag=True,
              help="Install new premake even if it's been installed already")
@click.option('-v', '--version', 'version_name', default='any', show_default=True,
              is_flag=False, flag_value='latest',
              help="Version name to install (or 'latest' for the latest release)")
@click.pass_context
def install(ctx, force, version_name):
    """
    Install premake in the project <external> directory
    """
    cfg = ctx.obj['cfg']

    if Path(cfg['exe']).exists():
        click.echo(f"{cfg['name']} found in {cfg['dir']}")
        if not force:
            click.echo( "No further installation is made\n")
            click.echo(f"If you want to reinstall {cfg['name']} anyways,")
            click.echo( "run <... premake install> with <-f/--force> option")
            return

        click.echo(f"Deleting existing {cfg['dir']}/{cfg['name']}")
        shutil.rmtree(ctx.obj['config']['dir']['external'], ignore_errors=True)
        click.echo()

    def _parse_releases(rep_api_link):
        # using cURL from cmd to get information on releases available on GitHub via GitHubAPI
        # https://docs.github.com/en/rest/reference/repos#list-releases
        cmnd = f'curl -H "Accept: application/vnd.github+json" {rep_api_link} --ssl-no-revoke'
        proc = subproc.run(cmnd, capture_output=True)

        # non-zero returncode indicates some problems speaking to GitHubAPI
        # for example, if internet connection is lost, curl ends with returncode=6
        if proc.returncode:
            click.echo("cURL failed working with GitHub API! Returned stderr is below\n")
            raise Exception(proc.stderr.decode('utf-8')) 

        try:
            # we get a json file from GitHub, so we need to decode it
            return json.loads(proc.stdout)
        except json.JSONDecodeError as error:
            click.echo("json module failed to decode what GitHub API returned! This stderr is below\n")
            raise error
    
    def _check_release(release_json):
        # presence of 'asset' segment indicates there are available releases
        # opposed to just 'message' and 'documentation_url' segments
        if 'assets' in release_json:
            for asset in release_json['assets']:
                if asset['name'].endswith(cfg['asset_name'][sys.platform]):
                    # returns version name and release asset url
                    return release_json['tag_name'], asset['browser_download_url']

        return False

    def _download(version, asset_link):
        name      = cfg['name']
        directory = cfg['dir']

        click.echo(f"Downloading {name}...")
        click.echo(asset_link)

        try:
            # unzipping and saving premake5 file
            # if breaks here, perhaps the dowloaded file wasn't a .zip one
            # btw, there is no much need to catch zipfile.BadZipFile Exception
            # because original error message is quite informative itself (see below)
            with urlopen(asset_link) as zip_asset:
                with ZipFile(BytesIO(zip_asset.read())) as zip_file: # ...zipfile.BadZipFile: File is not a zip file
                    click.echo(f"\nInstalling {name}...")
                    zip_file.extractall(directory)

        # usually occurs when link is invalid            
        except HTTPError as error:
            click.echo(f"{name} asset download link is broken! Returned stderr is below\n")
            raise error

        # configuring VERSION file rewriting anything with the same name
        with open(cfg['version'], 'w') as file:
            file.write(version)

        click.echo(f"{name} {version} has been downloaded to {directory}")
        click.echo("VERSION file configured")


        try:
            # downloading and configuring LICENSE file rewriting anything with the same name
            with urlopen(cfg['license_link']) as license_file:
                license_text = str(license_file.read().decode('utf-8'))

                with open(cfg['license'], 'w') as file:
                    file.write(license_text)
                
        except HTTPError as error:
            click.echo( "Unable to download LICENSE file,")
            click.echo(f"please consider doing it manually and placing it in {directory} dir")
            click.echo(cfg['repos_link'])
        
        else:
            click.echo("LICENSE file configured")

        return True

    def _loop_through_releases(v=None):
        for release in _parse_releases(cfg['releases_link']):
            data = _check_release(release)
            if data and (v is None or data[0].lower() == v):
                return _download(*data)

        return False

    version_name = version_name.lower()
    if version_name in ('latest', 'any', ""):
        # checking the latest release
        # https://docs.github.com/rest/reference/repos#get-the-latest-release
        click.echo("Checking the latest release...")

        latest_rel  = _parse_releases(f"{cfg['releases_link']}/latest")
        latest_data = _check_release(latest_rel)

        # if no release specified as 'latest' found, latest_data occurs to be a tuple('','')
        # which converts to False as bool(...)
        if latest_data:
            return _download(*latest_data)

        click.echo("No appropriate latest release found!")
    elif version_name:
        click.echo(f"Checking {version_name} release")
        if _loop_through_releases(version_name):
            return True

        click.echo("No {version_name} release found!")

    if version_name in ("any", ""):
        click.echo("Searching for any available release...")
    elif not click.confirm("Do you want to proceed searching for any available release?"):
        return

    if not _loop_through_releases():
        raise Exception(f"No appropriate releases found, unable to download {cfg['name']}")


def _get_file(filename, echo=True):
        try:
            with open(filename, 'r') as file:
                out = contents = file.read().rstrip('\n')
        except FileNotFoundError as error:
            out, contents = None, error

        if echo: click.echo(contents)
        return out

@premake.command()
@click.pass_context
def license(ctx):
    """
    Get premake license text
    """
    return _get_file(ctx.obj['cfg']['license'])

@premake.command()
@click.pass_context
def version(ctx):
    """
    Get premake version
    """
    return _get_file(ctx.obj['cfg']['version'])
