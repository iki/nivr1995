;********************************************************************* cpu_id
;*** checks the CPU type
;*** in :  -
;*** out:  dx=offset mix86; al=CPU
;***       data16:db CPU,CPU_mode,CPU_cpuid,CPU_stepping,CPU_model
;*** ass:  data16:m*;
;*** mod:  eax,ebx,ecx,edx

cpu_id proc far
           push    ds
           mov     ax,data16
           mov     ds,ax
           cmp     CPU,NCH
           jnz     cpu_id_e
@@check86:
; 8086 check : Flags 12-15 are always set on XT
           mov     CPU,I86
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
; 8086 check (2) : 86 - SP incs before push # x86 - SP incs after
           push    sp
           pop     ax
           cmp     ax,sp
           jne     @@check_e
@@check86_e:
@@checkpm:
; Protected mode check
           ;mov     dx,offset mrm    ; OITP
           smsw    ax
           test    al,01h
           jz      @@checkpm_e
           or      CPU_mode,CPU_mode_PM
           ;mov     dx,offset mpm    ; OITP
@@checkpm_e:
           ;mov     ah,9             ; OITP!
           ;int     21h              ; OITP!
@@check286:
; 80286 check : Flags 12-15 are always NOT set in Real Mode
           mov     CPU,I286
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
           mov     CPU,I386
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
           mov     CPU,I486
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
           test    eax,200000h      ; ID flag bit = 1 -> CPUID
           jz      @@check_e
           mov     CPU_cpuid,1
           xor     eax,eax
           M_CPUID
           mov     dword ptr [CPU_vendor],ebx
           mov     dword ptr [CPU_vendor+4],edx
           mov     dword ptr [CPU_vendor+8],ecx
           cmp     eax, 1
           mov     dx,offset mi486  ; OITP
           jb      cpu_id_e
           mov     CPU_cpuid,2
           mov     eax,1
           M_CPUID
           mov     dx,offset mi486  ; OITP
           mov     CPU_stepping,al
           and     CPU_stepping,CPUID_STEPPING_MSK
           ;and     al,CPUID_MODEL_MSK
           shr     al,CPUID_MODEL_SHR
           mov     CPU_model, al
           and     ah,CPUID_FAMILY_MSK/100h
           mov     CPU_feature,edx
           cmp     ah,5
           jb      @@check_e
           mov     dx,offset mi586  ; OITP
@@check_e:
cpu_id_e:
           mov     al,CPU
           pop     ds
           ret
cpu_id endp

;***************************************************************** KBC_flush
;*** waits until Keyboard Controller input buffer flushed (synchronizes 8042)
;*** in :  -
;*** out:  ZF=0 <=> error
;*** ass:  -
;*** mod:  CX=0 <=> error (99,99%); AL#0 <=> error

KBC_flush proc far
                xor     cx,cx
@@insync:       in      al,KBC_CMD_PORT
                test    al,KBC_IBF_BIT
                loopnz  @@insync
                ret
KBC_flush endp


;******************************************* set_a20, set_a20_on, set_a20_off
;*** sets A20 to the desired state
;*** in :  AL=0 <=> disable
;*** out:  AL=0 => failed; AL=1 => succeeded
;*** ass:  -
;*** mod:  AH=a20_on/off_data; CX
set_A20 proc far
                or      al,al
                jnz     set_a20_on
set_a20_off proc far
	        mov     ah,A20_OFF_DATA
                jmp     short set_A20_onoff_start
set_a20_on proc far
                mov     ah,A20_ON_DATA
set_A20_onoff_start:
                call    KBC_flush
                jnz     @@a20_err

                mov     al,KBC_WOP_CMD
                out     KBC_CMD_PORT,al
                call    KBC_flush
                jnz     @@a20_err

                mov     al,ah
                out     KBC_DAT_PORT,al
                call    KBC_flush
                jnz     @@a20_err

                mov     al,KBC_NOP_CMD
                out     KBC_CMD_PORT,al
                call    KBC_flush
                jnz     @@a20_err
                mov     al,1
                ret
@@a20_err:      xor     al,al
                ret
set_a20_on endp
set_a20_off endp
set_a20 endp


;******************************************************************** tst_a20
;*** tests a20 state
;*** in :  -
;*** out:  AL=0 => disabled; AL=1 => enabled
;*** ass:  -
;*** mod:  AH=0; CX=0 <=> disabled

tst_a20 proc far
                push    ds
                push    es
                xor     ax,ax
		mov     ds,ax
                mov     si,ax
		dec     ax
		mov     es,ax
		mov     di,10h ;constant (SI)+10h
                mov     cx,10h ;it should be enough
                cld
                repz    cmpsw
                pop     es
                pop     ds
                inc     ax
                jcxz    @@a20_off
                inc     al
@@a20_off:      ret
tst_a20 endp


;******************************************************* rfm_enter, rfm_leave
;*** switch to protected mode, set ?S limit to 4GB/64KB and back to real
;*** in :  -
;*** out:  ?s.limit=0FFFFh*4KB=4GB or ?s.limit=0FFFFh*1B=64KB
;*** ass:  real mode, data16:rfm_gdt, cs=code16
;*** mod:  es=fs=ds=data16, gs=0

rfm_enter proc far
                pushad
                pushf
                mov     dl,8fh
                jmp     short rfm_le_go_on
rfm_leave proc far
                pushad
                pushf
                OPTZER0 dl,dl
rfm_le_go_on:
		cli
                NMI_DIS dh

                mov     eax,data16
                mov     ds,ax
                mov     es,ax

                mov     byte ptr [rfm_gdt+RFM_GDT_CODE16+6],dl
                mov     byte ptr [rfm_gdt+RFM_GDT_DATA16+6],dl
                ;mov     byte ptr [rfm_gdt+RFM_GDT_STACK16+6],dl

                or      dl,dl
                jz      @@save_igdt_e

                sgdt    fword ptr sav_gdtr
                sidt    fword ptr sav_idtr

                ;mov     ax,data16
                mov     di,offset rfm_idt
                ;and     eax,0ffffh
                and     edi,0ffffh
		shl     eax,4
                mov     esi,[sav_idt_ptr]
                add     eax,edi
                mov     ebx,esi
                mov     cx,RFM_IDT_LIMIT/4+1
                and     esi,0fh
                shr     ebx,1
                mov     [rfm_idt_ptr],eax
                mov     ds,bx
                rep movsd

                mov     ax,data16
                mov     ds,ax

		lgdt	fword ptr rfm_gdtr
                lidt    fword ptr rfm_idtr

                mov     si,PIC0_OLD_IRQ_ADDR
                mov     di,offset new_irqs
                shl     si,2
                mov     cx,8
                add     si,offset rfm_idt
                rep     movsd

                mov     si,PIC1_OLD_IRQ_ADDR
                mov     cx,8
                shl     si,2
                add     si,offset rfm_idt
                rep     movsd

                mov     si,offset new_irqs
                mov     di,offset old_irqs
                mov     cx,16
                rep     movsd


                in      al,021h
                PIC_IO_DELAY
                mov     [save_pic0_imr],al
                in      al,0A1h
                PIC_IO_DELAY
                mov     [save_pic1_imr],al

                mov     al,0FFh
                out     021h,al
                PIC_IO_DELAY
                out     0A1h,al
                ;;PIC_IO_DELAY


                mov     al,PIC0_NEW_IRQ_ADDR
                call    set_pic0_irqaddr

                mov     al,PIC1_NEW_IRQ_ADDR
                call    set_pic1_irqaddr

                SET_INT_VECT 0,ds,division_handler
                SET_IRQ_VECT 1,ds,empty_handler

                mov     al,[save_pic0_imr]
                and     al,PIC0_IMR_AND
                or      al,PIC0_IMR_OR
                mov     [new_pic0_imr],al
                out     021h,al
                ;;PIC_IO_DELAY

                mov     al,[save_pic1_imr]
                and     al,PIC1_IMR_AND
                or      al,PIC1_IMR_OR
                mov     [new_pic1_imr],al
                out     0A1h,al
                ;;PIC_IO_DELAY

@@save_igdt_e:
		;;mov 	  cx,cs
                ;;mov     cs:[@@self_mod_cs],cx
                ;;mov     cx,es

		mov	eax,cr0
                mov     bx,ss
		or	al,1			; set PE bit
                SET_INT_VECT 3,ds,empty_handler
		mov	cr0,eax			; protection enabled
		DB	0EAh
                DW      (offset @@csfix_pmode)
                DW      RFM_GDT_CODE16  	; purge queue and fix CS
@@csfix_pmode:
		mov	ax,RFM_GDT_DATA16
		mov	ds,ax
		mov	es,ax
		mov	fs,ax
		mov	gs,ax
                mov	ax,RFM_GDT_STACK16
                mov	ss,ax

		mov	eax,cr0
		and	al,NOT 01		; clear PE bit
		mov	cr0,eax			; protection disabled

		DB	0EAh
                DW	(offset @@csfix_rmode)
@@self_mod_cs:	DW	(seg @@csfix_rmode)
@@csfix_rmode:
                mov     ax,data16
		OPTZER0	cx
		mov	ss,bx
                mov	ds,ax
		mov	fs,ax
		mov	es,ax
		mov	gs,cx

                or      dl,dl
                jnz     @@rest_igdt_e

                ;mov     si,offset rfm_idt
                ;mov     edi,[sav_idt_ptr]
                ;mov     ebx,edi
                ;mov     cx,RFM_IDT_LIMIT/4+1
                ;and     edi,0fh
                ;shr     ebx,1
                ;mov     es,bx
                ;rep movsd

                mov     al,0FFh
                out     021h,al
                PIC_IO_DELAY
                out     0A1h,al
                PIC_IO_DELAY

                SET_PIC0_IRQ_ADDRESS PIC0_OLD_IRQ_ADDR
                PIC_IO_DELAY
                SET_PIC1_IRQ_ADDRESS PIC1_OLD_IRQ_ADDR
                PIC_IO_DELAY

                mov     al,[save_pic0_imr]
                out     021h,al
                PIC_IO_DELAY

                mov     al,[save_pic1_imr]
                out     0A1h,al
                PIC_IO_DELAY

                lgdt    fword ptr sav_gdtr
                lidt    fword ptr sav_idtr
@@rest_igdt_e:

                NMI_REST dh

		popf
                popad
		ret
rfm_leave endp
rfm_enter endp

;*********************************************************** set_pic0_irqaddr
;*** sets base interrupt for irqs 0-7
;*** in :  al=new_int
;*** out:  -
;*** ass:  data16:new_irqs,rfm_idt;
;*** mod:  -

set_pic0_irqaddr proc    near
                pushf
                cli
                pusha
                push    ds es
                mov     cx,data16
                and     ax,0f8h
                mov     si,offset new_irqs
                mov     di,ax
                mov     ds,cx
                mov     es,cx
                shl     di,2
                mov     ah,al
                mov     cx,8
                add     di,offset rfm_idt
                rep     movsd

                SET_PIC0_IRQ_ADDRESS ah

                pop     es ds
                popa
                popf
                ret
set_pic0_irqaddr endp

;*********************************************************** set_pic1_irqaddr
;*** sets base interrupt for irqs 8-F
;*** in :  al=new_int
;*** out:  -
;*** ass:  data16:new_irqs,rfm_idt;
;*** mod:  -

set_pic1_irqaddr proc    near
                pushf
                cli
                pusha
                push    ds es
                mov     cx,data16
                and     ax,0f8h
                mov     si,offset new_irqs+8*4
                mov     di,ax
                mov     ds,cx
                mov     es,cx
                shl     di,2
                mov     ah,al
                mov     cx,8
                add     di,offset rfm_idt
                rep     movsd

                SET_PIC1_IRQ_ADDRESS ah

                pop     es ds
                popa
                popf
                ret
set_pic1_irqaddr endp