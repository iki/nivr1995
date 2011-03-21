@echo off
echo.
echo.>>_data32x.inc
for %%f in (DX%1*.inc) do if exist .\%%f if not %%f == _DATA32X.INC call .\-ECHO.BAT %%f _DATA32X.INC