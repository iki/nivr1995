;*************************************************************** interpol_lin
;*** performs linear interpolation from STARTVAL to ENDVAL into
;*** DESTOFF+x*DESTSTEP (x goes from 0 to DESTN-1)
;*** in :  ds:di=DESTOFF, dx=DESTSTEP, al=STARTVAL, ah=ENDVAL, cl=DESTN
;*** ass:  -
;*** out:  -
;*** mod:  di=LAST+DESTN
interpol_lin proc far
        cmp     cl,1
        je      @@line_one
        jb      @@line_null
        push    ax cx ebx
        sub     ah,al
        je      @@fill_only
        jb      @@inversed_ranges
        OPTZER0 ebx
        dec     cl
        mov     bh,ah
        mov     bl,cl
        mov     ah,al
        mov     bx,gs:[2*ebx+DIVTAB_8_8_8x8]
        ;OPTZER0 al
        mov     al,80h
@@a_interpol:
        mov     [di],ah
        add     di,dx
        add     ax,bx
        dec     cl
        jnz     @@a_interpol
        pop     ebx cx ax
        mov     [di],ah
        add     di,dx
;        pop     di
        ret
@@line_one:
        push    ax
        shr     ax,1
        and     al,7fh
        add     al,ah
        mov     [di],al
        add     di,dx
        pop     ax
@@line_null:
        ret
@@fill_only:
        mov     [di],al
        add     di,dx
        dec     cl
        jnz     @@fill_only
        pop     ebx cx ax ;di
        ret

@@inversed_ranges:
        neg     ah
        OPTZER0 ebx
        dec     cl
        mov     bh,ah
        mov     bl,cl
        mov     ah,al
        mov     bx,gs:[2*ebx+DIVTAB_8_8_8x8]
        ;OPTZER0 al
        mov     al,80h
@@b_interpol:
        mov     [di],ah
        add     di,dx
        sub     ax,bx
        dec     cl
        jnz     @@b_interpol
        pop     ebx cx ax
        mov     [di],ah
        add     di,dx
;        pop     di
        ret
interpol_lin endp

;*************************************************************** interpol_lin

IF 0 EQ 1

interpol_lin2 proc far
        cmp     cl,1
        je      @@line_one
        jb      @@line_null
        push    ax bx cx
        sub     ah,al
        je      @@fill_only
        jb      @@inversed_ranges
        mov     bh,al
        dec     cl
        mov     al,ah
        OPTZER0 ah
;;;;    call    PR_AXX
        div     cl
        mov     bl,al
;;;;    call    PR_AXX
        mov     ch,128
        OPTZER0 al
        div     cl
;;;;    call    PR_AXX
        xchg    al,bh
@@a_interpol:
        mov     [di],al
        add     ch,bh
        adc     al,bl
        add     di,dx
        dec     cl
        jnz     @@a_interpol
        pop     cx bx ax
        mov     [di],ah
        add     di,dx
;        pop     di
        ret
@@line_one:
        push    ax
        shr     ax,1
        and     al,7fh
        add     al,ah
        mov     [di],al
        add     di,dx
        pop     ax
@@line_null:
        ret
@@fill_only:
        mov     [di],al
        add     di,dx
        dec     cl
        jnz     @@fill_only
        pop     cx bx ax ;di
        ret

@@inversed_ranges:
        mov     bh,al
        OPTZER0 al
        dec     cl
        sub     al,ah
        OPTZER0 ah
;;;;    call    PR_AXX
        div     cl
        mov     bl,al
;;;;    call    PR_AXX
        mov     ch,128
        OPTZER0 al
        div     cl
;;;;    call    PR_AXX
        xchg    al,bh
@@b_interpol:
        mov     [di],al
        sub     ch,bh
        sbb     al,bl
        add     di,dx
        dec     cl
        jnz     @@b_interpol
        pop     cx bx ax
        mov     [di],ah
        add     di,dx
;        pop     di
        ret
interpol_lin2 endp

ENDIF