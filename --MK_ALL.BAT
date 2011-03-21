@echo off
rem if .%1==. goto exit
if not exist kernel00.asm goto exit
call tasm /s /c /x /w2 /w-TPI /m /z /zi kernel00
rem if errorlevel 1 goto exit
if not exist kernel00.obj goto exit
call tlink /c /d /l /m /s /3 %1 %2 %3 %4 %5 %6 %7 %8 %9 kernel00
if errorlevel 1 goto exit
echo.
echo COMPILING SUCCESSFUL  !!
echo.
:exit