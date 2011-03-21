@echo off
rem for %%f in (_DATA32C.INC;_DATA32X.INC;_DATA16.INC;_L_PROCS.;_L_CONST.;_L_MACRO.;_MACROS.INC;_PC16.INC;_PC16A.INC) do if exist %%f del %%f>nul
REM del _*.inc
if not exist _DATA16.INC call -DATA16.BAT
if not exist _DATA32C.INC call -DATA32C.BAT
if not exist _DATA32X.INC call -DATA32X.BAT
if not exist _MACROS.INC call -MACROS.BAT
if not exist _PC16.INC call -PC16.BAT
if not exist _PC16A.INC call -PC16A.BAT