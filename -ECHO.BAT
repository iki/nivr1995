@echo off
if .%2==. goto exit
echo INCLUDE %1
echo INCLUDE %1 >>%2
:exit