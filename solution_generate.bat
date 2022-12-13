@echo OFF

call scripts\setup.py premake install
call scripts\setup.py solution generate

pause