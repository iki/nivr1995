;keyboard scancodes

K_ESC           EQU     001h
K_1             EQU     002h
K_2             EQU     003h
K_3             EQU     004h
K_4             EQU     005h
K_5             EQU     006h
K_6             EQU     007h
K_7             EQU     008h
K_8             EQU     009h
K_9             EQU     00Ah
K_0             EQU     00Bh
K_MINUS         EQU     00Ch
K_PLUS          EQU     00Dh
K_BACKSPACE     EQU     00Eh
K_TAB           EQU     00Fh
K_Q             EQU     010h
K_W             EQU     011h
K_E             EQU     012h
K_R             EQU     013h
K_T             EQU     014h
K_Y             EQU     015h
K_U             EQU     016h
K_I             EQU     017h
K_O             EQU     018h
K_P             EQU     019h
K_LPARENT       EQU     01Ah
K_RPARENT       EQU     01Bh
K_ENTER         EQU     01Ch
K_LCTRL         EQU     01Dh
K_LCONTROL      EQU     01Dh
K_A             EQU     01Eh
K_S             EQU     01Fh
K_D             EQU     020h
K_F             EQU     021h
K_G             EQU     022h
K_H             EQU     023h
K_J             EQU     024H
K_K             EQU     025h
K_L             EQU     026h
K_COLON         EQU     027h
K_INVCOMMAS     EQU     028h
K_WAVE          EQU     029h
K_LSHIFT        EQU     02Ah
K_BACKSLASH     EQU     02Bh
K_Z             EQU     02Ch
K_X             EQU     02Dh
K_C             EQU     02Eh
K_V             EQU     02Fh
K_B             EQU     030h
K_N             EQU     031h
K_M             EQU     032H
K_COMMA         EQU     033h
K_POINT         EQU     034h
K_SLASH         EQU     035h
K_RSHIFT        EQU     036h
K_K_MUL         EQU     037h
K_LALT          EQU     038h
K_SPACE         EQU     039h
K_CAPSLOCK      EQU     03Ah
K_F1            EQU     03Bh
K_F2            EQU     03Ch
K_F3            EQU     03Dh
K_F4            EQU     03Eh
K_F5            EQU     03Fh
K_F6            EQU     040h
K_F7            EQU     041h
K_F8            EQU     042h
K_F9            EQU     043h
K_F10           EQU     044h
K_NUMLOCK       EQU     045h
K_SCROLLLOCK    EQU     046h
K_SCROLL        EQU     046h
K_K_HOME        EQU     047h
K_K_7           EQU     047h
K_K_UP          EQU     048h
K_K_8           EQU     048h
K_K_PGUP        EQU     049h
K_K_9           EQU     049h
K_K_MINUS       EQU     04Ah
K_K_LEFT        EQU     04Bh
K_K_4           EQU     04Bh
K_K_5           EQU     04Ch
K_K_RIGHT       EQU     04Dh
K_K_6           EQU     04Dh
K_K_PLUS        EQU     04Eh
K_K_END         EQU     04Fh
K_K_1           EQU     04Fh
K_K_DOWN        EQU     050h
K_K_2           EQU     050h
K_K_PGDN        EQU     051h
K_K_3           EQU     051h
K_K_INSERT      EQU     052h
K_K_0           EQU     052h
K_K_POINT       EQU     053h
K_K_DEL         EQU     053h
K_ALTPRNSCR     EQU     054h
K_SYSREQ        EQU     054h

K_F11           EQU     057h
K_F12           EQU     058h

;extended keyboard scancodes (following e0) incremented by 080h

K_K_ENTER       EQU     080h+28
K_RCTRL         EQU     080h+29
K_RCONTROL      EQU     080h+29
K_BREAK_UNUSED  EQU     080h+29
K_PRNSCR_NOSHIFT EQU    080h+42
K_K_SLASH       EQU     080h+53
K_PRNSCR        EQU     080h+55
K_RALT          EQU     080h+56
K_BREAK         EQU     080h+69
K_HOME          EQU     080h+71
K_UP            EQU     080h+72
K_PGUP          EQU     080h+73
K_LEFT          EQU     080h+75
K_RIGHT         EQU     080h+77
K_END           EQU     080h+79
K_DOWN          EQU     080h+80
K_PGDN          EQU     080h+81
K_INSERT        EQU     080h+82
K_DELETE        EQU     080h+83

;keyboard controller

KBC_DAT_PORT    EQU 060h
KBC_CMD_PORT    EQU 064h
KBC_IBF_BIT     EQU 02h
KBC_WOP_CMD     EQU 0D1h
KBC_ROP_CMD     EQU 0D0h
KBC_NOP_CMD     EQU 0FFh
A20_ON_DATA     EQU 0DFh
A20_OFF_DATA    EQU 0DDh
KBD_CMD_PORT    EQU 060h
KBD_SETMI_CMD   EQU 0EDh
KBD_ECHO_CMD    EQU 0EEh
KBD_SETSCS_CMD  EQU 0F0h ;PS/2
KBD_GETID_CMD   EQU 0F2h ;PS/2
KBD_SETTRD_CMD  EQU 0F3h
KBD_ENABLE_CMD  EQU 0F4h
KBD_DISABLE_CMD EQU 0F5h
KBD_DEFAULT_CMD EQU 0F6h
KBD_S3ATM_CMD   EQU 0F7h ;PS/2
KBD_S3AMB_CMD   EQU 0F8h ;PS/2
KBD_S3AM_CMD    EQU 0F9h ;PS/2
KBD_S3ATMMB_CMD EQU 0FAh ;PS/2
KBD_S3ITM_CMD   EQU 0FBh ;PS/2
KBD_S3IMB_CMD   EQU 0FCh ;PS/2
KBD_S3IM_CMD    EQU 0FDh ;PS/2
KBD_RESEND_CMD  EQU 0FEh
KBD_RESET_CMD   EQU 0FFh


KBD_COMMAND1 macro command,operator
local ksmo_insync0,ksmo_insync1,ksmo_err
                OPTZER0 cx
ksmo_insync0:   in      al,KBC_CMD_PORT
                test    al,KBC_IBF_BIT
                loopnz  ksmo_insync0
                jnz     ksmo_err
                mov     al,command
                out     KBD_CMD_PORT,al
                OPTZER0 cx
ksmo_insync1:   in      al,KBC_CMD_PORT
                test    al,KBC_IBF_BIT
                loopnz  ksmo_insync1
                jnz     ksmo_err
                mov     al,operator
                out     KBD_CMD_PORT,al
ksmo_err:
endm

KBD_XTEOIRQ1 macro
                in      al,61h
                jmp     $+2
                mov     ah,al
                or      al,80h
                out     61h,al
                jmp     $+2
                mov     ah,al
                out     61h,al
                jmp     $+2
endm

;destroys bp si di ax ebx cx dx
RECOGNIZE_KEY macro
local end_rec,recog_shift,recog_start,update_loc,update_loc_E
local n_up,n_up_E,n_down,n_down_E,n_rol,n_rol_E,n_ror,n_ror_E,
local n_right,n_right_E,n_left,n_left_E,n_alt ;;end_of_macro
                ;;cmp       [KEY_LAST],K_ESC
                ;;je        recog_esc
                OPTZER0   bp
                OPTZER0   si
                OPTZER0   di
                cmp       byte ptr [KEYS+K_LSHIFT],0
                jnz       recog_shift
                cmp       byte ptr [KEYS+K_RSHIFT],0
                jnz       recog_shift
                mov       ax,MAN_A_STEP
                mov       cx,MAN_X_STEP
                mov       dx,MAN_Y_STEP
                jmp       short recog_start
recog_shift:    mov       ax,MAN_A_SSTEP
                mov       cx,MAN_X_SSTEP
                mov       dx,MAN_Y_SSTEP
recog_start:
                cmp       byte ptr [KEYS+K_K_UP],0
                jnz       n_up
                cmp       byte ptr [KEYS+K_UP],0
                jnz       n_up
n_up_E:
                cmp       byte ptr [KEYS+K_K_DOWN],0
                jnz       n_down
                cmp       byte ptr [KEYS+K_DOWN],0
                jnz       n_down
n_down_E:
                cmp       byte ptr [KEYS+K_LALT],0
                jnz       n_alt
                cmp       byte ptr [KEYS+K_RALT],0
                jnz       n_alt

                cmp       byte ptr [KEYS+K_K_RIGHT],0
                jnz       n_ror
                cmp       byte ptr [KEYS+K_RIGHT],0
                jnz       n_ror
n_ror_E:
                cmp       byte ptr [KEYS+K_K_LEFT],0
                jnz       n_rol
                cmp       byte ptr [KEYS+K_LEFT],0
                jnz       n_rol
n_rol_E:
                jmp       short end_rec
;;recog_esc:      or        [request_flags],REQF_EXIT_GLOBAL
;;                jmp       near end_of_macro
n_up:
                add       si,cx
                jmp       short n_up_E
n_down:
                sub       si,cx
                jmp       short n_down_E
n_alt:
                cmp       byte ptr [KEYS+K_K_RIGHT],0
                jnz       n_right
                cmp       byte ptr [KEYS+K_RIGHT],0
                jnz       n_right
n_right_E:
                cmp       byte ptr [KEYS+K_K_LEFT],0
                jnz       n_left
                cmp       byte ptr [KEYS+K_LEFT],0
                jnz       n_left
n_left_E:
                jmp       short end_rec
n_ror:
                sub       bp,ax
                jmp       short n_ror_E
n_rol:
                add       bp,ax
;;                jmp       short n_rol_E
                jmp       short end_rec
n_right:
                add       di,dx
                jmp       short n_right_E
n_left:
                sub       di,dx
;;                jmp       short n_left_E
;;                jmp       short end_rec
end_rec:
                or        si,si
                jnz       update_loc
                or        di,di
                jnz       update_loc
                or        bp,bp
                jz        update_loc_E
update_loc:
                mov       bx,[MAN_A]
                and       ebx,0ffffh
if MAN_A_KBDADDC eq 0
  display "No more MAN_A_KBDADDC" translation
else
                sub       bx,MAN_A_KBDADDC
endif
                add       ebx,ebx
                add       [MAN_A],bp

                mov       ax,gs:[COSTAB+ebx]
                OPTZER0   cx
                mov       bp,ax
                imul      si
                add       ax,08000h
                adc       cx,dx

                mov       ax,gs:[SINTAB+ebx]
                mov       bx,ax
                imul      di
                add       ax,08000h
                adc       cx,dx

                add       [MAN_X],cx

                mov       ax,bx
                OPTZER0   cx
                imul      si
                add       ax,08000h
                adc       cx,dx

                mov       ax,bp
                imul      di
                add       ax,08000h
                adc       dx,0
                or        [request_flags],REQF_RECALC_VIEW
                sub       cx,dx
                add       [MAN_Y],cx
update_loc_E:
;;end_of_macro:
endm