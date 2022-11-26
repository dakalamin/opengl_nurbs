import sys
import json
import subprocess as subproc

from pathlib        import Path
from io             import BytesIO
from urllib.error   import HTTPError
from urllib.request import urlopen
from zipfile        import ZipFile


g_name = 'Premake5'

g_dir  = 'vendor/premake'
g_ver  = f'{g_dir}/VERSION'
g_lic  = f'{g_dir}/LICENSE'
g_path = f'{g_dir}/premake5' + '.exe' if sys.platform == 'win32' else ''


# UR - user-repository 
g_ur = '/premake/premake-core'

g_license_link  = f'https://raw.githubusercontent.com{g_ur}/master/LICENSE.txt'
g_repos_link    = f'https://github.com{g_ur}'
g_api_link      = f'https://api.github.com/repos{g_ur}'
g_releases_link = f'{g_api_link}/releases'

g_assets = {
    'win32'  : 'windows.zip',
    'linux'  : 'linux.tar.gz',
    'darwin' : 'macosx.tar.gz'
}


def _parse_releases(rep_api_link):
    # using cURL from cmd to get information on releases available on GitHub via GitHubAPI
    # https://docs.github.com/en/rest/reference/repos#list-releases
    cmnd = f'curl -H "Accept: application/vnd.github+json" {rep_api_link} --ssl-no-revoke'
    proc = subproc.run(cmnd, capture_output=True)

    # non-zero returncode indicates some problems speaking to GitHubAPI
    # for example, if internet connection is lost, curl ends with returncode=6
    if proc.returncode:
        errmsg = f"{proc.stderr.decode('utf-8')}"

        print("cURL failed working with GitHub API! Returned stderr is below\n")
        raise Exception(errmsg) 

    try:
        # we get a json file from GitHub, so we need to decode it
        return json.loads(proc.stdout)
    except json.JSONDecodeError as error:
        print("json module failed to decode what GitHub API returned! Returned stderr is below\n")
        raise error
    

def _check_release(release_json):
    # presence of 'asset' segment indicates there are available releases
    # opposed to just 'message' and 'documentation_url' segments
    if 'assets' in release_json:
        for asset in release_json['assets']:
            if asset['name'].endswith(g_assets[sys.platform]):
                # returns version name and release asset url
                return release_json['tag_name'], asset['browser_download_url']

    return False


def _install_premake(version, asset_link):
    print(f"Downloading {g_name}...")
    print(asset_link)

    try:
        # unzipping and saving premake5 file
        # if breaks here, perhaps the dowloaded file wasn't a .zip one
        # btw, there is no much need to catch zipfile.BadZipFile Exception
        # because original error message is quite informative itself (see below)
        with urlopen(asset_link) as zip_asset:
            with ZipFile(BytesIO(zip_asset.read())) as zip_file: # ...zipfile.BadZipFile: File is not a zip file
                print(f"Installing {g_name}...")
                zip_file.extractall(g_dir)

    # usually occurs when link is invalid            
    except HTTPError as error:
            print(f"{g_name} asset download link is broken! Returned stderr is below\n")
            raise error

    # configuring VERSION file rewriting anything with the same name
    with open(g_ver, 'w') as file:
        file.write(version)

    print(f"{g_name} {version} has been downloaded to {g_dir}")
    print("VERSION file configured")


    try:
        # downloading and configuring LICENSE file rewriting anything with the same name
        with urlopen(g_license_link) as license_file:
            license_text = f"{license_file.read().decode('utf-8')}"

            with open(g_lic, 'w') as file:
                file.write(license_text)
                
    except HTTPError as error:
        print("Unable to download LICENSE file,")
        print(f"please consider doing it manually and placing it in {g_dir} directory")
        print(g_repos_link)
        
    else:
        print("LICENSE file configured")


def default():
    if Path(g_path).exists():
        print(f"{g_name} already found")
    else:
        force()


def force():
    """
    FORCED UPDATE:
    tries to download & install latest Premake release;
    if failed, raises Exception
    """

    # checking the latest release specifically
    # https://docs.github.com/rest/reference/repos#get-the-latest-release
    latest_rel  = _parse_releases(f'{g_releases_link}/latest')
    latest_data = _check_release(latest_rel)

    # if no release specified as 'latest' found, latest_data occurs to be a tuple('','')
    # which converts to False as bool(...)
    if latest_data:
        return _install_premake(*latest_data)

    print("No appropriate latest release found, searching for all available releases...")
    all_rels = _parse_releases(g_releases_link)

    for release in all_rels:
        data = _check_release(release)
        if data:
            return _install_premake(*data) 
    else:
        raise Exception(f"No appropriate releases found, unable to download {g_name}")


def update():
    """
    USUAL UPDATE:
    tries to download & install new latest Premake release
    if no information found on previously installed versions or
    if failed, raises Exception
    """

    # quite similar to fupdate(), but also checking if anything already exists
    # and comparing versions of premake5 in local directory and found on GitHub
    if not Path(g_path).exists():
        errmsg = f"No {g_name} file found, download it with -default or -force options first or do it manually!"
        raise Exception(errmsg)

    if not Path(f'{g_dir}/VERSION'):
        raise Exception("No VERSION file found, configure it with -default or -force options first!")

    version = ''
    with open(g_ver, 'r') as file:
        version = file.read().rstrip('\n')

    latest_rel  = _parse_releases(f'{g_releases_link}/latest')
    latest_data = _check_release(latest_rel)

    if latest_data:
        # turns out python deals with comparing versions represented as strings really well
        # (tested on several examples in the style 'premake' does on GitHub)
        if version < latest_data[0]:
            _install_premake(*latest_data)
        else:
            print(f"Latest version of {g_name} is already downloaded")
        return

    print("No appropriate latest release found, searching for all available releases...")
    all_rels = _parse_releases(g_releases_link)

    for release in all_rels:
        data = _check_release(release)
        if data:
            if version < data[0]:
                _install_premake(*data)
            else:
                print(f"Latest version of {g_name} is already installed")
            return
    else:
        print(f"No appropriate releases found, current version of {g_name} is used")


def help(part):
    if part == 1:
        print( "--default\t:" + f" downloads {g_name} if not found in {g_dir}")
        print("--update\t\t:" + f" checks if latest version of {g_name} is installed")
        print(        "\t\t " + f" if not, downloads it")
        print( "--force\t\t:" + f" downloads {g_name} rewriting anything found in {g_dir}")
        
    elif part == 2:
        print(f"If something goes wrong in PREMAKE section, try installing {g_name} manually from...")
        print(f"{g_repos_link}/releases")
        print(f"Choose release asset for your specific platform and put it in {g_dir}")


if __name__ == "__main__":
    raise Exception("This script is for import only!")
