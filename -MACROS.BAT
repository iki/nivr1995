@echo off
echo.
echo.>>_macros.inc
for %%f in (M%1*.inc) do if exist .\%%f if not %%f == _MACROS.INC call .\-ECHO.BAT %%f _MACROS.INC