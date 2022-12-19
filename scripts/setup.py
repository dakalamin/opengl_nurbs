import json
import os
import sys
import subprocess as subproc

from pathlib import Path


min_version = (3, 8)  # Because of walrus operators
if sys.version_info[0:2] < min_version:
    errmsg = f"This script requires Python version {min_version[0]}.{min_version[1]}+"
    raise RuntimeError(errmsg)
    

config_filename = "config.json"

prjdir = Path.cwd()
while not prjdir.joinpath(config_filename).exists():
    if str(prjdir := prjdir.parent) == prjdir.anchor:
        errmsg = "This script must be executed only somewhere from the project directory!"
        raise RuntimeError(errmsg)


with open(prjdir.joinpath(config_filename), 'r') as config_file:
    config_raw = json.load(config_file)

def _unwrap(struct):
    if   isinstance(struct, dict):
        return dict((sub, _unwrap(struct[sub])) for sub in struct)
    elif isinstance(struct, list):
        return list(_unwrap(sub) for sub in struct)
    elif isinstance(struct, str):
        while struct != (parsed := struct.format_map(config_raw)):
            struct = parsed
        return struct
         
config = _unwrap(config_raw)

# https://stackoverflow.com/questions/1871549/determine-if-python-is-running-inside-virtualenv
if __name__ == '__main__' and sys.prefix == sys.base_prefix:
    python_path = Path(sys.prefix).joinpath('python.exe')

    venv_path  = prjdir.joinpath(config['venv']['path'])
    venv_python_path = venv_path.joinpath('Scripts', 'python.exe')
    venv_pip_path    = venv_path.joinpath('Scripts', 'pip.exe')

    if not (venv_python_path.exists() and venv_pip_path.exists()):
        print("Upgrading pip...")
        subproc.run(f'"{python_path}" -m pip install --upgrade pip')

        print("\nCreating virtual env...")
        subproc.run(f'"{python_path}" -m venv "{venv_path}"')

        print("Upgrading pip in virtual env...")
        subproc.run(f'"{venv_python_path}" -m pip install --upgrade pip')

        print("\nChecking all requirements installed...")
        subproc.run(f'"{venv_pip_path}" install -r "{config["dir"]["script"]}/requirements.txt"')

        print(f"\nRunning script(s) in <{config['venv']['name']}>")
        
    py_script = f'"{sys.argv[0]}"'
    if len(sys.argv) > 1: py_script += ' ' + ' '.join(sys.argv[1:])

    subproc.run(f"{venv_python_path} {py_script}")
    exit()


# Click documentation: https://click.palletsprojects.com
import click

import premake
import solution


@click.group()
@click.pass_context
def setup(ctx):
    ctx.ensure_object(dict)
    ctx.obj['config_filename'] = config_filename
    ctx.obj['config'] = config
    ctx.obj['config']['premake']['exe'] += ctx.obj['config']['premake']['extension'][sys.platform]

    os.chdir(prjdir)

setup.add_command(premake.premake)
setup.add_command(solution.solution)

if __name__ == '__main__':
    setup(obj=dict())
