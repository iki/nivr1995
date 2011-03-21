@echo off
echo.
echo.>>_pc16.inc
for %%f in (PN%1*.inc) do if exist .\%%f if not %%f == _PC16.INC call .\-ECHO.BAT %%f _PC16.INC