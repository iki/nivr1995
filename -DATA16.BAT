@echo off
echo.
echo.>>_data16.inc
for %%f in (DB%1*.inc) do if exist .\%%f if not %%f == _DATA16.INC call .\-ECHO.BAT %%f _DATA16.INC