;gate_a20.inc
;idk [killian@novell.karlin.mff.cuni.cz], '94

KBC_DAT_PORT    EQU 060h                        ; datovy RW port KeyB. Contr.
KBC_CMD_PORT    EQU 064h                        ; prikazovy W a stavovy R

KBC_IBF_BIT     EQU 02h                         ; Input Buffer Full flag

KBC_WOP_CMD     EQU 0D1h                        ; Write 8042 Output Port
KBC_NOP_CMD     EQU 0FFh                        ; Flush 8042 Output Port and
                                                ; pulse none of its bits

A20_ON_DATA     EQU 0DFh                        ; 11011111 8042 Output Port
A20_OFF_DATA    EQU 0DDh                        ; 11011101
                        ;  keyboard data (output) /||||||\ system reset line
                        ; keyboard clock (output) _/||\|\_ gate a20
                        ;      input buffer empty __/| \__ undefined
                        ;      output buffer full ___/

;***************************************************************** KBC_flush
;flushes Keyboard Controller input buffer (synchronizes 8042)
; in :  -
; out:  ZF=0 <=> error
; mod:  CX=0 <=> error (99,99%); AL#0 <=> error

KBC_flush proc
                xor     cx,cx                   ; cykluj maximalne 64K-krat
@@insync:       in      al,KBC_CMD_PORT
                test    al,KBC_IBF_BIT          ; dokud jsou na portu 060h-R
                loopnz  @@insync                ;  nejaka neprectena data
                ret                             ; jsou-li i po 64K testech,
KBC_flush endp                                  ;  vrat chybu

;******************************************* set_a20, set_a20_on, set_a20_off
;sets gate a20 to the desired state
; in :  AL=0 <=> disable
; out:  AL=0 => failed; AL=1 => succeeded
; mod:  AH=a20_on/off_data; CX

set_A20 proc                                    ; nastaveni a20 podle al
                or      al,al
                jnz     set_a20_on              ;  je-li al>0, povol a20
set_a20_off:    mov     ah,A20_OFF_DATA         ; zakazani a20
                jmp     short @@set_A20
set_a20_on:     mov     ah,A20_ON_DATA          ; povoleni a20
@@set_A20:      call    KBC_flush               ; vyprazdni vstupni buffer KBC
                jnz     @@a20_err

                mov     al,KBC_WOP_CMD          ; zapis 8042 OP
                out     KBC_CMD_PORT,al
                call    KBC_flush               ; vyprazdni vstupni buffer KBC
                jnz     @@a20_err

                mov     al,ah                   ; A20_(ON/OFF)_DATA
                out     KBC_DAT_PORT,al         ; nastav bit urcujici stav a20
                call    KBC_flush               ; vyprazdni vstupni buffer KBC
                jnz     @@a20_err

                mov     al,KBC_NOP_CMD
                out     KBC_CMD_PORT,al         ; zajisti provedeni zapisu
                call    KBC_flush               ; vyprazdni vstupni buffer KBC
                jnz     @@a20_err
                mov     al,1
                ret
@@a20_err:      xor     al,al                   ; chyba - neprectena data
                ret                             ;  ve vstupnim bufferu KBC
set_a20 endp

;******************************************************************** tst_a20
;test state of gate a20
; in :  -
; out:  AL=0 => disabled; AL=1 => enabled
; mod:  AH=0; CX=0 <=> disabled

tst_a20 proc
                push    ds es                   ; uschovej es,ds
                xor     ax,ax
		mov     ds,ax
                mov     si,ax                   ; DS:SI = 0000:0000h = 000000h
		dec     ax
		mov     es,ax
		mov     di,10h                  ; ES:DI = FFFF:0010h = 100000h
                mov     cx,10h                  ; porovnani 10h slov by melo
                cld                             ;  stacit
                repz    cmpsw                   ; porovnej 10h slov od
                                                ; DS:SI se slovy od ES:DI
                pop     es ds                   ; obnov es,ds
                inc     ax                      ; al=0
                jcxz    @@a20_off               ; bylo-li porovnani uspesne,
                        ; doslo pravdepodobne k "memory wrap" - vynulovani
                        ; 20. bitu adresy (100000&0FFFFFh=000000h, srovnej
                        ; s pretecenim offsetu v segmentu) a a20 je zakazana
                inc     al                      ; jinak nastav al=1
@@a20_off:      ret
tst_a20 endp