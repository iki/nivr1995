;*************************************************************** printdfsinfo
;*** prints iinfo on current state of datafile-system
;*** in :  -
;*** ass:  all the structures&datas related to dfs
;*** out:  -
;*** mod:  -
printdfsinfo proc far

        pushad
        push    ds
        mov     dx,data16
        mov     ds,dx
        DPRINTC 10
        DPRINTC 13
        DPRINTC 'h'
        mov     ax,DFS_FATTAB_ACTHANDLE
        call    PR_AXX
        DPRINTC 'e'
        mov     ax,DOS_ERRN
        call    PR_AXX
        DPRINTC 'f'
        mov     ax,DFS_FATTAB_ENTRIES
        call    PR_AXX
        DPRINTC 'o'
        mov     ax,DFS_FATTAB_OPENS
        call    PR_AXX
        DPRINTC 'a'
        mov     eax,DFS_FATTAB_ADDR
        call    PR_EXX
        DPRINTC 's'
        mov     eax,DFS_FATTAB_SIZE
        call    PR_EXX
        DPRINTC 'r'
        mov     al,DFS_ERR_OPEN
        call    PR_ALB
        DPRINTC 'r'
        mov     al,DFS_ERR_CLOSE
        call    PR_ALB

        mov     cx,DFS_FATTAB_ENTRIES
        mov     esi,DFS_FATTAB_ADDR
@@print_fat_nfo:
        DPRINTC 10
        DPRINTC 13
        DPRINTC ' '
        mov     al,gs:[esi]
        call    PR_ALB
        cmp     word ptr gs:[esi],0
        je      @@dont_print
        DPRINTC 'n'
        mov     ax,gs:[esi+DFS_FT_NFILES]
        call    PR_AXX
        DPRINTC 'b'
        mov     eax,gs:[esi+DFS_FT_DDKBSBEG]
        call    PR_EXX
        DPRINTC 's'
        mov     eax,gs:[esi+DFS_FT_SEEKPOS]
        call    PR_EXX
        DPRINTC 'd'
        mov     eax,gs:[esi+DFS_FT_DDSIZE]
        call    PR_EXX
        DPRINTC 'f'
        mov     eax,gs:[esi+DFS_FT_FATSIZE]
        call    PR_EXX
        DPRINTC 'a'
        mov     eax,gs:[esi+DFS_FT_FATADDR]
        call    PR_EXX
        DPRINTC 'h'
        mov     ax,gs:[esi+DFS_FT_DOSFILEHANDLE]
        call    PR_AXX
        DPRINTC 'o'
        mov     ax,gs:[esi+DFS_FT_DFNAMEOFF]
        call    PR_AXX
        DPRINTC 'b'
        mov     ax,gs:[esi+DFS_FT_FATBLOCKS]
        call    PR_AXX

@@dont_print:
        add     ebx,DFS_FATTAB_ENTRYSIZE
        OPTLOOP @@print_fat_nfo

        WAIT_KEY
        DPRINTC 10
        DPRINTC 13

        pop     ds
        popad
        ret
printdfsinfo endp