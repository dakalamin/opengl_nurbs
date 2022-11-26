import os
import sys

from pathlib import Path

import premake
import solution


def script_help():
    print("usage: [your system path to python]/python setup.py [option] ...")
    print("Only one or no arguments is expected, otherwise Exception is raised")
    print("Options and arguments:")
    premake.help(1)
    print("--clean")
    print("--help\t\t:" + " output reference information")
    print("If no option or argument is specified, behaves like -default is passed\n")

    print("All debug information is provided during script runtime")
    print("In case any problems encountered, read debug carefully")
    premake.help(2)


g_projname = 'CourseWork'
g_options  = {
    '--default' : (premake.default, True),
    '--update'  : (premake.update,  True),
    '--force'   : (premake.force,   True),
    '--clean'   : (solution.clean, False),
    '--help'    : (script_help,    False)
}
    

def main():
    # CWD - current working directory (whence the script was launched)
    cwd = Path(os.getcwd())

    # if currently not in "../Veresk" dir but inside some of its subdirs
    # changing CWD to "../Veresk"
    if not cwd.stem == g_projname:
        for parent in cwd.parents:
            if parent.stem == g_projname:
                os.chdir(parent)
                break
        else:
            errmsg = "This script must be executed only somewhere from '" + projname + "' project directory!"
            raise Exception(errmsg)


    build_solution = True

    if len(sys.argv) > 2:
        raise Exception("Too many options specified, only one expected!")

    # if no parameters passed, invoking default behavior
    if len(sys.argv) == 1:
        premake.default()
        
    elif sys.argv[1] not in g_options:
        errmsg = f"Option {sys.argv[1]} is undefined!"
        raise Exception(errmsg)

    # else, mapping the function from options dict and calling it
    else:
        g_options[sys.argv[1]][0]()
        build_solution = g_options[sys.argv[1]][1]

    if build_solution:
        solution.build()


if __name__ == "__main__":
    main()
