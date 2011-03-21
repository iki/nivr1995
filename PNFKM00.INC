;this version supports intensity only
;********************************************************** prepare_pal_ic_tt
;*** prepares intensity/contrast translation table
;*** in :  ah=max_color_shades,cl=max_contrast,ch=max_intensity,di=offset pal_int_tt
;*** ass:  -
;*** out:  data16:di (pal_int_tt)
;*** mod:  -
prepare_pal_ic_tt proc far
            pusha
            mov         cl,ah
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
prepare_pal_ic_tt endp