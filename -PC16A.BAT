@echo off
echo.
echo.>>_pc16a.inc
for %%f in (PF%1*.inc) do if exist .\%%f if not %%f == _PC16A.INC call .\-ECHO.BAT %%f _PC16A.INC