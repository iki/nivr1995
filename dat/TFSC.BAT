@echo off
copy C:\mkdf.exe 
copy C:\kbd2.c .\01234abcd.df
copy D:\*.exe
copy D:\zork.orf .\00000001.orf
copy D:\PALETTES\3-3-2.plt .\3aa3aa2a.plt
if exist .\datafile. del .\datafile.
if exist .\fatfile. del .\fatfile.
if exist .\megafile. del .\megafile.
purge