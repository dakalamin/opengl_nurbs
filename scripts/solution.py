import click

import os
import shutil
import subprocess as subproc

from glob    import glob
from pathlib import Path


@click.group()
@click.pass_context
def solution(ctx):
    """
    Project solution management section
    """
    ctx.ensure_object(dict)


@solution.command()
@click.option('--ide', type=str, help="IDE version to generate project files for",
              prompt="Enter IDE name to generate project files for", prompt_required=False)
@click.pass_context
def generate(ctx, ide):
    """
    Generate project solution files
    """
    if ide is None:
        ide = ctx.obj['config']['ide']

    click.echo(f"Generating {ide} project files...")
    subproc.run(
        f"\"{ctx.obj['config']['premake']['exe']}\" {ide} --file={ctx.obj['config']['premake']['lua']}",
        shell=True)

@solution.command()
@click.argument('build_dir',  type=click.Path())
@click.pass_context
def copy(ctx, build_dir):
    """
    Copy dirs&files configured in <to_copy> to build_dir
    """
    click.echo(f"Copying dirs&files configured in <to_copy> section of {ctx.obj['config_filename']}...\n")

    build_path = Path(click.format_filename(build_dir))

    for pattern in ctx.obj['config']['to_copy']['dirs']:
        for dir_name in glob(pattern):
            shutil.copytree(dir_name, build_path.joinpath(dir_name))

    for pattern in ctx.obj['config']['to_copy']['files']:
        for file_name in glob(pattern):
            shutil.copy(file_name, build_path)


@click.option('-h', '--hard', default=False, is_flag=True,
              help="Perform the hard cleanup")
@solution.command()
@click.pass_context
def clean(ctx, hard):
    """
    Clean dirs&files configured in <to_delete(_hard)>
    """
    click.echo(f"Warning! You are about to clean up {'(HARD) ' if hard else ''}the solution")
    click.echo(f"by deleting dirs and folders specified in <{ctx.obj['config_filename']}>")
    click.echo( "\nSOME UNSAVED WORK MAY BE LOST!")
    if not click.confirm("Are you sure you'd like to proceed?"):
        return

    dirs, files = 0, 0

    message = "\nDeleting dirs&files configured in " + \
              "<to delete> and <to_delete_hard> sections" if hard else "<to delete> section" + \
             f" of {ctx.obj['config_filename']}...\n"
    click.echo(message)

    dir_patterns  = ctx.obj['config']['to_delete']['dirs']
    file_patterns = ctx.obj['config']['to_delete']['files']
    if hard:
        dir_patterns  += ctx.obj['config']['to_delete_hard']['dirs']
        file_patterns += ctx.obj['config']['to_delete_hard']['files']

    for pattern in dir_patterns:
        for dir_name in glob(pattern):
            shutil.rmtree(dir_name, ignore_errors=True)

            dirs += 1
            click.echo(f"Deleted dir \t{dir_name}")

    for pattern in file_patterns:
        for file_name in glob(pattern):
            os.remove(file_name)
            
            files += 1
            click.echo(f"Deleted file \t{file_name}")

    if dirs or files:
        click.echo()
        if dirs:
            click.echo(f"{dirs} dir{'s have' if dirs > 1 else ' has'} been deleted")
        if files:
            click.echo(f"{files} file{'s have' if files > 1 else ' has'} been deleted")
    else:
        click.echo("No dirs/files deleted - solution has already been cleaned")
