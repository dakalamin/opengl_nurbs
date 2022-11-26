@echo OFF
::makes calling this script possible from any location
pushd %~dp0

call python scripts/setup.py --default
echo. && pause

::returns cmd to original location
::if the script was called not from its own one
popd
exit /b 0