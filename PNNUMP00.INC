;******************************************************* PS/PR+_+EX/AX/AL+X/B
;*** prints EAX/AX/AL in heX or Bin
;*** in :  eax/ax/al - number to print
;*** out:  -
;*** ass:  int_21h/fn_2 doesn't change general regs
;*** mod:  -

PR_EXX      proc far
     ror    eax,16
     call   PR_AXX
     rol    eax,16
PR_AXX	    proc far
     push   cx
     mov    cx,12
     jmp    short pr_xxx_cont16
PR_ALX      proc far
     push   cx
     mov    cx,4
pr_xxx_cont16:
     push   bx
     mov    bx,40fh
     jmp    short pr_xxx_cont
PR_EXB	    proc far
     ror    eax,16
     call   PR_AXB
     rol    eax,16
PR_AXB      proc far
     push   cx
     mov    cx,15
     jmp    short pr_xxx_cont2
PR_ALB      proc far
     push   cx
     mov    cx,7
pr_xxx_cont2:
     push   bx
     mov    bx,101h
pr_xxx_cont:
     push   ax dx si
     mov    si,ax
@@next_n:
     mov    dx,si
     shr    dx,cl
     and    dl,bl
     cmp    dl,09h
     ja     @@abcdef
     add    dl,30h
     jmp    @@dos_pr
@@abcdef:
     add    dl,37h
@@dos_pr:
     mov    ah,2
     int    21h
     sub    cl,bh
     cmp    cl,0fh
     jb     @@next_n
     pop    si dx ax bx cx
     ret
PR_ALB      endp
PR_AXB      endp
PR_EXB      endp
PR_ALX      endp
PR_AXX      endp
PR_EXX      endp


PS_EXX      proc far
     ror    eax,16
     call   PS_AXX_not_zer0
     or     ax,ax
     jz     @@exx_space
     rol    eax,16
     jmp    PR_AXX
@@exx_space:
     rol    eax,16
PS_AXX      proc far
     or     ax,ax
     jz     ps_pnum_zer0
PS_AXX_not_zer0 proc far
     push   cx
     mov    cx,12
     jmp    short ps_xxx_cont16
PS_ALX	    proc far
     or     al,al
     jz     ps_pnum_zer0
     push   cx
     mov    cx,4
ps_xxx_cont16:
     push   bx
     mov    bx,40fh
     jmp    short ps_xxx_cont
PS_EXB	    proc far
     ror    eax,16
     call   PS_AXB_not_zer0
     or     ax,ax
     jz     @@exb_space
     rol    eax,16
     jmp    PR_AXX
@@exb_space:
     rol    eax,16
PS_AXB	    proc far
     or     ax,ax
     jz     ps_pnum_zer0
PS_AXB_not_zer0 proc far
     push   cx
     mov    cx,15
     jmp    short ps_xxx_cont2
PS_ALB	    proc far
     or     al,al
     jz     ps_pnum_zer0
     push   cx
     mov    cx,7
ps_xxx_cont2:
     push   bx
     mov    bx,101h
ps_xxx_cont:
     push   ax dx si di
     mov    di,0
     mov    si,ax
@@next_n:
     mov    dx,si
     shr    dx,cl
     and    dl,bl
     cmp    dl,0
     jne    @@notzero
     cmp    di,0
     jne    @@0to9
     jmp    short @@endprlp
@@notzero:
     mov    di,1
     cmp    dl,09h
     ja     @@abcdef
@@0to9:
     add    dl,30h
     jmp    @@dos_pr
@@abcdef:
     add    dl,37h
@@dos_pr:
     mov    ah,2
     int    21h
@@endprlp:
     sub    cl,bh
     cmp    cl,0fh
     jb     @@next_n
     pop    di si dx ax bx cx
     ret
ps_pnum_zer0:
     push   ax dx
     mov    ah,2
     mov    dl,30h
     int    21h
     pop    dx ax
     ret
PS_ALB      endp
PS_AXB_not_zer0 endp
PS_AXB      endp
PS_EXB      endp
PS_ALX      endp
PS_AXX_not_zer0 endp
PS_AXX      endp
PS_EXX      endp
