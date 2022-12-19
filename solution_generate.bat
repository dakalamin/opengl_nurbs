@echo OFF

call python scripts\setup.py premake install
echo.
call python scripts\setup.py solution generate

pause