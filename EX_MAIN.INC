DEBUG_WAIT_KEY

DEBUG_ENABLE_DEBUG_TEXT
DEBUG_INTERPOLL00
DEBUG_INTERPOLL01
DEBUG_MMS00
DEBUG_DFS01
DEBUG_INT00
DEBUG_INT01
DEBUG_VRTH01 1


                VGA_ENA0
                VGA_ENA1
                VGA_ENABLE
                VGA_PEL_MASK_SET

                mov    ax,data16
                mov    ds,ax
                mov    es,ax
                mov    fs,ax

                mov    di,offset PAL_DST
                mov    cx,0c0h
                xor    eax,eax
                rep    stosd

                ;call   start_vretrace
                ;VRT_IMMFADE_PAL ds,PALF_NSTEPS0,PAL,PAL_DST,PAL2_ADD,PAL2_FAD
                ;call   end_vretrace

                mov    bl,PALF_STEPA0
                mov    si,offset PAL
                mov    di,offset PAL_DST
                call   fade_apal

                mov    dx,VGA_IOMSBW+VGA_M_INS1_R
                in     al,dx
                mov    dl,VGA_A_ADDR_RW
                mov    al,VGA_AI_OVSCOL
                out    dx,al
                mov    al,[pal_overscan]
                out    dx,al
                mov    al,20h
                out    dx,al

                cld
                VGA_VRAMA_ZER0
                VGA_VRAMB_ZER0

MKDIVTAB_8_8_8x8


                mov      ebx,MAP_VRAM
                OPTZER0  eax
                mov      cx,PAGESIZEROUNDED/4
init_clr_mapvrambuf:
REPT 8
                mov      gs:[ebx],eax
                add      ebx,4
ENDM
                OPTLOOP  init_clr_mapvrambuf

                mov      ebx,MOVEVRAMBUF
                ;OPTZER0  eax
                mov      cx,PAGESIZEROUNDED/2
init_clr_movevrambuf:
REPT 8
                mov      gs:[ebx],eax
                add      ebx,4
ENDM
                OPTLOOP  init_clr_movevrambuf

DEBUG_FKM0_FMV

                mov      dx,data16
                mov      ah,PAL_MAX_COLOR_SHADES
                mov      cx,PAL_CON_MAXTAB+256*PAL_INT_MAXTAB
                mov      di,offset pal_ic_tt
                mov      ds,dx
                mov      es,dx
                call     prepare_pal_ic_tt
;;                mov      di,offset PAL_STD
;;                OPTZER0  ax
                ;SET_FKM_PAL_IC 0,0,PAL_STD,ds
                SET_FKM_PAL_IC_STD PAL_INT_STD,PAL_CON_STD,PAL_STD,ds
                ;mov      ax,data16
                mov      si,offset PAL_STD
                mov      di,offset PAL
                mov      cx,768/4
                ;mov      ds,ax
                ;mov      es,ax
                rep      movsd

;;DEBUG_FILL_AREA2 768,0,1,data16,PAL_DST,1

DEBUG_PRINT_PAL00 PAL_STD

;;                mov      ax,13h
;;                int      10h
                mov      bl,SET_XMODE_MODE
                call     set_xmode
;enable all planes
                mov      dx,VGA_IOMSBW+VGA_S_ADDR_W
                mov      ax,0f02h
                out      dx,ax
                VGA_VRAMA_ZER0

                VGA_ENABLE
                VGA_ENA1
                VGA_SADDRL_SET
                VGA_SADDRH_SET

                VGA_C_DEFADDR_SET
                VGA_S_DEFADDR_SET

                call     start_vretrace
                mov      fs:[vrt_request_flags],VRT_RF_FLIPPAGES
DEBUG_PRINT_VRT_T

		;mov	 bx,data16
                OPTZER0  eax
                mov      di,offset PAL_DST
                mov      cx,768/4
                ;mov      es,bx
                rep      stosd

                call     zer0_pal

DEBUG_OLD_FADEPAL
DEBUG_VGA00

include ex_dfsl.inc

                and      byte ptr fs:[request_flags],not REQF_EXIT_SYSTEM

;                mov      cx,768/4
;                mov      ebx,offset PALETTE_0
;                mov      di,offset PAL_DST
;loop_move_palette:
;                mov      eax,gs:[ebx]
;                mov      fs:[di],eax
;                add      ebx,4
;                add      di,4
;                OPTLOOP loop_move_palette

;                call     _view
                ;MOVE2VRAMUN calcvramseg,0A000h,RESX*RESY
                MOVE2VRAMFN MAP_VRAM,0a000h,RESX,RESY
                and      byte ptr fs:[request_flags],not REQF_SYSTEM
                VRT_IMMFADE_PAL fs,PALF_NSTEPS0,PAL_DST,PAL,PAL2_ADD,PAL2_FAD
                mov       byte ptr fs:[request_flags],REQF_EXIT_FADE
;;                call     _view
;;                mov      ax,data16
;;                mov      bl,PALF_STEP0
;;                mov      si,offset PAL
;;                mov      di,offset PAL_DST
;;                mov      ds,ax
;;                mov      es,ax
;;                call     fade_pal
                jmp        main_loop
main_exit_point:
                test    al,REQF_EXIT_FADE
                jz      p_exit_fade
                and     al,not REQF_EXIT_FADE
                or      al,REQF_DISABLE_INPUT
;;                and     byte ptr fs:[vrt_request_flags],not VRT_RF_PF_GO
                mov     byte ptr fs:[palf_nsteps],0
                mov     byte ptr fs:[request_flags],al
		mov     word ptr fs:[exit_ever_safe_proc],offset EXIT_POINT
                mov     ax,data16
                mov     di,offset PAL_DST
                mov     es,ax
                mov     cx,768/4
                OPTZER0 eax
                rep     stosd
                VRT_IMMFADE_PAL fs,PALF_NSTEPS0,PAL,PAL_DST,PAL2_ADD,PAL2_FAD
                jmp     short main_loop
p_exit_fade:
;;                test    byte ptr fs:[vrt_request_flags],VRT_RF_PF_GO
                cmp     byte ptr fs:[palf_nsteps],0
                jnz     main_loop_tstview

                or      al,REQF_SYSTEM
                mov     byte ptr fs:[request_flags],al
                call    endimm_vretrace
;                call    end_vretrace
                jmp     main_inter0_exit_point

main_loop:
                mov     al,byte ptr fs:[request_flags]
main_loop_tstexit:
                test    al,REQF_EXIT_GLOBAL
                jnz     main_exit_point
main_loop_tstview:
                ;test    al,REQF_PAUSE
                ;jnz     main_loop
                test    al,REQF_RECALC_VIEW
                jz      main_loop
                and     al,not REQF_RECALC_VIEW
                mov     byte ptr fs:[request_flags],al
;                call    _view
                ;MOVE2VRAMUN calcvramseg,0A000h,RESX*RESY
                ;MOVE2VRAMFN MAP_VRAM,0a000h,RESX,RESY
                jmp     main_loop
main_inter0_exit_point: