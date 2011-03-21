@echo off
if .%2==. goto exit
echo INCLUDE %1
echo INCLUDE %1 >>%2
if not exist %1 goto exit
find ";***" %1 >> _l_procs.
find "EQU" %1 >> _l_const.
find "equ" %1 >> _l_const.
find "MACRO" %1 >> _l_macro.
find "macro" %1 >> _l_macro.
:exit