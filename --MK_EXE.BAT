@echo off
if not exist kernel00.obj --mk_all.bat
tlink /c /d /l /m /s /3 %1 %2 %3 %4 %5 %6 %7 %8 %9 kernel00