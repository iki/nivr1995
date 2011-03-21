;qw_cpuid
locals @@
;jumps

.386p

NCH    EQU 0FFh ; Not Checked
NP     EQU 0FEh ; Not Present
I86    EQU 0
I286   EQU 1
I386   EQU 2
I486   EQU 3
I586   EQU 4
CPM    EQU 7
PMM    EQU 128
cr     EQU 13
lf     EQU 10
c$     EQU '$'

DATA    SEGMENT PUBLIC USE16 'DATA'
mi86     DB      '8086/88 CPU',cr,lf,c$
mi286    DB      '80286 CPU',cr,lf,c$
mi386    DB      '80386 CPU',cr,lf,c$
mi486    DB      '80486 CPU',cr,lf,c$
mi586    DB      'Pentium',cr,lf,c$
mi87n    DB      'FPU not found'
mi87     DB      '8087 FPU$'
mi287    DB      '80287 FPU$'
mi387    DB      '80387 FPU$'
mpm      DB      'Protected mode $'
mrm      DB      'Real Mode $'
CPU      DB      NCH
FPU      DB      NCH
fpu_st   DW      ?
DATA    ENDS

STKSEG  SEGMENT STACK USE16 'STACK' ; Z sobn¡k podle 16bitov˜ch pravidel.
         DB      1024 DUP(?)         ; Hloubka z sobn¡ku je 1KB.
STKSEG  ENDS

CODE    SEGMENT PUBLIC USE16 'CODE'
        ASSUME  CS:CODE,SS:STKSEG,DS:DATA

;********************************************************************* cpu_id

cpu_id proc
           cmp     CPU,NCH
           jnz     cpu_id_e
@@check86:
; 8086 check : Flags 12-15 are always set on XT
           mov     cl,I86
           mov     dx,offset mi86   ; Only In Test Program
           pushf
           pop     ax
           mov     bx,ax
           and     ah,0Fh
           push    ax
           popf
           pushf
           pop     ax
           and     ah,0F0h
           cmp     ah,0F0h
           jne     @@check86_e
@@check86b:
; 8086 check (2) : 86 - SP incs before pushing XXX others - SP incs after
           push    sp
           pop     ax
           cmp     ax,sp
           jne     @@check_e
@@check86_e:
@@checkpm:
; Protected mode check
           mov     dx,offset mrm    ; OITP
           smsw    ax
           test    al,01h
           jz      @@checkpm_e
           or      cl,PMM
           mov     dx,offset mpm    ; OITP
@@checkpm_e:
           mov     ah,9             ; OITP
           int     21h              ; OITP
@@check286:
; 80286 check : Flags 12-15 are always NOT set in Real Mode
           inc     cl
           mov     dx,offset mi286  ; OITP
           mov     ax,bx
           or      ah,0F0h
           push    ax
           popf
           pushf
           pop     ax
           test    ah,0F0h
           jz      @@check_e
@@check286_e:
           push    bx               ; not necessary
           popf                     ;    - || -
@@check386:
; 80386 check : Can't flip AC flag (18)
           inc     cl
           mov     edx,esp
           and     esp,not 3        ; align sp to limit 32b to prevent the ex-
                                    ; ception 17 when we set AC flag on a 486
           pushfd
           pop     eax
           mov     ebx,eax
           xor     eax,40000H       ; flip AC
           push    eax
           popfd
           pushfd
           pop     eax
           push    ebx
           popfd
           mov     esp,edx
           mov     dx,offset mi386  ; OITP
           xor     eax,ebx          ; compare
           test    eax,40000H       ; AC flag bit = 1 -> i486
           jz      @@check_e
@@check386_e:
@@check_80486:
; 80486 check : Can't flip ID flag (21)
           inc     cl
           mov     dx,offset mi486  ; OITP
           mov     eax,ebx
           xor     eax,200000h      ; flip ID
           push    eax
           popfd
           pushfd
           pop     eax
           push    ebx
           popfd
           xor     eax,ebx          ; compare
           test    eax,200000h      ; ID flag bit = 1 -> Pentium
           jz      @@check_e
           inc     cl
           mov     dx,offset mi586  ; OITP
@@check_e: mov     CPU,cl
cpu_id_e:
          ;mov     al,CPU
           ret
cpu_id endp

;********************************************************************* fpu_id

fpu_id proc
           cmp     FPU,NCH
           jnz     fpu_id_e
       	   mov     cl,NP
           mov     dx,offset mi87n  ; OITP
	   fninit                   ; reset FP status word
	   mov     fpu_st,5a5ah     ; initialize temp word to nonzero value				; non-zero value
	   fnstsw  fpu_st           ; save FP status word
           mov     ax,fpu_st        ; check FP status word
           cmp     al,0             ; see if correct status with  written
           jnz     @@check_e
@@check_control_word:
           mov     cl,I86
           mov     dx,offset mi87   ; OITP
           fnstcw  fpu_st           ; save FP control word
           mov     ax,fpu_st        ; check FP control word
           and     ax,103fh         ; see if selected parts look OK
           cmp     ax,3fh           ; check that 1's & 0's  correctly read
           jnz     @@check_e
@@check_infinity:
; 80287/80387 check for the Intel386 CPU
           mov     cl,I86
           mov     dx,offset mi87   ; OITP
           mov     al,CPU
           and     al,CPM
           cmp     al,I386
           jb      @@check_e
           inc     cl
           mov     dx,offset mi287  ; OITP
           fld1                     ; must use default control from FNINIT
           fldz                     ; form infinity
           fdiv                     ; 8087 and Intel287 NDP say +inf = -inf
           fld     st               ; form negative infinity
           fchs                     ; Intel387 NDP says +inf <> -inf
           fcompp                   ; see if they are the same and remove them
           fstsw   fpu_st           ; look at status from FCOMPP
           mov     ax,fpu_st
           sahf                     ; see if infinities matched
           jz      @@check_e        ; jump if 8087 or Intel287 is present
           inc     cl
           mov     dx,offset mi387  ; OITP
@@check_e: mov     FPU,cl
fpu_id_e:
          ;mov     al,FPU
	   ret
fpu_id endp

;*********************************************************************** main

start:     mov     ax,DATA
           mov     ds,ax
           call    cpu_id
           mov     ah,9
           int     21h
           call    fpu_id
           mov     ah,9
           int     21h
           mov     al,CPU
           mov     ah,4Ch
           int     21h
CODE    ENDS
END     start