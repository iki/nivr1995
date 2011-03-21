;********************************************************************** error
;*** informs about error
;*** in :  al-(b7:exit/;b6:askex/;b2:req/;b1:err/;b0:warn/);dx,cx,bx-msg_offs
;*** out:  -
;*** ass:  data16:error_n,error_msg,m*;
;*** mod:  dx,ax,di,si
error           proc far
                push    ds
                mov     dx,data16
		cmp	error_n,0
                mov     ds,dx
		jz 	@@err_return
		mov	al,error_t
                test    al,err_exit1
                jnz     @@err_noreq
                test    al,err_err+err_wrn
                jz      @@err_noerrwrn
		xor	ch,ch
                xor     bx,bx
                mov     cl,error_n
                mov     si,offset merr
                mov     di,offset merr2
                test    al,err_err
                jnz     @@err_nowrn
                mov     si,offset mwrn
                mov     di,offset mwrn2
@@err_nowrn:
                mov     dx,[error_msg+bx]
                or      dx,dx
                jz      @@err_ewdx_e
                mov     ah,9
                mov     dx,si
                int     21h
                mov     si,di
                mov     dx,[error_msg+bx]
                mov     ah,9
                int     21h
@@err_ewdx_e:   add     bl,4
                loop    @@err_nowrn
@@err_errwrn_e:
@@err_noerrwrn: mov	error_n,0
		mov	al,error_t
                test    al,err_exit
                jz      @@err_noexit
                test    al,err_askex
                jz      @@err_noaskex
                mov     ah,9
                mov     dx,si
                int     21h
                DPRINTS mcont
                mov     ah,8
                int     21h
                cmp     al,'y'
                je      @@err_noexit
                cmp     al,'Y'
                je      @@err_noexit
@@err_noaskex:
                test    error_t,err_req
                jz      @@err_noreq
                DPRINTS mreq
                pop     ds
@@err_noreq:    and      error_t,not (err_exit+err_exit1)
                EXIT
@@err_noexit:
@@err_return:   pop     ds
                ret
error           endp