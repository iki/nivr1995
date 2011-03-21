JUMPFAR         macro	 seg,off
	        db       0eah
                dw	 (offset off)
                dw	 seg
endm

JUMPNEAR16      macro	 off
	        db       0e9h
                dw	 (offset off)
endm


CALLFAR         macro    seg,off
	        db       09ah
                dw	 (offset off)
                dw	 seg
endm

DPRINTS         macro    msg
                mov      ah,9
	        mov      dx,offset msg
	        int      21h
endm

DPRINTC         macro	 chr
                mov      ah,2
                mov      dl,chr
                int      21h
endm

WAIT_KEY_BIOS   macro
                xor      ah,ah
                int      16h
endm

WAIT_KEY        macro
local wait_k
                pushf
                sti
		mov	 fs:[KEY_NP],0
wait_k:
                cmp      fs:[KEY_NP],0
                je       wait_k
                popf
endm

REG_ERROR       macro    error_message
                ;;push     ds
                ;;push     bx
                ;;mov      bx,data16
                ;;mov      ds,bx
                mov      bl,fs:[error_n]
                xor      bh,bh
		shl	 bl,2
		inc	 byte ptr fs:[error_n]
                mov      word ptr fs:[error_msg+bx],offset error_message
                ;;pop      bx
                ;;pop      ds
endm

GEN_ERROR       macro    err_type
                mov      fs:error_t,err_type
                CALLFAR  code16,error
endm

GEN_ERROR2      macro
                CALLFAR  code16,error
endm

EXIT		macro
		JUMPFAR	 code16,EXIT_POINT
endm

EXITFO		macro
		JUMPFAR	 code16,EXITFO_POINT
endm

EXIT_FO         macro
                EXITFO
endm

EXIT_EVER_SAFE_OLD macro sreg
		jmp far  sreg:[exit_ever_safe_proc]
endm

EXIT_EVER_SAFE macro sreg
local exit_with_fadeout
		cmp      word ptr sreg:[exit_ever_safe_proc],offset EXITFO_POINT
                je       exit_with_fadeout
                EXIT
exit_with_fadeout:
                EXIT_FO
endm

SET_INT_VECT    macro    vectno,sreg,handler
                mov      word ptr sreg:[rfm_idt+vectno*4],offset handler
                mov      word ptr sreg:[rfm_idt+vectno*4+2],seg handler
endm

SET_INT_VECT2   macro    vectno,sreg,reg32
                mov      dword ptr sreg:[rfm_idt+vectno*4],reg32
endm

SET_IRQ_VECT   macro    vectno,sreg,handler
       pushf
       cli
       mov      word ptr sreg:[new_irqs+vectno*4],offset handler
       mov      word ptr sreg:[new_irqs+vectno*4],seg handler
IF vectno GT 7
       mov      word ptr sreg:[rfm_idt+(PIC1_NEW_IRQ_ADDR+vectno)*4],offset handler
       mov      word ptr sreg:[rfm_idt+(PIC1_NEW_IRQ_ADDR+vectno)*4+2],seg handler
ELSE
       mov      word ptr sreg:[rfm_idt+(PIC0_NEW_IRQ_ADDR+vectno)*4],offset handler
       mov      word ptr sreg:[rfm_idt+(PIC0_NEW_IRQ_ADDR+vectno)*4+2],seg handler
ENDIF
       popf
endm

SET_IRQ_VECT2   macro    vectno,sreg,reg32
       mov      dword ptr sreg:[new_irqs+vectno*4],reg32
IF vectno GT 7
       mov      dword ptr sreg:[rfm_idt+(PIC1_NEW_IRQ_ADDR+vectno)*4],reg32
ELSE
       mov      dword ptr sreg:[rfm_idt+(PIC0_NEW_IRQ_ADDR+vectno)*4],reg32
ENDIF
endm

;; SET_IRQ_PVECT   macro    vectno,sreg,handler
;;       mov      word ptr sreg:[old_irqs+vectno*4],offset handler
;;       mov      word ptr sreg:[old_irqs+vectno*4],seg handler
;;       SET_IRQ_VECT vectno,sreg,handler
;;endm

;;SET_IRQ_PVECT2   macro    vectno,sreg,reg32
;;       mov      dword ptr sreg:[old_irqs+vectno*4],reg32
;;       SET_IRQ_VECT2 vectno,sreg,reg32
;;endm

REM_IRQ_VECT    macro    vectno,sreg,reg32
       mov      reg32,dword ptr sreg:[old_irqs+vectno*4]
IF vectno GT 7
       mov      dword ptr sreg:[rfm_idt+(PIC1_NEW_IRQ_ADDR+vectno)*4],reg32
ELSE
       mov      dword ptr sreg:[rfm_idt+(PIC0_NEW_IRQ_ADDR+vectno)*4],reg32
ENDIF
endm

GO_OLD_IRQ macro vectno,sreg
       jmp      dword ptr sreg:[old_irqs+vectno*4]
endm
