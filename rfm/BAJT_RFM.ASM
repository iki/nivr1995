;bajt_rfm.asm -> .exe
;idk [killian@novell.karlin.mff.cuni.cz], '94

;demonstruje prepnuti do 'real flat' modu a zpet
;predpoklada 386 v REALNEM MODU !!

locals @@
;jumps
.486p

RFM_GDT_CODE16  EQU 08h
RFM_GDT_DATA16  EQU 10h
RFM_GDT_STK16   EQU 18h
RFM_GDT_DATA32  EQU 20h

DPRINTS         macro   msg                     ; makro pro tisk retezce
                mov     ah,9
	        mov     dx,offset msg
	        int     21h
                endm

STACK16    SEGMENT USE16 STACK 'STACK16'
                db    128 DUP('STACK#$@')
STACK16    ENDS

DATA16     SEGMENT PUBLIC USE16 'DATA16'

mrfmenter       db 13,10,'entering real flat mode..$'
mrfmleave       db 13,10,'leaving real flat mode..$'
merrea20f       db 13,10,'enabling a20 failed!$'
A20	        db 0FFh

rfm_gdt		dw	00000h, 00000h, 00000h, 00000h	; unusable
                dw	0FFFFh, 00000h, 09B00h, 00000h	; code16
                dw	0FFFFh, 00000h, 09300h, 00000h	; data16
                dw	00000h, 00000h, 09700h, 00000h	; stk16
                dw	0FFFFh, 00000h, 09300h, 0008Fh	; data32 (limit=4GB)
rfm_gdtr	dw	$-rfm_gdt-1             ; velikost GDT  } potrebne
rfm_gdt_ptr     dd      0                       ; lin_addr GDT  } pro LGDT

DATA16     ENDS

CODE16     SEGMENT PUBLIC USE16 'CODE16'
           ASSUME   CS:CODE16,SS:STACK16,DS:DATA16,GS:DATA32

include gate_a20.inc

;******************************************************* rfm_enter, rfm_leave
;to protected mode, set GS limit to 4GB/64KB and back to real
; in :  -
; out:  gs.limit=0FFFFh*4KB=4GB or gs.limit=0FFFFh*1B=64KB
; mod:  general regs, es=fs=ds, gs=0

rfm_enter proc                                  ; GS limit se nastavi na 4GB
                mov     cl,RFM_GDT_DATA32       ; do cl uloz selektor DATA32
                jmp     short @@rfm_go_on
rfm_leave:                                      ; GS limit se nastavi na 64KB
                mov     cl,RFM_GDT_DATA16       ; do cl uloz selektor DATA16
@@rfm_go_on:
		pushf                           ; uloz flagy (kvuli cli)
		cli                             ; zakaz maskovatelna preruseni

		in	al,70h                  ; zakaz pro jistotu i NMI
		mov	ah,al
		or	al,80h
		out	70h,al
                in      al,71h
		and	ah,80H
		mov	ch,ah                   ; a do 7. bitu ch uloz
                                                ;  jeho puvodni hodnotu

		mov	dx,cs                   ; modifikace vlastniho kodu
                mov     cs:[@@self_mod_cs],dx   ;  pro far jump pri prepinani
                        ; zpet do realu (cekam x pripominek o 'necistote'
                        ; tohoto reseni, prestoze jsem v real modu a v RAM
                        ; a zadne okolnosti mi nebrani to provest
                mov     bx,ss                   ; uschovej ss
                mov     dx,ds                   ; uschovej ds

		lgdt	fword ptr rfm_gdtr      ; LoadGDT

		mov	eax,cr0
		or	al,1			; set Protection Enabled bit
		mov	cr0,eax			; prepni do protected modu
		DB	0EAh                    ; far jump pro vycisteni
                DW      (offset @@csfix_pmode)  ;  instrukcni fronty
                DW      RFM_GDT_CODE16  	;  a nastaveni cs
@@csfix_pmode:
		mov	ax,RFM_GDT_DATA16       ; nastav ds, es, fs na DATA16
		mov	ds,ax
		mov	es,ax
		mov	fs,ax
                mov	ax,RFM_GDT_STK16        ; ss na STK16
                mov	ss,ax
                xor     ah,ah
                mov     al,cl
		mov	gs,ax                   ; gs=ax=cl, kde cl je selektor
                                                ;  data16 nebo data32
		mov	eax,cr0
		and	al,NOT 01		; clear PE bit
		mov	cr0,eax			; prepni zpet do realu

		DB	0EAh                    ; opet far jump pro vycisteni
                DW	(offset @@csfix_rmode)  ;  instrukcni fronty
                                                ;  a nastaveni cs
@@self_mod_cs	DW	?                       ; misto modifikace kodu (vyse)
@@csfix_rmode:
		mov	ss,bx                   ; obnov ss
		mov	ds,dx                   ; obnov ds
		mov	es,dx
		mov	fs,dx                   ; es=fs=ds .. obsah es,fs se
                        ; neuschovaval, ale je treba tyto registry pred prvnim
                        ; pouzitim zpatky v realu inicializovat
		xor	ax,ax
		mov	gs,ax                   ; gs=0

		in	al,70h                  ; obnov ze 7. bitu ch puvodni
                and	al,07Fh                 ;  stav NMI povoleno/zakazano
		or	al,ch
                out	70h,al
                in      al,71h

		popf                            ; obnov Interrupt Flag
		ret
rfm_enter endp

;*********************************************************************** main

start:          mov     dx,data16
                mov     ds,dx                   ; init ds

		call    tst_a20                 ; testuj a uloz stav a20
                mov     A20,al

                call    set_a20_on              ; povol ji (neni to treba
                or      al,al                   ;  pro samotne prepnuti)
                jnz     settinga20ok            ; pokud byla vracena chyba,
                DPRINTS merrea20f               ;  ohlas ji a skonci
                jmp     dosexit
settinga20ok:
                DPRINTS mrfmenter
                mov     ax,cs                   ; nastav bazi v code16 v GDT
		movzx   eax,ax
		shl     eax,4                   ; eax=lin_addr(cs)=cs*10h
		mov     word ptr [rfm_gdt+RFM_GDT_CODE16+2],ax
		ror     eax,16
                mov     byte ptr [rfm_gdt+RFM_GDT_CODE16+4],al

                mov     ax,ss                   ; nastav bazi v stk16 v GDT
		movzx   eax,ax
		shl     eax,4                   ; eax=lin_addr(ss)=ss*10h
		mov     word ptr [rfm_gdt+RFM_GDT_STK16+2],ax
		ror     eax,16
                mov     byte ptr [rfm_gdt+RFM_GDT_STK16+4],al

                mov     ax,ds                   ; nastav bazi v data16 v GDT
		movzx   eax,ax
		shl     eax,4                   
                mov     ebx,eax                 ; ebx=eax=lin_addr(ds)=ds*10h
                mov     word ptr [rfm_gdt+RFM_GDT_DATA16+2],ax
                ror     eax,16
                mov     byte ptr [rfm_gdt+RFM_GDT_DATA16+4],al

                xor     eax,eax                 ; uloz linearni adresu rfm_gdt
                mov     ax,offset rfm_gdt       ; do rfm_gdt_ptr
		add     ebx,eax
		mov     rfm_gdt_ptr,ebx

                call    rfm_enter               ; GS limit bude 4GB

                                                ; priklady 32b-adresace v gs
                                                ;  1) pres primy dword ofset
                mov     vram,0A690948h
			; "Hi" ('H' je svetle modre, 'i' je svetle zelene)

                                                ;  2) pres ebx,ebp,esi,edi
		mov 	ebx,offset vram+4
		mov	dword ptr gs:[ebx],02690148h
			; "Hi" ('H' je modra, 'i' je zelena)

                DPRINTS mrfmleave
		call    rfm_leave               ; GS limit bude opet 64KB

exit:           ;cmp     A20,0FFh               ; pripadny test, zda jsme tes-
                ;je      dosexit                ;  tovali (a zmenili) stav a20
                mov     al,A20                  ; obnov stav a20
                call    set_a20
                xor     al,al
dosexit:        mov     ah,4Ch                  ; exit
                int     21h
CODE16    ENDS

DATA32    SEGMENT USE32 'DATA32'
	  org 00B8A00h         ; 16. radek, 1. sloupec, 1. stranka vram E/VGA
vram      label dword
	  org 0100000h         ; musi zde byt (adresa>1MB), jinak DOS hlasi
                               ; 'Could not execute ....... .'
DATA32    ENDS

END start