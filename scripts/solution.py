import os
import sys
import shutil
import subprocess as subproc

from premake import g_name as premake_name
from premake import g_path as premake_path

from pathlib import Path


g_vs_version = 'vs2022'
g_deps_dir   = 'dependencies'


def build():
    """
    BUILD SOLUTION FILES FOR VS2022
    """
    
    print(f"\nRunning {premake_name}...")
    # running a cmd command like '[path_to_premake.exe]/premake5 vs2019'
    # that runs the lua script in project root dir
    # prebuilding it for Visual Studio 2019
    subproc.run((premake_path, g_vs_version))


def _rm_df(cwd):
    """
    REMOVE DIRS&FILES
    """
    
    smth_deleted = False
    
    fstodel = ('*.sln', '*.vcxproj*')        # files to delete
    dstodel = ('bin', 'bin_inter', 'build')  # directories to delete
    
    for folder in os.scandir(cwd):
        if folder.name in dstodel:
            shutil.rmtree(folder)
            smth_deleted = True
            print(f"Removed dir\t{cwd.joinpath(folder)}")

    for template in fstodel:
        for file in cwd.glob(template):
            os.remove(file)
            smth_deleted = True
            print(f"Deleted file\t{file}")

    return smth_deleted


def clean():
    """
    DELETE ALL SOLUTION RELATED DIRS&FILES
    also cleans all dependent subproject folders
    """
    
    # CWD - current working directory
    cwd = Path(os.getcwd())
    smth_deleted = 0
    
    smth_deleted += _rm_df(cwd)
    cwd = cwd.joinpath(g_deps_dir)
    if cwd.exists():
        for folder in os.scandir(cwd):
            if folder.is_dir():
                smth_deleted += _rm_df(Path(folder))
    else:
        print(f"DEPENDENCIES folder cannot be found")
        print(f"Please, check deps_dir value in {__file__}")
        print(f"(now it's <{g_deps_dir}\\>) so it matches")
        print(f"the folder that contains all project dependencies")

    if not smth_deleted:
        print("\nNo dirs/files deleted - solution is already cleaned")


if __name__ == "__main__":
    raise Exception("This script is for import only!")
