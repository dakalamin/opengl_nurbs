@echo OFF
::makes calling this script possible from any location
pushd %~dp0

echo WARNING! YOU ARE GOING TO CLEAN THE SOLUTION
echo SOME FILES MAY BE LOST PERMANENTLY
echo.


:start
set choice=
set /p choice=Would you like to proceed? [Y/N]: 

::if input is longer than one symbol, read the first & dump remaining
if not '%choice%' == ''  set choice=%choice:~0,1%
if /i  '%choice%' == 'Y' goto yes
if /i  '%choice%' == 'N' goto no
::default behavior - no option is intepreted as NO
if /i  '%choice%' == ''  goto no

echo "%choice%" is not valid
goto start


:yes
echo.
call python scripts/setup.py --clean
goto exit


:no
echo.
echo Default option is NO due to safety reasons
echo No cleaning was done!
goto exit


:exit
echo. && pause

::returns cmd to original location
::if the script was called not from its own one
popd
exit /b 0

