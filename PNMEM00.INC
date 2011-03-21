MMS_F_FREEMEMONEXIT  EQU 1
MMS_F_FREEFATONCLOSE EQU 2
MMS_F_FREETABONCLOSE EQU 4
MMS_F_FREETABIFNFAT  EQU 8
MMS_F_EXITONERR      EQU 10h

;******************************************************************* initheap
;*** inits heap
;*** in :  -
;*** ass:  all the structures related to heap management
;*** out:  -
;*** mod:  -
initheap proc far
        push    ebx
        mov     ebx,offset HEAPHEAD+8
        mov     [heapheadentries],1
        mov     [heapheade0],1
@@zeroheaphead:
        mov     dword ptr gs:[ebx],0
        add     ebx,4
        cmp     ebx,offset HEAPSTART
        jae     @@zeroheaphead
        mov     dword ptr gs:[HEAPHEAD],offset HEAPSTART
        mov     ebx,[EXT_TOP]
        mov     [heaptop],ebx
        sub     ebx,offset HEAPSTART
        mov     [heapsize],ebx
        mov     dword ptr gs:[HEAPHEAD+4],ebx
        pop     ebx
        ret
initheap endp

;********************************************************************* allocp
;*** alloc permanent memory (at the top of heap)
;*** in :  eax=size
;*** ass:  all the structures related to heap management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
allocp proc far
        test    al,MMS_ALIGN_C0-1
        jz      @@size_aligned
        or      al,MMS_ALIGN_C0-1
        inc     eax
@@size_aligned:
        sub     dword ptr gs:[HEAPHEAD+4],eax
        jb      @@err
        mov     eax,dword ptr gs:[HEAPHEAD]
        add     eax,dword ptr gs:[HEAPHEAD+4]
@@return:
        ret
@@err:
        add     dword ptr gs:[HEAPHEAD+4],eax
;        jmp far alloc
allocp endp

;********************************************************************** alloc
;*** alloc memory (at the bottom of heap)
;*** in :  eax=size
;*** ass:  all the structures related to heap management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
alloc proc far
        push    ebx ecx edx esi ds
        test    al,MMS_ALIGN_C0-1
        jz      @@size_aligned
        or      al,MMS_ALIGN_C0-1
        inc     eax
@@size_aligned:
        OPTZER0 edx
        mov     ecx,[heapheadentries]
        mov     ebx,offset HEAPHEAD+4
        mov     ds,dx
@@find1stlarger:
        cmp     DS:[ebx],eax
        ja      @@foundlarger
        je      @@foundexact
        add     ebx,8
        OPTLOOPD @@find1stlarger
@err:   test    FS:[mms_flags],MMS_F_EXITONERR
        jz      @@err_ret
        mov     bx,data16
        mov     ds,bx
        REG_ERROR merrmmsa0
        GEN_ERROR err_exit+err_err
@@err_ret:
        OPTZER0  eax
;        jmp     short @@return
        pop     ds esi edx ecx ebx
        ret
@@foundexact:
        mov     dword ptr DS:[ebx],0
        mov     eax,DS:[ebx-4]
        lea     ecx,[(-(offset HEAPHEAD)-4)+ebx]
        shr     ecx,3
        cmp     ecx,FS:[heapheadentries]
        jb      @@notlastrm
@@findlastnot0:
        dec     ecx
        jz      @@setlastnot0
        sub     ebx,8
        cmp     dword ptr DS:[ebx],0
        jz      @@findlastnot0
        add     ebx,8
@@setlastnot0:
        inc     ecx
        mov     FS:[heapheadentries],ecx
@@notlastrm:
        cmp     ecx,FS:[heapheade0]
        jae     @@return
        mov     FS:[heapheade0],ecx
        jmp     short @@return
@@foundlarger:
        mov     esi,ebx
        mov     edx,DS:[ebx]
@@findminlarger:
        add     ebx,8
        dec     ecx
        jz      @@findminlarger_e
        cmp     DS:[ebx],eax
        jb      @@findminlarger
        je      @@foundexact
        cmp     DS:[ebx],edx
        jae     @@findminlarger
        jmp     short @@foundlarger
@@findminlarger_e:
        sub     edx,eax
        mov     DS:[esi],edx
        sub     esi,4
        mov     ebx,DS:[esi]
        add     DS:[esi],eax
        mov     eax,ebx
@@return:
        pop     ds esi edx ecx ebx
        ret
alloc endp

;*********************************************************************** free
;*** free memory (anywhere in heap)
;*** in :  eax=address, edx=size
;*** ass:  all the structures related to heap management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
free proc far
        push    ebx ecx edx esi edi ds
        OPTZER0 ebx
        test    dl,MMS_ALIGN_C0-1
        jz      @@size_aligned
        or      dl,MMS_ALIGN_C0-1
        inc     edx
@@size_aligned:
        mov     ecx,[heapheadentries]
        mov     ds,ebx
        add     edx,eax
        mov     ebx,offset HEAPHEAD
;-----------------------------
@@findh01:
        mov     esi,DS:[ebx]
        cmp     esi,edx
        je      @@foundh0a      ;above our block
        add     esi,DS:[ebx+4]
        cmp     esi,eax
        je      @@foundh0b      ;below our block
        add     ebx,8
        OPTLOOPD @@findh01
@@h0abnotfound:
        mov     ebx,FS:[heapheade0]
        sub     edx,eax
        shl     ebx,3
        mov     ecx,FS:[heapheadentries]
        add     ebx,(offset HEAPHEAD+4)
        sub     ecx,FS:[heapheade0]
;        lea     ebx,[(offset HEAPHEAD+4)+ebx*8]
        jna     @@unusednotfound
@@find1stunused:
        cmp     dword ptr DS:[ebx],0
        je      @@found1stunused
        add     ebx,8
        OPTLOOPD @@find1stunused
@@unusednotfound:
        mov     ecx,FS:[heapheadentries]
        cmp     ecx,HEAPMAXB
        jae     @@err
        inc     ecx
        mov     DS:[ebx-4],eax
        mov     FS:[heapheade0],ecx
        mov     DS:[ebx],edx
        mov     FS:[heapheadentries],ecx
;        jmp     short @@return
        pop     ds edi esi edx ecx ebx
        ret
;-----------------------------
@@foundh0a:
        mov     edi,ebx
        sub     edx,eax
@@findh1b:
        add     ebx,8
        dec     ecx
        jz      @@h0a_only
        mov     esi,DS:[ebx+4]
        add     esi,DS:[ebx]
        cmp     esi,eax
        jne     @@findh1b
@@foundh0a1b:
        mov     ecx,DS:[ebx]
        mov     esi,DS:[ebx+4]
        mov     dword ptr DS:[ebx+4],0
        add     esi,edx
        mov     DS:[edi],ecx
        add     DS:[edi+4],esi
;       mov     dword ptr DS:[ebx],0
        jmp     short @@bothfound
;-----------------------------
@@foundh0b:
        mov     edi,ebx
@@findh1a:
        add     ebx,8
        dec     ecx
        jz      @@h0b_only
        cmp     edx,DS:[ebx]
        jne     @@findh1a
@@foundh0b1a:
        mov     esi,DS:[ebx+4]
        sub     edx,eax
        mov     dword ptr DS:[ebx+4],0
        add     esi,edx
        add     DS:[edi+4],esi
;       mov     dword ptr DS:[ebx],0
        jmp     short @@bothfound
;-----------------------------
@@found1stunused:
        mov     esi,FS:[heapheadentries]
        mov     DS:[ebx-4],eax
        sub     esi,ecx
        mov     DS:[ebx],edx
        mov     FS:[heapheade0],esi
;        jmp     short @@return
;-----------------------------
@@return:
        pop     ds edi esi edx ecx ebx
        ret
;-----------------------------
@@h0a_only:
        mov     DS:[edi],eax
        add     DS:[edi+4],edx
        jmp     short @@return
;-----------------------------
@@h0b_only:
        sub     edx,eax
        add     DS:[edi+4],edx
        jmp     short @@return
;-----------------------------
@@bothfound:
        mov     ecx,ebx
        sub     ecx,offset HEAPHEAD
        shr     ecx,3
        inc     ecx
        cmp     ecx,FS:[heapheadentries]
        jb      @@notlastrm
        mov     ecx,FS:[heapheadentries]
        add     ebx,4
        dec     ecx
@@findlastnot0:
        dec     ecx
        jz      @@setlastnot0
        sub     ebx,8
        cmp     dword ptr DS:[ebx],0
        jz      @@findlastnot0
        add     ebx,8
@@setlastnot0:
        inc     ecx
        mov     FS:[heapheadentries],ecx
@@notlastrm:
        cmp     ecx,FS:[heapheade0]
        jae     @@return
        mov     FS:[heapheade0],ecx
        jmp     short @@return
;-----------------------------
@@err:  OPTZER0 eax
        test    FS:[mms_flags],MMS_F_EXITONERR
        jz      @@return
        mov     bx,data16
        mov     ds,bx
        REG_ERROR merrmmsf0
        GEN_ERROR err_exit+err_err
;-----------------------------
free endp

;****************************************************************************
;****************************************************************** initheap2
;*** alloc heap2 space in heap + alloc&init heap2 control block in heap2list
;*** in :  ax=log2(entry_s), bx=entry_n
;*** ass:  all the structures related to heap & heap2 management
;*** out:  ea -x=address (Ok) or 0 (Err), bx=heap2handle
;*** mod:  -
initheap2 proc far

initheap2 endp

;******************************************************************** rmheap2
;*** dealloc heap2 space in heap + dealloc heap2 control block in heap2list
;*** in :  bx=heap2handle
;*** ass:  all the structures related to heap & heap2 management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
rmheap2 proc far

rmheap2 endp

;********************************************************************* alloc2
;*** alloc memory in heap2
;*** in :  bx=heap2handle
;*** ass:  all the structures related to heap2 management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
alloc2 proc far

alloc2 endp

;********************************************************************** free2
;*** free heap memory in heap2
;*** in :  bx=heap2handle, eax=address
;*** ass:  all the structures related to heap2 management
;*** out:  eax=address (Ok) or 0 (Err)
;*** mod:  -
free2 proc far

free2 endp