@echo off
echo.
echo.>>_data32c.inc
for %%f in (DC%1*.inc) do if exist .\%%f if not %%f == _DATA32C.INC call .\-ECHO.BAT %%f _DATA32C.INC