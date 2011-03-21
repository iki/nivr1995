CPUID_FAMILY_MSK    equ     0f00h
CPUID_FAMILY_SHR    equ     8
CPUID_MODEL_MSK     equ     0f0h
CPUID_MODEL_SHR     equ     4
CPUID_STEPPING_MSK  equ     0fh
CPUID_STEPPING_SHR  equ     0
CPUID_F_FPU         equ     1
CPUID_F_MCE         equ     80h
CPUID_F_CMPXCHG8B   equ     100h

DB_PREFIX_ES        equ     026h
DB_PREFIX_CS        equ     02Eh
DB_PREFIX_SS        equ     036h
DB_PREFIX_DS        equ     03Eh
DB_PREFIX_FS        equ     064h
DB_PREFIX_GS        equ     065h
DB_PREFIX_OPND      equ     066h
DB_PREFIX_ADDR      equ     067h
DB_PREFIX_LOCK      equ     0F0h
DB_PREFIX_REPN      equ     0F2h
DB_PREFIX_REP       equ     0F3h

PREFIX_CS macro
        db      DB_PREFIX_CS
endm

PREFIX_DS macro
        db      DB_PREFIX_DS
endm

PREFIX_ES macro
        db      DB_PREFIX_ES
endm

PREFIX_FS macro
        db      DB_PREFIX_FS
endm

PREFIX_GS macro
        db      DB_PREFIX_GS
endm

PREFIX_SS macro
        db      DB_PREFIX_SS
endm

PREFIX_OPND macro op_code, op_erand
        db      DB_PREFIX_OPND
  ifnb <op_code>
        db      op_code
    ifnb <op_erand>
        dd      op_erand
    endif
  endif
endm

PREFIX_ADDR macro op_code, op_erand
        db      DB_PREFIX_ADDR
  ifnb <op_code>
        db      op_code
    ifnb <op_erand>
        dd      op_erand
    endif
  endif
endm

PREFIX_LOCK macro
        db      DB_PREFIX_LOCK
endm

PREFIX_REPN macro
        db      DB_PREFIX_REPN
endm

PREFIX_REP macro
        db      DB_PREFIX_REP
endm

;----------------------------------------------------------------------------
; M-CPUID
; Pre B0 steppings:
;  Input:   -
;  Output:  EAX[3:0]   <- stepping
;	    EAX[7:4]   <- model
;	    EAX[11:8]  <- family
; B0 and later steppings:
;  Input:   EAX = 0 or 1
;  Output:  IF EAX=0
;	         EAX=highest value CPUID understands
;		 EBX, EDX, ECX = vendor identification string
;	    ELSE
;	  	 EAX[3:0]   <- stepping
;	  	 EAX[7:4]   <- model
;	  	 EAX[11:8]  <- family
;	  	 EAX[31:12] <- reserved
;	  	 EBX, ECX   <- reserved
;	  	 EDX[0:0]   <- floating point present if 1
;	  	 EDX[31:1]  <- reserved feature flag bits

M_CPUID macro
        db      0fh
        db      0a2h
endm

TIMER_PORT0     EQU     40h
TIMER_PORT1     EQU     41h
TIMER_PORT2     EQU     42h
TIMER_PORTM     EQU     43h

TIMER_MSEL_0    EQU     00h
TIMER_MSEL_1    EQU     40h
TIMER_MSEL_2    EQU     80h
TIMER_MSEL_RB   EQU     0C0h
TIMER_MSEL      EQU     0C0h

TIMER_MRWL_LCH  EQU     00h
TIMER_MRWL_LSB  EQU     10h
TIMER_MRWL_MSB  EQU     20h
TIMER_MRWL_LMSB EQU     30h
TIMER_MRWL      EQU     30h

TIMER_MMOD_M0   EQU     00h
TIMER_MMOD_M1   EQU     02h
TIMER_MMOD_M2   EQU     04h
TIMER_MMOD_M3   EQU     06h
TIMER_MMOD_M4   EQU     08h
TIMER_MMOD_M5   EQU     0Ah
TIMER_MMOD      EQU     0Eh

TIMER_MMOD_EVENT   EQU     00h
TIMER_MMOD_ONESHOT EQU     02h
TIMER_MMOD_DIV_N   EQU     04h
TIMER_MMOD_SQUARE  EQU     06h
TIMER_MMOD_SWSTRB  EQU     08h
TIMER_MMOD_HWSTRB  EQU     0Ah

TIMER_MTYP_BIN  EQU     00h
TIMER_MTYP_BCD  EQU     01h
TIMER_MTYP      EQU     01h

TIMER_RBLTC_CS  EQU     00h
TIMER_RBLTC_C   EQU     10h
TIMER_RBLTC_S   EQU     20h
TIMER_RBLTC_NOP EQU     30h
TIMER_RBLTC     EQU     30h

TIMER_RBC2      EQU     08h
TIMER_RBC1      EQU     04h
TIMER_RBC0      EQU     02h

NMI_DIS macro where2store
    	in	al,70h
	mov	where2store,al
	or	al,80h
	out	70h,al
	and	where2store,80H
        in      al,71h
endm

NMI_ENA macro where2store
    	in	al,70h
	mov	where2store,al
	and	al,7Fh
        out	70h,al
	and	where2store,80H
        in      al,71h
endm


NMI_GET macro
    	in	al,70h
endm

NMI_REST macro where_stored
        in	al,70h
	and	al,7Fh
        or      al,where_stored
	out	70h,al
        in      al,71h
endm


SET_PIC0_IRQ_ADDRESS macro int_addr
                mov     al,PIC0_ICW1
                out     20h,al
                PIC_IO_DELAY
                mov     al,int_addr
                out     21h,al
IF PIC0_ICW3_OMMIT EQ 0
                PIC_IO_DELAY
                mov     al,PIC0_ICW3
                out     21h,al
ELSE
  display "PIC0_ICW3 ommited.."
ENDIF

IF PIC0_ICW4_OMMIT eq 0
                PIC_IO_DELAY
                mov     al,PIC0_ICW4
                out     21h,al
ELSE
  display "PIC0_ICW4 ommited.."
ENDIF
                ;;PIC_IO_DELAY
                ;;OPTZER0 al,al
                ;;out     21h,al
endm



SET_PIC1_IRQ_ADDRESS macro int_addr
                mov     al,PIC1_ICW1
                out     0A0h,al
                PIC_IO_DELAY
                mov     al,int_addr
                out     0A1h,al
IF PIC1_ICW3_OMMIT EQ 0
                PIC_IO_DELAY
                mov     al,PIC1_ICW3
                out     0A1h,al
ELSE
  display "PIC1_ICW3 ommited.."
ENDIF

IF PIC1_ICW4_OMMIT EQ 0
                PIC_IO_DELAY
                mov     al,PIC1_ICW4
                out     0A1h,al
ELSE
  display "PIC1_ICW4 ommited.."
ENDIF
                ;;PIC_IO_DELAY
                ;;OPTZER0 al,al
                ;;out     0A1h,al
endm



PIC0_DS_ENS macro disable_mask,enable_mask
                in      al,021h
                PIC_IO_DELAY
IF NOT disable_mask EQ 0
                or      al,disable_mask
ENDIF

IF NOT enable_mask EQ 0FFh
                and     al,enable_mask
ENDIF
                out     021h,al
                PIC_IO_DELAY
endm


PIC1_DS_ENS macro disable_mask,enable_mask
                in      al,0A1h
                PIC_IO_DELAY
IF NOT disable_mask EQ 0
                or      al,disable_mask
ENDIF

IF NOT enable_mask EQ 0FFh
                and     al,enable_mask
ENDIF
                out     0A1h,al
                PIC_IO_DELAY
endm



PIC0_DEF_IMR macro sreg
                mov     al,sreg:[new_pic0_imr]
                out     021h,al
endm

PIC1_DEF_IMR macro sreg
                mov     al,sreg:[new_pic1_imr]
                out     0A1h,al
endm

PIC0_SET_IMR macro
                out     021h,al
endm

PIC1_SET_IMR macro
                out     0A1h,al
endm

PIC0_EOI macro
                mov     al,20h
                out     020h,al
endm

PIC1_EOI macro
                mov     al,20h
                out     0A0h,al
endm
