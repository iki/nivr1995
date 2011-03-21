;*********************************************************** print_pal_int_tt
;*** prints intensity translation table
;*** in :  cl=max_colors,ch=max_intensity,di=offset pal_int_tt
;*** ass:  -
;*** out:  -
;*** mod:  -
print_pal_int_tt proc far
            pusha
            push        ds
            mov         dx,data16
            mov         bl,cl
            OPTZER0     bh
            mov         ds,dx
@@print_block:
            mov         al,bh
            call        PR_ALX
            mov         cl,bl
@@print_row:
            mov         al,[di]
            call        PR_ALX
            inc         di
            dec         cl
            jnz         @@print_row

            DPRINTS     crlf

            inc         bh
            test        bh,15
            jnz         @@testloop
            WAIT_KEY
@@testloop:
            dec         ch
            jnz         @@print_block
            WAIT_KEY
            pop         ds
            popa
            ret
print_pal_int_tt endp

;************************************************************ print_pal_ic_tt
;*** prints intensity/contrast translation table
;*** in :  ah=max_colors,cl=max_contrast,ch=max_intensity,di=offset pal_ic_tt
;*** ass:  -
;*** out:  -
;*** mod:  -
print_pal_ic_tt proc far
            pusha
            push        ds
            mov         dx,data16
            mov         ds,dx
            push        ax
            mov         al,ah
            call        PR_ALX
            mov         ax,cx
            call        PR_AXX
            mov         ax,PAL_INT_STD*256+PAL_CON_STD
            call        PR_AXX
            mov         ax,di
            call        PR_AXX
            mov         ax,word ptr [pal_ic_tt_act]
            call        PR_AXX
            DPRINTS     crlf
            pop         ax
            OPTZER0     bh
            mov         dh,ah
            mov         dl,cl
@@print_block_all:
            OPTZER0     bl
            mov         cl,dl
@@print_block_i:
            mov         ax,bx
            call        PR_AXX
            mov         ah,dh
@@print_row_c:
            mov         al,[di]
            call        PR_ALX
            inc         di
            dec         ah
            jnz         @@print_row_c
            push        dx
            DPRINTS     crlf
            pop         dx
            inc         bl
            dec         cl
            jnz         @@print_block_i

            inc         bh
;            test        bh,15
;            jnz         @@testloop
            WAIT_KEY
;@@testloop:
            dec         ch
            jnz         @@print_block_all
;            WAIT_KEY
            pop         ds
            popa
            ret
print_pal_ic_tt endp

;********************************************************* prepare_pal_int_tt
;*** prepares intensity translation table
;*** in :  cl=max_color_shades,ch=max_intensity,di=offset pal_int_tt
;*** ass:  -
;*** out:  data16:di (pal_int_tt)
;*** mod:  -
prepare_pal_int_tt proc far
            pusha
            push        ds
            mov         ax,data16
            mov         si,di
            mov         ds,ax
;            mov         dx,cx
;            mov         al,0
;@@fill_first:
;            mov         [di],al
;            inc         di
;            dec         cl
;            jnz         @@fill_first

;            OPTZER0     bh
;            mov         bl,dl
;            mov         al,dl
;            mov         cl,dl
;            mov         di,si
;            mul         dh
;            sub         di,bx
;            mov         bp,ax
;            add         di,ax
;            mov         al,DAC_MASK
;@@fill_last:
;            mov         [di],al
;            inc         di
;            dec         cl
;            jnz         @@fill_last

            mov         dl,cl
            OPTZER0     dh
            mov         cl,ch
            mov         bx,cx
            shr         cl,1
            cmp         cl,bh
            adc         cl,0
            sub         ch,cl
            cmp         ch,bh
            adc         ch,0
            OPTZER0     ax
            call        interpol_lin

            sub         di,dx
            mov         ah,DAC_MASK
            xchg        cl,ch
            call        interpol_lin

            mov         di,si
            add         di,dx
            xchg        cl,ch
            dec         di
            call        interpol_lin

            sub         di,dx
            mov         al,DAC_MASK
            xchg        cl,ch
            call        interpol_lin

            mov         di,si
            mov         cx,dx
            dec         si
            mov         dx,1
@@line_interpol:
            add         si,cx
            mov         al,[di]
            mov         ah,[si]
            call        interpol_lin
            dec         bh
            jnz         @@line_interpol

            pop         ds
            popa
            ret
prepare_pal_int_tt endp

;************************************************************** print_palette
;*** prints palette
;*** in :  si=off palette,dl=num_per_row,dh=blockrows,bl=blocks,bh=pgtestmask
;*** ass:  -
;*** out:  -
;*** mod:  -
print_palette proc far
            pusha
            push        ds
            mov         cx,data16
            mov         di,si
            mov         ds,cx
            OPTZER0     ah
@@print_all:
            mov         ch,dh
@@print_block:
            push        ax
            mov         si,di
            mov         cl,dl
            mov         al,ah
            call        PR_ALX
            OPTZER0     ah
@@print_row:
            mov         al,[si]
            call        PR_ALX

            mov         al,dh
            add         si,ax
            dec         cl
            jnz         @@print_row

            mov         bp,dx
            DPRINTS     crlf
            mov         dx,bp

            inc         di
            pop         ax
            dec         ch
            jnz         @@print_block

            push        ax
            OPTZER0     ah
            mov         al,dh
            sub         si,ax
            inc         si
            mov         di,si
            pop         ax

            inc         ah
            test        ah,bh
            jnz         @@not_eopg
            WAIT_KEY
@@not_eopg: dec         bl
            jnz         @@print_all
            pop         ds
            popa
            ret
print_palette endp


vga_vr_ena_irq2 proc far
           pushf
           push    dx ax
           cli
           mov     dx,VGA_IOMSBW+VGA_C_ADDR_W

           mov     al,VGA_CI_HBE
           out     dx,al
           inc     dl

           in      al,dx
           or      al,80h
;           mov     ah,al

;           dec     dl
;           mov     al,VGA_CI_HBE
;           out     dx,al
;           inc     dl

;           mov     al,ah
           out     dx,al

;;           dec     dl
;;           mov     al,VGA_CI_HBE
;;           out     dx,al
;;           inc     dl

;;	     in	     al,dx
;;	     call    PR_ALB

;----------------------------

           dec     dl
           mov     al,VGA_CI_VRE
           out     dx,al
           inc     dl

           in      al,dx
           and     al,4fh
           ;or      al,10h
;           mov     ah,al

;           dec     dl
;           mov     al,VGA_CI_VRE
;           out     dx,al
;           inc     dl

;           mov     al,ah
           out     dx,al

;;           dec     dl
;;           mov     al,VGA_CI_VRE
;;           out     dx,al
;;           inc     dl

;;           in      al,dx
;;	     call    PR_ALB

;----------------------------

           dec     dl
           mov     al,VGA_CI_HBE
           out     dx,al
           inc     dl

           in      al,dx
           or      al,80h
;           mov     ah,al

;           dec     dl
;           mov     al,VGA_CI_HBE
;           out     dx,al
;           inc     dl

;           mov     al,ah
           out     dx,al

;----------------------------

           dec     dl
           mov     al,VGA_CI_VRE
           out     dx,al
           inc     dl

           in      al,dx
           and     al,4fh
           ;or      al,10h
;           mov     ah,al

;           dec     dl
;           mov     al,VGA_CI_VRE
;           out     dx,al
;           inc     dl

;           mov     al,ah
           out     dx,al

@@return:
           popf
           pop     ax dx
           ret
vga_vr_ena_irq2 endp

vrtsth1 proc far
           push    ax bx dx si ds
           mov     dx,VGA_IOMSBW+VGA_M_INS0_R
           in      al,dx

           mov     bl,0ffh
           test    al,80h
           jz      @@is0_reg_nok

           mov     dx,VGA_IOMSBW+VGA_C_ADDR_W
           mov     al,VGA_CI_VRE
           out     dx,al
           inc     dl

           in      al,dx
           and     al,0FEh
           mov     ah,al

           dec     dl
           mov     al,VGA_CI_VRE
           out     dx,al
           inc     dl

           mov     al,ah
           out     dx,al

           mov     bl,1
@@is0_reg_nok:

           mov     ax,data16
           mov     si,offset PAL
           mov     ds,ax
           mov     dx,VGA_IOMSBW+VGA_PEL_WA_RW
           in      al,dx
           mov     ah,al
           OPTZER0 al
           out     dx,al
           inc     dl
@@incnout: add     [si],bl
           mov     al,[si]
           out     dx,al
           add     si,3
           cmp     si,offset PAL+15*3
           jb      @@incnout

           dec     dl
           mov     al,ah
           out     dx,al

;           pop     ds si dx bx ax
;           JUMPFAR code16,keyboard_handler

           mov     al,20h
           out     20h,al
           pop     ds si dx bx ax
           iret
vrtsth1 endp

vrtsth2 proc far
           push    ax
           add     word ptr fs:[MAN_A],MAN_A_STEP
           or      fs:[request_flags],REQF_RECALC_VIEW

           mov     ax,0Bh
           out     020h,al
           PIC_IO_DELAY
           in      al,020h
           PIC_IO_DELAY
           or      ah,al
           jz      @@pic1_a
           test    ah,1
           jnz     @@pic0_eoi0
           test    ah,2
           jnz     @@pic0_eoi0
           test    ah,4
           jnz     @@pic0_eoi1
@@pic0_eoi0:
           mov     al,20h
           out     020h,al
           jmp     short @@return
@@pic0_eoi1:
           mov     al,20h
           out     020h,al
           PIC_IO_DELAY
@@pic1_b:
           mov     al,0Bh
           out     0a0h,al
           PIC_IO_DELAY
           in      al,0a0h
           PIC_IO_DELAY
           or      al,al
           jnz     @@pic1_eoi
           ;;test    ah,255-1-2-4
           ;;jz      @@return
           ;;mov     al,20h
           ;;out     020h,al
           jmp     short @@return
@@pic1_a:
           mov     al,0Bh
           out     0a0h,al
           PIC_IO_DELAY
           in      al,0a0h
           PIC_IO_DELAY
           or      al,al
           jz      @@return
@@pic1_eoi:
           mov     al,20h
           out     0a0h,al
@@return:
           mov     dx,VGA_IOMSBW+VGA_M_INS0_R
           in      al,dx

           test    al,80h
           jz      @@return2

           mov     dl,VGA_C_ADDR_W
           mov     al,VGA_CI_VRE
           out     dx,al
           inc     dl

           in      al,dx
           and     al,0FEh
           out     dx,al
@@return2:

           pop     ax
           iret
vrtsth2 endp