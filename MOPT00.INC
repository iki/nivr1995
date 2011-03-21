OPTLOOPB macro loop_label
        dec     cl
        jnz     loop_label
endm


OPTLOOP macro loop_label
;;       loop    loop_label
        dec     cx
        jnz     loop_label
endm

OPTLOOPD macro loop_label
;;       db      67h
;;       loop    loop_label
        dec     ecx
        jnz     loop_label
endm

OPTZER0 macro zer0_var
;;       mov     zer0_var,0
        xor	zer0_var,zer0_var
endm

OPTDESTRTEST macro tested_var,mask
;;        test    tested_var,mask
        and    tested_var,mask
endm

OPTTESTZER0 macro tested_var
;;        cmp    tested_var,0
;;        or     tested_var,tested_var
        test   tested_var,tested_var
endm

OPTTESTZER0D macro tested_var
;;        cmp    tested_var,0
;;        or     tested_var,tested_var
        test   tested_var,tested_var
endm

OPTTESTZER0MEM macro tested_var
        cmp    tested_var,0
;;        or     tested_var,tested_var
;;        test   tested_var,tested_var
endm

OPTACCESSWTDA macro sreg,areg,dstreg
        mov    dstreg,sreg:[areg*2]
ENDM
