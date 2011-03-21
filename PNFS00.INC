DFS_FT_F_OPEN           EQU 1
DFS_FT_F_ALLOC          EQU 2
DFS_FT_F_READ           EQU 4

DFS_FT_FLAGS            EQU 0
DFS_FT_NFILES           EQU 2
DFS_FT_DDKBSBEG         EQU 4
DFS_FT_SEEKPOS          EQU 8
DFS_FT_DDSIZE           EQU 12
DFS_FT_FATSIZE          EQU 16
DFS_FT_FATADDR          EQU 20
DFS_FT_DOSFILEHANDLE    EQU 24
DFS_FT_DFNAMEOFF        EQU 26
DFS_FT_FATBLOCKS        EQU 28
DFS_FT_RESERVED0        EQU 30
DFS_FATTAB_ENTRYSIZE    EQU 32
DFS_FATTAB_ENTRYSHIFT   EQU 5


DF_BLOCKSIZE            EQU 1000h
DF_BLOCKSIZE_SHIFT      EQU 12
DF_ENTRYSIZE            EQU 10h
DF_ENTRYSIZE_SHIFT      EQU 4

DF_VERSION              EQU 10h
DF_MAXFATSIZE           EQU 10000h
DF_FATENTRYSIZE         EQU 10h
DF_MAXBLOCKS            EQU DF_MAXFATSIZE/DF_BLOCKSIZE
DF_MAXFILES             EQU DF_MAXFATSIZE/DF_FATENTRYSIZE


 DFF_RESERVED0           EQU 0
;DFF_DDKBSBEG            EQU 0
 DFF_DATE0               EQU 4
 DFF_TIME0               EQU 6
;DFF_SEEKPOS             EQU 4
 DFF_DATE                EQU 8
 DFF_TIME                EQU 10
 DFF_VERSION             EQU 12
 DFF_BLOCKSIZE_MSB       EQU 13
 DFF_NFILES              EQU 14
 DFF_FOFFSET             EQU DF_BLOCKSIZE-16
;DFF_DDSIZE              EQU DF_BLOCKSIZE-16
 DFF_FATADDCRC0          EQU DF_BLOCKSIZE-12
;DFF_HANDLE              EQU DF_BLOCKSIZE-12
;DFF_IDOPEN              EQU DF_BLOCKSIZE-10
 DFF_FATADDCRC           EQU DF_BLOCKSIZE-8
 DFF_FATXORCRC           EQU DF_BLOCKSIZE-4

DFFF_OFFSET             EQU 0
DFFF_OFFSET_MASK_OFF    EQU 0FFFFFF00h
DFFF_OFFSET_MASK_CRC    EQU 0000000FFh
DFFF_SIZE               EQU 4
DFFF_SIZEC              EQU 8
DFFF_FILEIDD            EQU 12
DFFF_FILEIDW0           EQU 12
DFFF_FILEIDW1           EQU 14


DF_FID_CODE             EQU 0
DF_FID_DATA32E          EQU 1
DF_FID_GRAFX            EQU 2
DF_FID_SOUND            EQU 3


DF_ER0_CANTOPEN         EQU 1
DF_ER0_CORRUPT          EQU 2
DF_ER0_UNSVER           EQU 4
DF_ER0_CANTALLOCMEM     EQU 8
DF_ER0_NOFREEHANDLE     EQU 10h

DF_ER1_NOTOPEN          EQU 1
DF_ER1_STILLOPEN        EQU 2
DF_ER1_CANTFREEFAT      EQU 4
DF_ER1_CANTFREETAB      EQU 8
DF_ER1_NOONEOPEN        EQU 10h
DF_ER1_INVALIDHANDLE    EQU 20h

DF_ER2_INVALIDFAT       EQU 1
DF_ER2_CANTALLOCMEM     EQU 2
DF_ER2_FILENOTFOUND     EQU 4
DF_ER2_READFAULT        EQU 8


;* datafile format:
;    - block of (df_fatsize)b - fat
;    - blocks of (df_blocksize)kb - data area
;
;* fat format (16b blocks):
;    - first      : dd 0                    ;; dd ddkbsbeg
;                   dw date0, time0         ;; dd seekpos
;                   dw date(1980+rrrrrrrmmmmddddd), time(hhhhhmmmmmmsssss*2)
;                   db version, db blocksize_msb, dw nfiles
;    - allocated  : dd offset,xc^ac (off.off.off.axc)
;                   dd lenght
;                   dd lenghtc
;                   dw fileid0,fileid1
;    - free       : first xored by allocated0,
;                   allocated0 xored by allocated1, ..
;    - last       : dd foffset              ;; dd ddsize
;                   dd fataddcrc0           ;; dw handle, idopen=idopen_const
;                   dd fataddcrc            ;; dd reserved
;                   dd fatxorcrc            ;; dd reserved

;******************************************************************* dfs_init
;init datafile system (alloc memory for DFS_FATTAB)
; in :  data16:: dw DFS_FATTAB_ENTRIES
; ass:  data16:: BUFFER = (blocksize)b buffer,dw DOS_ERRN,
; out:  eax=address (Ok) or 0 (Err)
;       data16::dd DFS_FATTAB_SIZE,dd DFS_FATTAB_ADDR,dw DFS_FATTAB_OPENS
; mod:  -
dfs_init proc far
                push    cx ebx edx
                mov     byte ptr fs:[DFS_FATTAB_OPENS],0
                ;mov     ax,DFS_FATTAB_ENTRYSIZE
                ;mul     ax,fs:[DFS_FATTAB_ENTRIES]
                ;shl     edx,16
                ;mov     dx,ax
                ;mov     fs:[DFS_FATTAB_SIZE],edx
                ;mov     eax,edx
                OPTZER0 eax
                mov     ax,fs:[DFS_FATTAB_ENTRIES]
                shl     eax,DFS_FATTAB_ENTRYSHIFT
                mov     fs:[DFS_FATTAB_SIZE],eax

                call    allocp
                mov     fs:[DFS_FATTAB_ADDR],eax
                cmp     eax,0
                jz      @@return
                mov     ebx,eax
                mov     cx,fs:[DFS_FATTAB_ENTRIES]
@@zer0all_idopen:
                mov     word ptr gs:[ebx],0
                add     ebx,DFS_FATTAB_ENTRYSIZE
                OPTLOOP @@zer0all_idopen
@@return:       pop     edx ebx cx
                ret
dfs_init endp

;******************************************************************* dfs_open
;open datafile, check it and get info
; in :  data16:dx - asciiz fspec, eax - ddkbsbeg
; ass:  data16:: db BUFFER[DF_BLOCKSIZE], dw DFS_FATTAB_ACTHANDLE
;                dw DOS_ERRN, dw DFS_FATTAB_ENTRIES, dd DFS_FATTAB_ADDR
;                db DFS_ERR_OPEN, dw DFS_FATTAB_OPENS, dd DFS_FATTAB_SIZE
; out:  al= 0 (no_err) or DF_ER0_* ; dx=fat_handle
; mod:  -
dfs_open proc far
                pushad
                push    ds es
DEBUG_DFS00
                mov     dword ptr fs:[BUFFER],eax
                mov     bx,data16
                mov     ax,offset BUFFER
                mov     word ptr fs:[BUFFER+4],dx
                shr     ax,4
                add     ax,data16
                mov     ds,bx
                mov     es,ax
                mov     DFS_ERR_OPEN,0

                mov     ax,[DFS_FATTAB_ENTRIES]
                cmp     ax,[DFS_FATTAB_OPENS]
                jna     @@err_nofreehandle

                mov     esi,[DFS_FATTAB_ADDR]
                mov     cx,[DFS_FATTAB_ENTRIES]
@@find_not_open:
                test    BYTE ptr gs:[esi],DFS_FT_F_OPEN
                jz      @@found_not_open
                add     esi,DFS_FATTAB_ENTRYSIZE
                OPTLOOP @@find_not_open
@@err_nofreehandle:
                mov     byte ptr [DFS_ERR_OPEN],DF_ER0_NOFREEHANDLE
                REG_ERROR merrdfs5
                pop     es ds
                popad
                mov     al,DF_ER0_NOFREEHANDLE
                ret
@@err_cantopen:
                mov     fs:[DOS_ERRN],ax
                mov     byte ptr [DFS_ERR_OPEN],DF_ER0_CANTOPEN
                REG_ERROR merrdfs0
                pop     es ds
                popad
                mov     al,DF_ER0_CANTOPEN
                ret
@@err_unsver:   mov     byte ptr [DFS_ERR_OPEN],DF_ER0_UNSVER
                REG_ERROR merrdfs2
                pop     es ds
                popad
                mov     al,DF_ER0_UNSVER
                ret
@@found_not_open:
                mov     ax,[DFS_FATTAB_ENTRIES]
                sub     ax,cx
                mov     DFS_FATTAB_ACTHANDLE,ax
                ;mov     bx,DFS_FATTAB_ENTRYSIZE
                ;mul     ax,bx
                ;shl     edx,16
                ;mov     esi,edx
                ;mov     si,ax
                ;add     esi,fs:[DFS_FATTAB_ADDR]

                mov     dx,word ptr [BUFFER+4]
                mov     ax,DOS_FN_FH_OPEN*256+DFS_DOS_OPEN_CONST0
                mov     gs:[esi+DFS_FT_DFNAMEOFF],dx
                int     21h
                jc      @@err_cantopen

                mov     gs:[esi+DFS_FT_DOSFILEHANDLE],ax
                mov     bx,ax
                mov     ecx,dword ptr [BUFFER]
                inc     DFS_FATTAB_OPENS
                mov     BYTE ptr gs:[esi],DFS_FT_F_OPEN
                mov     gs:[esi+DFS_FT_DDKBSBEG],ecx

                or      ecx,ecx
                jz      @@noseekbefore
                mov     dx,cx
                mov     ax,DOS_FN_FH_LSEEK*256+DOS_FN_FH_LSEEK_SET
                shr     ecx,16
                int     21h
                jc      @@err_corrupt_dos
                ;mov     cx,dx
                ;shl     ecx,16
                ;mov     cx,ax
@@noseekbefore:
                mov     ah,DOS_FN_FH_READ
                mov     dx,offset BUFFER
                mov     cx,DF_BLOCKSIZE
                int     21h
                jc      @@err_corrupt_dos
                cmp     ax,DF_BLOCKSIZE ;
                jne     @@err_corrupt   ;
@@check_rng:
                cmp     byte ptr es:[DFF_VERSION],DF_VERSION
                jb      @@err_corrupt
                ja      @@err_unsver

                cmp     byte ptr es:[DFF_BLOCKSIZE_MSB],DF_BLOCKSIZE/256
                jne     @@err_corrupt

                mov     ax,es:[DFF_NFILES]
                cmp     ax,0
                jz      @@err_corrupt
                cmp     ax,(DF_MAXFILES-2)
                ja      @@err_corrupt
                add     ax,2
                mov     gs:[esi+DFS_FT_NFILES],ax

                cmp     byte ptr es:[DFF_RESERVED0],0
                jne     @@err_corrupt

                mov     ebx,ds:[DFF_DATE0]
                cmp     ebx,ds:[DFF_DATE]
                jne     @@err_corrupt
                mov     ax,bx
                and     ax,DATE_Y_MASK
                cmp     ax,DATE_Y_MAX
                ja      @@err_corrupt
                mov     ax,bx
                and     ax,DATE_M_MASK
                cmp     ax,DATE_M_MAX
                ja      @@err_corrupt
                mov     ax,bx
                and     ax,DATE_D_MASK
                cmp     ax,DATE_D_MAX
                ja      @@err_corrupt

                shl     ebx,16
                mov     ax,bx
                and     ax,TIME_H_MASK
                cmp     ax,TIME_H_MAX
                ja      @@err_corrupt
                mov     ax,bx
                and     ax,TIME_M_MASK
                cmp     ax,TIME_M_MAX
                ja      @@err_corrupt
                mov     ax,bx
                and     ax,TIME_S_MASK
                cmp     ax,TIME_S_MAX
                ;ja      @@err_corrupt
                jbe     @@alloc_fat
                jmp     short @@err_corrupt
@@err_corrupt_dos:
                mov     fs:[DOS_ERRN],ax
@@err_corrupt:
                mov     byte ptr [DFS_ERR_OPEN],DF_ER0_CORRUPT
                REG_ERROR merrdfs1
                pop     es ds
                popad
                mov     al,DF_ER0_CORRUPT
                ret
@@err_cantalloc:
                mov     byte ptr [DFS_ERR_OPEN],DF_ER0_CANTALLOCMEM
                pop     es ds
                popad
                mov     al,DF_ER0_CANTALLOCMEM
                ret
@@alloc_fat:
                OPTZER0 edx
                mov     dx,es:[DFF_NFILES]
                ;inc     edx
                shl     edx,DF_ENTRYSIZE_SHIFT
                test    edx,DF_BLOCKSIZE-1
                jz      @@rounded_fatsize
                or      edx,DF_BLOCKSIZE-1
                inc     edx
@@rounded_fatsize:
                mov     gs:[esi+DFS_FT_FATSIZE],edx
                mov     eax,edx
                shr     edx,DF_BLOCKSIZE_SHIFT
                mov     gs:[esi+DFS_FT_FATBLOCKS],dx
                call    allocp
                or      eax,eax
                jz      @@err_cantalloc
                or      BYTE ptr gs:[esi],DFS_FT_F_ALLOC

                mov     ebx,eax
                mov     gs:[esi+DFS_FT_FATADDR],eax

                mov     di,offset buffer
                mov     cx,DF_BLOCKSIZE/4
@@move_up_f0:   mov     eax,[di]
                mov     gs:[ebx],eax
                add     di,4
                add     ebx,4
                OPTLOOP @@move_up_f0

DEBUG_DFS00

                mov     di,gs:[esi+DFS_FT_FATBLOCKS]
                mov     ebp,ebx
@@read_all_fatblocks:
                dec     di
                jz      @@rafb_e
                mov     ah,DOS_FN_FH_READ
                mov     bx,[DFS_FATTAB_ACTHANDLE]
                           ;gs:[esi+DFS_FT_DOSFILEHANDLE]
                mov     dx,offset BUFFER
                mov     cx,DF_BLOCKSIZE
                int     21h
                jc      @@err_corrupt_dos
                cmp     ax,DF_BLOCKSIZE ;
                jne     @@err_corrupt   ;

                mov     bx,offset buffer
                mov     cx,DF_BLOCKSIZE/4
@@move_up_fn:   mov     eax,[bx]
                mov     gs:[ebp],eax
                add     bx,4
                add     ebp,4
                OPTLOOP @@move_up_fn

                jmp     short @@read_all_fatblocks
@@rafb_e:

@@check_xor:

@@check_crc:

                mov     cx,gs:[esi+DFS_FT_NFILES]
                mov     ebx,gs:[esi+DFS_FT_FATADDR]
                mov     eax,gs:[esi+DFS_FT_DDKBSBEG]
                dec     cx
@@translate_ddsize:
                add     gs:[ebx],eax
                add     ebx,DF_FATENTRYSIZE
                OPTLOOP @@translate_ddsize



                mov     ax,DOS_FN_FH_LSEEK*256+DOS_FN_FH_LSEEK_END
                mov     bx,[DFS_FATTAB_ACTHANDLE]
                           ;gs:[esi+DFS_FT_DOSFILEHANDLE]
                xor     cx,cx
                xor     dx,dx
                int     21h
                jc      @@err_corrupt_dos
                shl     edx,16
                mov     dx,ax
                mov     gs:[esi+DFS_FT_DDSIZE],edx
                mov     gs:[esi+DFS_FT_SEEKPOS],edx
                or      BYTE ptr gs:[esi],DFS_FT_F_READ
@@ok:
DEBUG_DFS00
                pop     es ds
                popad
                mov     dx,fs:[DFS_FATTAB_ACTHANDLE]
                xor     al,al
@@return:       ret
dfs_open endp

;******************************************************************* dfs_read
;reads a file from an opened datafile
; in :  eax = displacement ; edx = file_id ; di = fat_handle
;       ebx = 0 (alloc) or address ; ecx = 0 (whole) or max_size
; ass:  data16:: db BUFFER[DFS_BUFFER_SIZE], dw DFS_FATTAB_ACTHANDLE
;                dd DFS_READBUF_ADDR/SIZE
;                dw DOS_ERRN, dw DFS_FATTAB_ENTRIES, dd DFS_FATTAB_ADDR
;                db DFS_ERR_READ, dw DFS_FATTAB_OPENS, dd DFS_FATTAB_SIZE
; out:  al= 0 (no_err) or DF_ER2_* ; ebx = adress (ok) ; ecx = size
; mod:  eax,ebx,ecx
dfs_read proc far
                pushad
                push    ds
                cmp     di,fs:[DFS_FATTAB_ENTRIES]
                jae     @@err_filenotfound
                mov     si,data16
                and     edi,0ffffh
                shl     edi,DFS_FATTAB_ENTRYSHIFT
                mov     ds,si
                mov     byte ptr fs:[DFS_ERR_READ],0
                add     edi,fs:[DFS_FATTAB_ADDR]
                mov     DFS_READBUF_ADDR,ebx
                mov     DFS_READBUF_SIZE,ecx


                mov     bp,gs:[edi]
                and     bp,DFS_FT_F_READ+DFS_FT_F_ALLOC+DFS_FT_F_OPEN
                cmp     bp,DFS_FT_F_READ+DFS_FT_F_ALLOC+DFS_FT_F_OPEN
                jne     @@err_invalidfat
                mov     ebp,edi

                mov     cx,gs:[ebp+DFS_FT_NFILES]
                mov     esi,gs:[ebp+DFS_FT_FATADDR]
                add     esi,DFFF_FILEIDD+DF_FATENTRYSIZE
                sub     cx,2

@@find_fileid:
                cmp     gs:[esi],edx
                je      @@found_fileid
                add     esi,DF_FATENTRYSIZE
                OPTLOOP @@find_fileid

@@err_filenotfound:
                mov     byte ptr fs:[DFS_ERR_READ],DF_ER2_FILENOTFOUND
                REG_ERROR merrdfs3
                pop     ds
                popad
                mov     al,DF_ER2_FILENOTFOUND
                ret
@@err_invalidfat:
                mov     byte ptr fs:[DFS_ERR_READ],DF_ER2_INVALIDFAT
                pop     ds
                popad
                mov     al,DF_ER2_INVALIDFAT
                ret
@@err_cantallocmem:
                mov     byte ptr fs:[DFS_ERR_READ],DF_ER2_CANTALLOCMEM
                pop     ds
                popad
                mov     al,DF_ER2_CANTALLOCMEM
                ret
@@err_readfault:
                mov     fs:[DOS_ERRN],ax
                mov     byte ptr fs:[DFS_ERR_READ],DF_ER2_READFAULT
                REG_ERROR merrdfs4
                pop     ds
                popad
                mov     al,DF_ER2_READFAULT
                ret

@@found_fileid:
                sub     esi,DFFF_FILEIDD
                mov     edi,gs:[esi]
                and     edi,DFFF_OFFSET_MASK_OFF
                add     edi,eax
                mov     ax,DOS_FN_FH_LSEEK*256+DOS_FN_FH_LSEEK_SET
                mov     ecx,edi
                mov     edx,edi
                mov     bx,gs:[ebp+DFS_FT_DOSFILEHANDLE]
                shr     ecx,16
                int     21h
                jc      @@err_readfault


                mov     ecx,[DFS_READBUF_SIZE]
                or      ecx,ecx
                jz      @@loadsize
                cmp     ecx,gs:[esi+DFFF_SIZE]
                jbe     @@size_ok
@@loadsize:     mov     ecx,gs:[esi+DFFF_SIZE]
@@size_ok:
                test    ecx,DF_BLOCKSIZE-1
                jz      @@size_ok2
                or      ecx,DF_BLOCKSIZE-1
                inc     ecx
@@size_ok2:     mov     DFS_READBUF_SIZE,ecx
                shr     ecx,DF_BLOCKSIZE_SHIFT


                mov     ebx,[DFS_READBUF_ADDR]
                or      ebx,ebx
                jnz     @@adress_ok
                mov     eax,ecx
                call    alloc
                or      eax,eax
                jz      @@err_cantallocmem
                mov     ebx,eax
@@adress_ok:    mov     DFS_READBUF_ADDR,ebx

                mov     esi,ecx
                mov     edi,ebx
@@read_all_fileblocks:
                mov     ah,DOS_FN_FH_READ
                mov     bx,gs:[ebp+DFS_FT_DOSFILEHANDLE]
                mov     dx,offset BUFFER
                mov     cx,DF_BLOCKSIZE
                int     21h
                jc      @@err_readfault
                cmp     ax,DF_BLOCKSIZE ;
                jne     @@err_readfault ;

                mov     bx,offset buffer
                mov     cx,DF_BLOCKSIZE/4
@@move_up_fln:  mov     eax,[bx]
                mov     gs:[edi],eax
                add     bx,4
                add     edi,4
                OPTLOOP @@move_up_fln

                dec     esi
                jnz     @@read_all_fileblocks
@@rafb_e:


@@ok:           pop     ds
                popad
                mov     ebx,fs:[DFS_READBUF_ADDR]
                mov     ecx,fs:[DFS_READBUF_SIZE]
                xor     al,al
@@return:       ret
dfs_read endp

;****************************************************************** dfs_close
;closes datafile(s) currently open
;dealloc mem if MMS_F_FREEONCLOSE&[exitflags]
; in :  ax=fattab_entry or DFS_HANDLE_ALLOPEN
; out:  al= 0 (no_err) or DF_ER1_*
; ass:  data16:: dw DOS_ERRN, dw DFS_FATTAB_ENTRIES, dd DFS_FATTAB_ADDR
;                db DFS_ERR_CLOSE, dw DFS_FATTAB_OPENS, dd DFS_FATTAB_SIZE
; mod:  -
dfs_close proc  far
                push    esi edx bx cx ds
                mov     dx,data16
                mov     ds,dx
DEBUG_DFS00
                mov     byte ptr DS:[DFS_ERR_CLOSE],0

                cmp     ax,DFS_HANDLE_ALLOPEN
                je      @@close_all

                cmp     ax,DS:[DFS_FATTAB_ENTRIES]
                jne     @@c0err_invalidhandle

                ;mov     bx,DFS_FATTAB_ENTRYSIZE
                ;mul     ax,bx
                ;shl     edx,16
                ;mov     esi,edx
                ;mov     si,ax
                ;add     esi,DS:[DFS_FATTAB_ADDR]
                and     eax,0ffffh
                shl     eax,DFS_FATTAB_ENTRYSHIFT
                add     eax,DS:[DFS_FATTAB_ADDR]
                mov     esi,eax

                test    BYTE ptr gs:[esi],DFS_FT_F_OPEN
                jne     @@c0err_notopen

                ;inc     byte ptr gs:[esi+1]

                test    byte ptr DS:[mms_flags],MMS_F_FREEFATONCLOSE
                jz      @@c0dontfreefatmem
                mov     edx,DS:[esi+DFS_FT_FATSIZE]
                mov     eax,DS:[esi+DFS_FT_FATADDR]
                call    free
                cmp     eax,0
                jnz     @@c0dontfreefatmem
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_CANTFREEFAT
@@c0dontfreefatmem:
                mov     bx,gs:[esi+DFS_FT_DOSFILEHANDLE]
                mov     ah,DOS_FN_FH_CLOSE
                int     21h
                jc      @@c0err_open
                mov     BYTE ptr gs:[esi],0
                dec     word ptr DS:[DFS_FATTAB_OPENS]
@@return:       mov     al,DS:[DFS_ERR_CLOSE]
DEBUG_DFS00
                pop     ds cx bx edx esi
                ret
@@c0err_invalidhandle:
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_INVALIDHANDLE
                jmp     short @@return
@@c0err_notopen:or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_NOTOPEN
                jmp     short @@return
@@c0err_open:   mov     DS:[DOS_ERRN],ax
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_STILLOPEN
                jmp     short @@return

@@err_nooneopen:or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_NOONEOPEN
                jmp     short @@return


@@close_all:    cmp     word ptr DS:[DFS_FATTAB_OPENS],0
                jz      @@err_nooneopen
                mov     esi,DS:[DFS_FATTAB_ADDR]
                mov     cx,DS:[DFS_FATTAB_ENTRIES]
@@testall_idopen:
                test    BYTE ptr gs:[esi],DFS_FT_F_OPEN
                jz      @@next_fat

                ;inc     byte ptr gs:[esi+1]

                test    byte ptr DS:[mms_flags],MMS_F_FREEFATONCLOSE
                jz      @@c1dontfreefatmem
                test    BYTE ptr gs:[esi],DFS_FT_F_ALLOC
                jz      @@c1dontfreefatmem
                mov     edx,gs:[esi+DFS_FT_FATSIZE]
                mov     eax,gs:[esi+DFS_FT_FATADDR]
                call    free
                cmp     eax,0
                jnz     @@c1dontfreefatmem
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_CANTFREEFAT
@@c1dontfreefatmem:
                mov     bx,gs:[esi+DFS_FT_DOSFILEHANDLE]
                mov     ah,DOS_FN_FH_CLOSE
                int     21h
                jc      @@c1err_open
                mov     BYTE ptr gs:[esi],0
                dec     word ptr DS:[DFS_FATTAB_OPENS]
@@c1err_open:
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_STILLOPEN
@@next_fat:
                add     esi,DFS_FATTAB_ENTRYSIZE
                OPTLOOP @@testall_idopen
@@all_fats_tested:

                test    byte ptr DS:[mms_flags],MMS_F_FREETABONCLOSE
                jz      @@c11dontfreetabmem
                cmp     word ptr DS:[DFS_FATTAB_OPENS],0
                je      @c11freetabmem
                test    byte ptr DS:[mms_flags],MMS_F_FREETABIFNFAT
                jz      @@c11dontfreetabmem
@c11freetabmem:
                mov     edx,DS:[DFS_FATTAB_SIZE]
                mov     eax,DS:[DFS_FATTAB_ADDR]
                call    free
                cmp     eax,0
                jnz     @@c11dontfreetabmem
                or      byte ptr DS:[DFS_ERR_CLOSE],DF_ER1_CANTFREETAB
@@c11dontfreetabmem:

@@return2:      mov     al,DS:[DFS_ERR_CLOSE]
DEBUG_DFS00
                pop     ds cx bx edx esi
                ret
dfs_close endp