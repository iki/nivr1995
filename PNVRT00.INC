;*********************************************************** vretrace_handler
;*** VERTICAL RETRACE (synchronized irq 0) HANDLER
;*** ass:  data16:vrt_*

vretrace_handler proc far
            pusha
            push        ds
            mov         ax,data16
            mov         dx,VGA_IOMSBW+VGA_C_ADDR_W
                        ;+VGA_C_DATA_RW ;+VGA_M_INS1_R ;+VGA_C_ADDR_W
            mov         ds,ax

            inc         word ptr [timer_vrt_timer]

            mov         bh,byte ptr [vrt_request_flags]
            test        bh,VRT_RF_FLIPPAGES
            jz          @@flippage_e
            mov         ah,byte ptr [vga_c_saddrh_db]
            mov         al,VGA_CI_SADDRH
            xor         ah,40h
            ;mov         dl,VGA_C_ADDR_W
            mov         byte ptr [vga_c_saddrh_db],ah
            out         dx,ax
            ;;mov         al,byte ptr [vga_c_saddrh_db]
            ;;xor         al,40h
            ;;mov         dl,VGA_C_DATA_RW
            ;;mov         byte ptr [vga_c_saddrh_db],al
            ;;out         dx,al
            ;VGA_XOR_SADDR 40h,ds,vga_c_saddrh_db
@@flippage_e:

;------------------------------------------------

            mov         dl,VGA_M_INS1_R
@@wait_vrt:
            in          al,dx
            and         al,8
            jz          @@wait_vrt

;------------------------------------------------

; 34h=00110100b..counter0, LSB+MSB, monoflop (1:N rate gen,standard), 16bit
            mov         al,24h
            out         43h,al
            ;OPTZER0 al
            ;out         40h,al
            mov         al,byte ptr [timer_vrt_T+1]
            out         40h,al
            ;in           al,40h

;------------------------------------------------

            ;mov         bh,byte ptr [vrt_request_flags]
            ;test        bh,VRT_RF_FLIPPAGES
            ;jz          @@flippage_e
            ;;mov         ah,byte ptr [vga_c_saddrh_db]
            ;;mov         al,VGA_CI_SADDRH
            ;;xor         ah,40h
            ;;mov         dl,VGA_C_ADDR_W
            ;;mov         byte ptr [vga_c_saddrh_db],ah
            ;;out         dx,ax
            ;mov         al,byte ptr [vga_c_saddrh_db]
            ;xor         al,40h
            ;mov         dl,VGA_C_DATA_RW
            ;mov         byte ptr [vga_c_saddrh_db],al
            ;out         dx,al
            ;VGA_XOR_SADDR 40h,ds,vga_c_saddrh_db
;@@flippage_e:
;------------------------------------------------

            ;test        bh,VRT_RF_PF_GO
            ;jz          @@palfade_e

;------------------------------------------------
;fade 256 colors per retrace - top rows flicker on most vga's
IF 0 EQ 1
            test        byte ptr [request_flags],REQF_PAUSE
            jnz         @@palfade_e
            mov         bl,[palf_nsteps]
            OPTZER0     cl
            OPTTESTZER0 bl
            jz          @@palfade_e
            mov         al,cl
            dec         bl
            mov         dl,VGA_PEL_WA_RW
            mov         [palf_nsteps],bl
            out         dx,al
            mov         di,[palf_fadpal2off]
            mov         si,[palf_addpal2off]
            inc         dl ;,VGA_PEL_DATA_RW
@@fadeloop:
REPT 3
            mov         ax,[si]
            add         ax,[di]
            add         si,2
            mov         [di],ax
            mov         al,ah
            add         di,2
            out         dx,al
ENDM
            dec         cl
            jnz         @@fadeloop
@@palfade_e:
ENDIF
;------------------------------------------------
            cmp         byte ptr [palf_first128],0
            jz          @@fadenext128
            test        byte ptr [request_flags],REQF_PAUSE
            jnz         @@palfade_e
            mov         bl,[palf_nsteps]
            mov         cl,128
            OPTTESTZER0 bl
            jz          @@palfade_e
            OPTZER0     al
            dec         bl
            mov         dl,VGA_PEL_WA_RW
            mov         [palf_first128],al
            mov         [palf_nsteps],bl
            out         dx,al
            mov         di,[palf_fadpal2off]
            mov         si,[palf_addpal2off]
            inc         dl ;,VGA_PEL_DATA_RW
@@fadeloop0:
REPT 3
            mov         ax,[si]
            add         ax,[di]
            add         si,2
            mov         [di],ax
            mov         al,ah
            add         di,2
            out         dx,al
ENDM
            dec         cl
            jnz         @@fadeloop0
            jmp         short @@palfade_e
@@fadenext128:
            mov         cl,128
            mov         dl,VGA_PEL_WA_RW
            ;dec         bl
            ;mov         al,cl
            mov         [palf_first128],cl
            ;mov         [palf_nsteps],bl
            ;;dec         [palf_nsteps]
            ;out         dx,al
            mov         di,[palf_fadpal2off]
            mov         si,[palf_addpal2off]
            add         di,768
            add         si,768
            inc         dl ;,VGA_PEL_DATA_RW
@@fadeloop1:
REPT 3
            mov         ax,[si]
            add         ax,[di]
            add         si,2
            mov         [di],ax
            mov         al,ah
            add         di,2
            out         dx,al
ENDM
            dec         cl
            jnz         @@fadeloop1
@@palfade_e:

            mov         ah,byte ptr [timer_vrt_T+1]
            add         ah,byte ptr [timer_mftimer+1]
            mov         byte ptr [timer_mftimer+1],ah
            ;jc          @@std_timer_handler
            jnc         @@no_std_timer_handler
            ;JUMPNEAR16  @@std_timer_handler
            jmp          @@std_timer_handler
;------------------------------------------------
@@no_std_timer_handler:
            mov         al,20h
            out         20h,al
            pop         ds
            popa
            iret
;------------------------------------------------
@@illum_fade:   SET_FKM_ICP PAL_STD,ds
                VRT_IMMFADE_PAL ds,PALF_NSTEPS_IC,PAL,PAL_STD,PAL2_ADD,PAL2_FAD
                mov     ax,data16
                mov     si,offset PAL_STD
                mov     cx,768/4
                mov     di,offset PAL
                mov     es,ax
                rep     movsd
                pop     es
                ;jmp     short @@sys_key_e
                ;JUMPNEAR16 @@sys_key_e
                jmp     @@sys_key_e
@@yes_illzer0:  push    es
                SET_FKM_ICV_STD 0,255,ds
                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
@@yes_illminus: push    es
                SET_FKM_ICV -1,0,ds
                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
@@yes_illstd:   push    es
                SET_FKM_ICV_STD PAL_INT_STD,255,ds
                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
@@yes_illplus:  push    es
                SET_FKM_ICV 1,0,ds
                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
@@yes_invint:   push    es
                xor     [pal_flags],PAL_FLG_INVINT
@@yes_invfix:
                mov     dx,3c8h
                mov     si,[pal_ic_tt_act]
                mov     al,[pal_overscan]
                xor     al,PAL_OVERSCAN_XOR
                mov     [pal_overscan],al
                mov     cl,al
                out     dx,al
                mov     ah,[si]
                xor     ah,[pal_col_xor]
                OPTZER0 ch
                inc     dl
                mov     al,ah
                mov     bx,cx
                out     dx,al
                add     bx,cx
                out     dx,al
                add     bx,cx
                out     dx,al
                add     bx,offset PAL
                mov     dl,VGA_A_ADDR_RW
                mov     al,VGA_AI_OVSCOL
                mov     byte ptr [bx],ah
                out     dx,al
                inc     bx
                mov     al,cl
                out     dx,al
                mov     byte ptr [bx],ah
                mov     al,20h
                inc     bx
                out     dx,al
                mov     byte ptr [bx],ah

                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
@@yes_invcol:   push    es
                xor     [pal_flags],PAL_FLG_INVCOL
                xor     [pal_col_xor],DAC_MASK
                ;jmp     short @@yes_invfix
                ;jmp     short @@illum_fade
                ;JUMPNEAR16 @@illum_fade
                jmp     @@illum_fade
IF 0 EQ 1
@@yes_esc:
                or      [request_flags],REQF_EXIT_GLOBAL
                jmp     short @@sys_key_e
@@yes_break:
                ;;or      [request_flags],REQF_PAUSE
                cmp     byte ptr [KEY_LAST+1],K_BREAK
                je      @@sys_key_e
                xor     [request_flags],REQF_PAUSE
                jmp     short @@sys_key_e
ENDIF
;------------------------------------------------
@@std_timer_handler:
            test    [request_flags],REQF_EXIT_SYSTEM
            jnz     @@no_recog_key
            xor     [vrt_key_wait],1
            jz      @@sys_key_e
            cmp     byte ptr [KEYS+K_F5],0
            jnz     @@yes_invint
            cmp     byte ptr [KEYS+K_F4],0
            jnz     @@yes_invcol
            cmp     byte ptr [KEYS+K_F9],0
            jnz     @@yes_illzer0
            cmp     byte ptr [KEYS+K_F10],0
            jnz     @@yes_illminus
            cmp     byte ptr [KEYS+K_F12],0
            jnz     @@yes_illplus
            cmp     byte ptr [KEYS+K_F11],0
            jnz     @@yes_illstd
IF 0 EQ 1
            mov     al,[KEY_LAST]
            cmp     al,K_ESC
            je      @@yes_esc
            cmp     al,K_BREAK
            je      @@yes_break
            ;;and     [request_flags],not REQF_PAUSE
ENDIF
@@sys_key_e:
            test    [request_flags],REQF_SYSTEM+REQF_EXIT_SYSTEM+REQF_DISABLE_INPUT
            jz      @@recog_key
;------------------------------------------------
@@no_recog_key:
            push        ebx
            mov         ebx,dword ptr gs:[46Ch]
            inc         ebx
            cmp         ebx,TIMER_MIDNIGHT
            jna         @@not_midnight2
            OPTZER0     ebx
            mov         byte ptr gs:[470h],1
@@not_midnight2:
            mov         dword ptr gs:[46Ch],ebx
            pop         ebx ds
            ;;jmp         dword ptr fs:[old_irqs+0]
            mov         al,20h
            out         20h,al
            popa
            iret
;------------------------------------------------
@@recog_key:
            ;test        [request_flags],REQF_SYSTEM+REQF_EXIT_SYSTEM+REQF_DISABLE_INPUT
            ;jnz         @@no_recog_key
            push        ebx
            RECOGNIZE_KEY
            mov         ebx,dword ptr gs:[46Ch]
            inc         ebx
            cmp         ebx,TIMER_MIDNIGHT
            jna         @@not_midnight
            OPTZER0     ebx
            mov         byte ptr gs:[470h],1
@@not_midnight:
            mov         dword ptr gs:[46Ch],ebx
            pop         ebx ds
            ;;jmp         dword ptr fs:[old_irqs+0]
            mov         al,20h
            out         20h,al
            popa
            iret
vretrace_handler endp


;*************************************************** vretrace_handler_bfading
IF 0 EQ 1

vretrace_handler_bfading proc far
            pusha
            push        ds
            mov         ax,data16
            mov         dx,VGA_IOMSBW+VGA_M_INS1_R ;W+VGA_M_INS1_R
            mov         ds,ax

            inc         word ptr [timer_vrt_timer]

;            mov         bh,byte ptr [vrt_request_flags]
;            test        bh,VRT_RF_FLIPPAGES
;            jz          @@flippage_e
            ;;mov         ah,byte ptr [vga_c_saddrh_db]
            ;;mov         al,VGA_CI_SADDRH
            ;;xor         ah,40h
            ;;mov         dl,VGA_C_ADDR_W
            ;;mov         byte ptr [vga_c_saddrh_db],ah
            ;;out         dx,ax
            ;mov         al,byte ptr [vga_c_saddrh_db]
            ;xor         al,40h
            ;mov         dl,VGA_C_DATA_RW
            ;mov         byte ptr [vga_c_saddrh_db],al
            ;out         dx,al
;            VGA_XOR_SADDR 40h,ds,vga_c_saddrh_db
;@@flippage_e:

;------------------------------------------------

;            mov         dl,VGA_M_INS1_R
@@wait_vrt:
            in          al,dx
            and         al,8
            jz          @@wait_vrt

;------------------------------------------------

; 34h=00110100b..counter0, LSB+MSB, monoflop (1:N rate gen,standard), 16bit
            mov         al,34h
            out         43h,al
            OPTZER0 al
            out         40h,al
            mov         al,byte ptr [timer_vrt_T+1]
            out         40h,al

;------------------------------------------------

            mov         bh,byte ptr [vrt_request_flags]
            test        bh,VRT_RF_FLIPPAGES
            jz          @@flippage_e
            ;;mov         ah,byte ptr [vga_c_saddrh_db]
            ;;mov         al,VGA_CI_SADDRH
            ;;xor         ah,40h
            ;;mov         dl,VGA_C_ADDR_W
            ;;mov         byte ptr [vga_c_saddrh_db],ah
            ;;out         dx,ax
            ;mov         al,byte ptr [vga_c_saddrh_db]
            ;xor         al,40h
            ;mov         dl,VGA_C_DATA_RW
            ;mov         byte ptr [vga_c_saddrh_db],al
            ;out         dx,al
            VGA_XOR_SADDR 40h,ds,vga_c_saddrh_db
@@flippage_e:
;------------------------------------------------

            test        bh,VRT_RF_PF_GO
            jz          @@palfade_e

            test        byte ptr [request_flags],REQF_PAUSE
            jnz         @@palfade_e
            mov         cl,byte ptr [palf_i_arcolors]
            OPTZER0     ch
            or          cl,cl
            jnz         @@fade_it
            dec         byte ptr [palf_i_wait]
            jnz         @@palfade_e
            mov         al,byte ptr [palf_waitstep]
            mov         si,word ptr [palf_dstpaloff]
            mov         bl,byte ptr [palf_step]
            mov         byte ptr [palf_i_wait],al
            and         bh,not VRT_RF_PF_GO
            mov         di,offset PAL
            OPTZER0     dl
@@fadestep:
            mov         al,[di]
            mov         ah,[si]
            and         ax,DAC_MASK*257
            cmp         ah,al
            je          @@fs00_l
            ja          @@fs00_chg_gr
            sub         al,bl
            jbe         @@fs00_rplc
            cmp         ah,al
            jae         @@fs00_rplc
            jmp         short @@fs00_rplc2
@@fs00_chg_gr:
            add         al,bl
            jc          @@fs00_rplc
            cmp         ah,al
            jbe         @@fs00_rplc
@@fs00_rplc2:
            mov         ah,al
@@fs00_rplc:
            or          bh,VRT_RF_PF_GO
            mov         [di],ah
@@fs00_l:   inc         di
            inc         si

            mov         al,[di]
            mov         ah,[si]
            and         ax,DAC_MASK*257
            cmp         ah,al
            je          @@fs11_l
            ja          @@fs11_chg_gr
            sub         al,bl
            jbe         @@fs11_rplc
            cmp         ah,al
            jae         @@fs11_rplc
            jmp         short @@fs11_rplc2
@@fs11_chg_gr:
            add         al,bl
            jc          @@fs11_rplc
            cmp         ah,al
            jbe         @@fs11_rplc
@@fs11_rplc2:
            mov         ah,al
@@fs11_rplc:
            or          bh,VRT_RF_PF_GO
            mov         [di],ah
@@fs11_l:   inc         di
            inc         si

            mov         al,[di]
            mov         ah,[si]
            and         ax,DAC_MASK*257
            cmp         ah,al
            je          @@fs22_l
            ja          @@fs22_chg_gr
            sub         al,bl
            jbe         @@fs22_rplc
            cmp         ah,al
            jae         @@fs22_rplc
            jmp         short @@fs22_rplc2
@@fadedone: mov         byte ptr [vrt_request_flags],bh
            jmp         short @@palfade_e
@@fs22_chg_gr:
            add         al,bl
            jc          @@fs22_rplc
            cmp         ah,al
            jbe         @@fs22_rplc
@@fs22_rplc2:
            mov         ah,al
@@fs22_rplc:
            or          bh,VRT_RF_PF_GO
            mov         [di],ah
@@fs22_l:   inc         di
            inc         si

            dec         dl
            jnz         @@fadestep

            test        bh,VRT_RF_PF_GO
            jz          @@fadedone
;;            jnz         @@palfade_e
;;@@fadedone: mov         [vrt_request_flags],bh
;;            jmp         short @@palfade_e

            OPTZER0     al
            mov         dl,VGA_PEL_WA_RW
            out         dx,al
@@fade_it:  mov         si,offset PAL
            add         si,cx
            add         si,cx
            add         si,cx
            add         cl,PALF_RCOLORS
            mov         [palf_i_arcolors],cl
            mov         dl,VGA_PEL_DATA_RW
            mov         cx,PALF_RCOLORS*3
            rep         outsb
@@palfade_e:

;------------------------------------------------

            mov         ah,byte ptr [timer_vrt_T+1]
            add         ah,byte ptr [timer_mftimer+1]
            mov         byte ptr [timer_mftimer+1],ah
            jc          @@std_timer_handler

@@no_std_timer_handler:
            mov         al,20h
            out         20h,al
            pop         ds
            popa
            iret

;------------------------------------------------

@@std_timer_handler:
            push        ebx
            RECOGNIZE_KEY
            mov         ebx,dword ptr gs:[46Ch]
            inc         ebx
            cmp         ebx,TIMER_MIDNIGHT
            jna         @@not_midnight
            OPTZER0     ebx
            mov         byte ptr gs:[470h],1
@@not_midnight:
            mov         dword ptr gs:[46Ch],ebx
            pop         ebx ds
            ;;jmp         dword ptr fs:[old_irqs+0]
            mov         al,20h
            out         20h,al
            popa
            iret
vretrace_handler_bfading endp

ENDIF

;************************************************************* vrt_st_handler
;*** VERTICAL RETRACE SYNC_TIMER HANDLER
;*** ass:  data16:vrt_*

vrt_st_handler proc far
            push        ax
            inc         word ptr fs:[vrt_st_counter]
            mov         al,20h
            out         20h,al
            pop ax
            iret
vrt_st_handler endp

;************************************************************* start_vretrace
;*** synchronizes timer (irq0) with vertical retrace and installs handler
;*** in :  -
;*** ass:  data16:vrt_*
;*** out:  -
;*** mod:  -
start_vretrace proc far
            push        ax dx
            mov         ax,data16
            push        ds di
            mov         ds,ax
            pushf
            mov timer_vrt_T,(TIMER_MAXFREQ/TIMER_VRT_STFREQ) AND 0FF00h
            mov         vrt_request_flags,0

            cli
            SET_IRQ_VECT 0,ds,vrt_st_handler
            mov         al,0ffh
            PIC1_SET_IMR
            PIC_IO_DELAY
            dec         al
            PIC0_SET_IMR
            sti

            mov         dx,VGA_IOMSBW+VGA_M_INS1_R
@@wait0_endret:
            in          al,dx
            and         al,8
            jnz         @@wait0_endret
@@wait0_ret:
            in          al,dx
            and         al,8
            jz          @@wait0_ret

@@sync_loop:
            mov         al,34h
; 36h=00110110b..counter0, LSB+MSB, rectangle, 16bit
            out         43h,al
            mov         ax,timer_vrt_T
            out         40h,al
            mov         al,ah
            out         40h,al

            mov         vrt_st_counter,0

            ;;mov         dx,VGA_IOMSBW+VGA_M_INS1_R
@@wait1_endret:
            in          al,dx
            and         al,8
            jnz         @@wait1_endret
@@wait1_ret:
            in          al,dx
            and         al,8
            jz          @@wait1_ret

            add         timer_vrt_T,TIMER_VRT_ADDT
            cmp         vrt_st_counter,0
            jne         @@sync_loop
@@sync_loop_e:
            mov         al,24h
; 34h=00110100b..counter0, LSB+MSB, monoflop (1:N rate gen,standard), 16bit
            out         43h,al
            mov         ax,timer_vrt_T
            sub         ax,TIMER_VRT_ADDT+TIMER_VRT_SUBHT
            ;out         40h,al
            mov         timer_vrt_T,ax
            mov         al,ah
            out         40h,al

            cli
            SET_IRQ_VECT 0,ds,vretrace_handler
            PIC0_DEF_IMR ds
            PIC_IO_DELAY
            PIC1_DEF_IMR ds

            popf
            pop         di ds dx ax
            ret
start_vretrace endp

;*************************************************************** end_vretrace
;*** restores timer (irq0) default state and removes handler
;*** in :  -
;*** ass:  data16:vrt_*
;*** out:  -
;*** mod:  -
end_vretrace proc far
            push        eax
@@wait_fade:
;            test        fs:[vrt_request_flags],VRT_RF_PF_GO
;            jnz         @@wait_fade
            cmp         byte ptr fs:[palf_nsteps],0
            jnz         @@wait_fade
            pushf
            cli
            mov         al,36h
            out         43h,al
            OPTZER0     al
            out         40h,al
            mov         fs:[vrt_request_flags],al
            out         40h,al
            REM_IRQ_VECT 0,fs,eax
            popf
            pop         eax
            ret
end_vretrace endp

;************************************************************ endimm_vretrace
;*** restores timer (irq0) default state and removes handler immediately
;*** in :  -
;*** ass:  data16:vrt_*
;*** out:  -
;*** mod:  -
endimm_vretrace proc far
            push        eax
            pushf
            cli
            mov         al,36h
            out         43h,al
            OPTZER0     al
            out         40h,al
            mov         fs:[vrt_request_flags],al
            mov         byte ptr fs:[palf_nsteps],al
            out         40h,al
            REM_IRQ_VECT 0,fs,eax
            popf
            pop         eax
            ret
endimm_vretrace endp