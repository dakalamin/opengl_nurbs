@echo OFF

call scripts\setup.py premake install
call scripts\setup.py solution generate
call git submodule update --init

pause