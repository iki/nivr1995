@echo off
shift
REM set nidf=datafile
REM if exist %nidf% del %nidf%
if exist datafile del datafile
mkdf %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
echo.
if .%0 == . goto exit
if not exist fatfile goto exit
if not exist megafile goto exit
echo Binding fatfile and megafile to datafile..
copy /B fatfile + megafile datafile>nul
REM if not exist %0 goto exit
REM if not exist %1 goto exit
REM echo Binding fatfile, %0 and %1 to %nidf%..
REM copy /B fatfile + %0 + %1 %nidf%>nul
REM :loop
REM shift
REM if .%1 == . goto exit
REM if not exist %1 goto loop
REM echo Adding %1..
REM copy /B %nidf% + %1 %nidf%>nul
REM goto loop
:exit
REM set nidf=