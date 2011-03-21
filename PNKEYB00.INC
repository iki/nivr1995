;*************************************************************** key_clearbuf
;*** clears keyboard buffer
;*** in :  -
;*** ass:  all the structures related to keyboard handling
;*** out:  -
;*** mod:  -
;key_clearbuf proc far
;                mov     fs:[KEY_HEAD],0
;                mov     fs:[KEY_LAST],0
;                mov     fs:[KEY_TAIL],0
;                ret
;key_clearbuf endp

;*************************************************************** key_getfirst
;*** gets next char from keyboard buffer
;*** in :  -
;*** ass:  all the structures related to keyboard handling
;*** out:  -
;*** mod:  -
;key_getfirst proc far
;                push    bx ax
;                OPTZER0 ax
;                mov     bx,fs:[KEY_BUF_TAIL]
;                cmp     bx,fs:[KEY_BUF_HEAD]
;                jz      @@return
;
;                mov     al,fs:[KEY_BUFFER+bx]
;                dec     fs:[KEY_REPEAT+bx]
;                jnz     @@dontshifttail
;@@shifttail:
;                inc     bx
;                cmp     bx,KEY_BUFFER_SIZE
;                jb      @@truncated
;                OPTZER0 bx
;@@truncated:    mov     fs:[KEY_BUF_TAIL],bx
;@@dontshifttail:
;@@return:
;                mov     fs:[KEY_FIRST],al
;                pop     ax bx
;                ret
;key_getfirst endp

;*********************************************************** keyboard_handler
;*** IRQ 1 HANDLER
;*** ass:  all the structures related to keyboard handling
keyboard_handler proc far
                push    ax bx ds ;;es
                mov     bx,data16
                in      al,60h
                mov     ds,bx
                ;;mov     es,bx
                mov     ah,al

                cmp     [key_e1flag],0
                jne     @@e1flag_rem

                cmp     ah,0E1h
                je      @@e1flag_set

                cmp     ah,0E0h
                jae     @@e0flag_set

                mov     bl,al
                OPTZER0 bh
                and     bl,01111111b
                or      bl,[key_e0flag]
                ;;and     bl,[key_e0flag]
                ;;mov     [key_e0flag],127
                mov     [key_e0flag],0

                rol     ah,1
                jc      @@released

@@pressed:      mov     ah,[KEYS+bx]
                dec     ah
                add     [KEY_NP],ah
                mov     [KEYS+bx],1
                mov     ah,[KEY_LAST]
                mov     bh,byte ptr [request_flags]
                mov     word ptr [KEY_LAST],ax
                test    bh,REQF_EXIT_SYSTEM
                jnz     @@return
                ;;push    cx dx di si bp ebx
                ;;RECOGNIZE_KEY
                ;;pop     ebx bp si di dx cx
                cmp     bl,K_SCROLLLOCK
                jne     @@no_scroll
                cmp     byte ptr [KEYS+K_LCTRL],0
                jnz     @@ctrlscroll
                cmp     byte ptr [KEYS+K_RCTRL],0
                jz      @@no_scroll
@@ctrlscroll:
                cmp     byte ptr [KEYS+K_LALT],0
                jnz     @@altscroll
                cmp     byte ptr [KEYS+K_RALT],0
                jz      @@no_scroll
@@altscroll:
                PIC0_EOI
;                pop     ds bx ax
;                add     sp,2
;                cmp     word ptr ss:[sp],code16
;                je      @@programm_exit
;                add     sp,2
;                popf
;                call far dword ptr ss:[sp-6]
;@@programm_exit:
                EXIT_EVER_SAFE ds
@@no_scroll:
                cmp     bl,K_ESC
                je      @@yes_esc
                test    bh,REQF_SYSTEM
                jnz     @@return
                cmp     bl,K_BREAK
                je      @@yes_break
                ;;and     [request_flags],not REQF_PAUSE
IF 0 EQ 1
                cmp     bl,K_F9
                je      @@yes_illzer0
                cmp     bl,K_F10
                je      @@yes_illminus
                cmp     bl,K_F12
                je      @@yes_illplus
                cmp     bl,K_F11
                je      @@yes_illstd
ENDIF
@@return:
                PIC0_EOI
                pop     ds bx ax
                iret
@@e1flag_rem:   mov     word ptr [key_e0flag],128
                mov     word ptr [key_e1flag],0
                ;;mov     word ptr [key_e0flag],255
                jmp     short @@return
@@e1flag_set:   mov     [key_e1flag],128
                jmp     short @@return
@@e0flag_set:   mov     [key_e0flag],128
                ;;mov     [key_e0flag],255
                jmp     short @@return
@@released:
                dec     [KEY_NP]
                mov     [KEYS+bx],0
                jmp     short @@return
@@yes_esc:
                or      [request_flags],REQF_EXIT_GLOBAL
                jmp     short @@return
@@yes_break:
                ;;or      [request_flags],REQF_PAUSE
                cmp     byte ptr [KEY_LAST+1],K_BREAK
                je      @@return
                xor     [request_flags],REQF_PAUSE
                jmp     short @@return
IF 0 EQ 1
@@yes_illzer0:  push    es di si dx cx
                SET_FKM_ICV_STD 0,255,ds
                jmp     short @@illum_fade
@@yes_illminus: push    es di si dx cx
                SET_FKM_ICV -1,0,ds
                jmp     short @@illum_fade
@@yes_illstd:   push    es di si dx cx
                SET_FKM_ICV_STD PAL_INT_STD,255,ds
                jmp     short @@illum_fade
@@yes_illplus:  push    es di si dx cx
                SET_FKM_ICV 1,0,ds
;;                jmp     short @@illum_fade
@@illum_fade:   SET_FKM_ICP PAL_STD,ds
                VRT_IMMFADE_PAL ds,PALF_NSTEPS_IC,PAL,PAL_STD,PAL2_ADD,PAL2_FAD
                mov     ax,data16
                mov     si,offset PAL_STD
                mov     cx,768/4
                mov     di,offset PAL
                mov     es,ax
                rep     movsd
                pop     cx dx si di es
@@ret2:
                PIC0_EOI
                pop     ds bx ax
                iret
ENDIF
keyboard_handler endp