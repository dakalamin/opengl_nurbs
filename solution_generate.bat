@echo OFF

call scripts\setup.py premake install
echo.
call scripts\setup.py solution generate

pause