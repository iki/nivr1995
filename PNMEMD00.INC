;************************************************************** printheapinfo
;*** prints info on heap
;*** in :  -
;*** ass:  all the structures related to heap management
;*** out:  -
;*** mod:  -
printheapinfo proc far

        push    eax ebx ecx edx ds
        mov     dx,data16
        mov     ds,dx
        DPRINTC 10
        DPRINTC 13
        DPRINTC 'n'
        mov     eax,heapheadentries
        call    PR_EXX
        DPRINTC 'z'
        mov     eax,heapheade0
        call    PR_EXX
        DPRINTC 's'
        mov     eax,heapsize
        call    PR_EXX
        DPRINTC 'h'
        mov     eax,offset HEAPHEAD
        call    PR_EXX
        DPRINTC 'b'
        mov     eax,offset HEAPSTART
        call    PR_EXX
        DPRINTC 't'
        mov     eax,heaptop
        call    PR_EXX
        DPRINTC 'f'
        mov     al,mms_flags
        call    PR_ALB

        DPRINTC 10
        DPRINTC 13

        mov     ecx,[heapheadentries]
        mov     ebx,offset HEAPHEAD
@@find1stlarger:
        DPRINTC ' '
        mov     eax,gs:[ebx]
        call    PR_EXX
        DPRINTC ','
        mov     eax,gs:[ebx+4]
        call    PR_EXX
        add     ebx,8
        OPTLOOPD @@find1stlarger
        WAIT_KEY
        DPRINTC 10
        DPRINTC 13

        pop     ds edx ecx ebx eax
        ret
printheapinfo endp