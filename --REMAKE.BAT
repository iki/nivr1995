@echo off
rem for %%f in (_DATA32C.INC;_DATA32X.INC;_DATA16.INC;_L_PROCS.;_L_CONST.;_L_MACRO.;_MACROS.INC;_PC16.INC;_PC16A.INC) do if exist %%f del %%f>nul
del _*.inc
call -DATA16.BAT
call -DATA32C.BAT
call -DATA32X.BAT
call -MACROS.BAT
call -PC16.BAT
call -PC16A.BAT