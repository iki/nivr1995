;set_a20.asm -> .exe
;idk [killian@novell.karlin.mff.cuni.cz], '94

;je-li prvni parametr: '1', povoli branu a20,
;                      '0', zakaze branu a20,
;jinak pouze vytiskne jeji nastaveni

locals @@
;jumps

DATA16          SEGMENT PUBLIC USE16 'DATA16'
msg_a20e         db 'Gate A20 is currently Enabled..',13,10,'$'
msg_a20d         db 'Gate A20 is currently Disabled..',13,10,'$'
msg_dis          db 'Disabling $'
msg_ena          db 'Enabling $'
msg_a20s         db 'A20 succeeded..',13,10,'$'
msg_a20f         db 'A20 failed..',13,10,'$'
param            db 0                       ; hodnota prvniho parametru - '0'
a20_set          db 0                       ; vysledek pokus o nastaveni a20
a20_tst          db 0                       ; vysledek testu nastaveni a20
DATA16          ENDS

STACK16         SEGMENT STACK USE16 'STACK16'
                db    128 DUP('STACK#$@')
STACK16         ENDS

CODE16          SEGMENT PUBLIC USE16 'CODE16'
                ASSUME   CS:CODE16,SS:STACK16,DS:DATA16

include gate_a20.inc

;*********************************************************************** main

start:          mov     si,5Dh
                lodsb                      ; prvni parametr z PSP:5Dh do al
                sub     al,30h

                mov     dx,data16
                mov     ds,dx              ; init ds

                mov     param,al           ; uschovej parametr

                cmp     al,1               ; neni-li parametr '0' nebo '1'
                ja      test_only          ;  tak pouze testuj nastaveni,

                mov     al,param
		call    set_a20            ;  jinak nastav gate a20
                mov     a20_set,al         ;  a uloz vysledek operace
test_only:
                call    tst_a20            ; nyni testuj nastaveni
                mov     a20_tst,al         ;  a uloz vysledek operace

; teprve nyni muzeme pouzit fce DOSu, nebot v pripade, ze je jadro DOSu v HMA
; tak si prvni volana fce DOSu ulozi nastaveni gate a20 a povoli ji a pokud
; bychom po ni volali fci :
;  1) tst_a20, vratila by enabled, nezavisle na stavu pred spustenim programu
;  2) set_a20, tak by sice branu a20 korektne nastavila, nicmene fce exit DOSu
;     ji (vzdy je-li jadro DOSu v HMA) opet prenastavi na nastaveni zjistene
;     prvni volanou fci DOSu, tedy v tomto pripade na nastaveni pred spustenim
;     programu

                mov     dx,offset msg_dis
                cmp     param,1
                ja      test_only2         ; je-li parametr '0' nebo '1',
                jb      print_ed
                mov     dx,offset msg_ena
print_ed:       mov     ah,9
                int     21h                ;  tiskni enbling(1)/disabling(0)

                mov     dx,offset msg_a20s
                cmp     a20_set,0
                jnz     print_set
                mov     dx,offset msg_a20f
print_set:      mov     ah,9
                int     21h                ;  a vysledek operace (v a20_set)
test_only2:
                mov	dx,offset msg_a20e
                cmp     a20_tst,0
                jnz     print_tst
                mov     dx,offset msg_a20d
print_tst:      mov     ah,9
                int     21h                ; tiskni akt. stav a20 (v a20_tst)
                mov     ah,4Ch
                int     21h                ; exit
CODE16          ENDS
END start