OPEN_CONST0     EQU 20h

DF_FT_CODE              EQU 0
DF_FT_DATA32E           EQU 1
DF_FT_GRAFX             EQU 2
DF_FT_SOUND             EQU 3

DF_ER0_OPEN             EQU 1
DF_ER0_CORRUPT          EQU 2
DF_ER0_UNSVER           EQU 3
DF_ER1_NOTOPEN          EQU 1
DF_ER1_OPEN             EQU 2

DF_IDOPEN_CONST         EQU 4321h

DF_FATSIZE              EQU 4096
DF_BLOCKSIZE            EQU 4096
DF_VERSION              EQU 10h

DFF_DDKBSBEG            EQU 0
DFF_SEEKPOS             EQU 4
DFF_DATE0               EQU 4
DFF_DATE                EQU 8
DFF_TIME                EQU 10
DFF_VERSION             EQU 12
DFF_BLOCKSIZE_MSB       EQU 13
DFF_NFILES              EQU 14
DFF_DDSIZE              EQU DF_FATSIZE-16
DFF_FATADDCRC0          EQU DF_FATSIZE-12
DFF_HANDLE              EQU DF_FATSIZE-12
DFF_IDOPEN              EQU DF_FATSIZE-10
DFF_FATADDCRC           EQU DF_FATSIZE-8
DFF_FATXORCRC           EQU DF_FATSIZE-4

DFFF_OFFSET             EQU 0
DFFF_LENGHT             EQU 4
DFFF_LENGHTC            EQU 8
DFFF_FILEID             EQU 12
DFFF_FILEID0            EQU 12
DFFF_FILEID1            EQU 14

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

;**************************************************************** nidf_getnfo
;gets info abou ni-datafile
; in :  ds:dx - asciiz fspec, ds:si - (fatsize)b fat, ecx - ddkbsbeg
; ass:  data16:BUFFER - (fatsize)b buffer, gs:doserrn
; out:  al: 0-no_err, 1-open, 2-corrupt
; mod:  es:0=data16:BUFFER,fs=ds,ds:0=ds:si

nidf_getnfo proc far
                pusha
                mov     ax,offset BUFFER
                shr     ax,4
                add     ax,data16
                mov     es,ax

                mov     ax,DOS_FN_FH_OPEN*256+OPEN_CONST0
                int     21h
                mov     gs:[doserrn],al
                jnc     @@open_ok
                popa
                mov     al,DF_ER0_OPEN
                mov     word ptr ds:[DFF_IDOPEN],0
                ret
@@open_ok:      mov     bx,ax

                shr     si,4
                mov     ax,ds
                mov     fs,ax
                add     ax,si
                shl     si,4
                mov     ds,ax

                mov     es:[DFF_DDKBSBEG],ecx
                mov     ds:[DFF_HANDLE],ax
                or      ecx,ecx
                jz      @@noseekbefore
                mov     dx,cx
                mov     ax,DOS_FN_FH_LSEEK*256
                shr     ecx,16
                int     21h
                mov     gs:[doserrn],al
                jc      @@err_corrupt
                ;mov     cx,dx
                ;shr     ecx,16
                ;mov     cx,ax
@@noseekbefore: mov     ah,DOS_FN_FH_READ
                xor     dx,dx
                mov     cx,DF_FATSIZE
                int     21h
                mov     gs:[doserrn],al
                mov     eax,ds:[DFF_FATADDCRC0]
                mov     ds:[DFF_HANDLE],bx
                jc      @@err_corrupt
                cmp     eax,ds:[DFF_FATADDCRC]
                jne     @@err_corrupt
                cmp     dword ptr [si],0
                jnz     @@err_corrupt
                mov     ebx,ds:[DFF_DATE0]
                mov     eax,es:[DFF_DDKBSBEG]
                cmp     ebx,ds:[DFF_DATE]
                mov     ds:[DFF_DDKBSBEG],eax
                jne     @@err_corrupt
                mov     ax,bx
                and     ax,DATE_Y_MASK
                cmp     ax,DATE_Y_MAX
                jae     @@err_corrupt
                mov     ax,bx
                and     ax,DATE_M_MASK
                cmp     ax,DATE_M_MAX
                jae     @@err_corrupt
                mov     ax,bx
                and     ax,DATE_D_MASK
                cmp     ax,DATE_D_MAX
                jae     @@err_corrupt
                shl     ebx,16
                mov     ax,bx
                and     ax,TIME_H_MASK
                cmp     ax,TIME_H_MAX
                jae     @@err_corrupt
                mov     ax,bx
                and     ax,TIME_M_MASK
                cmp     ax,TIME_M_MAX
                jae     @@err_corrupt
                mov     ax,bx
                and     ax,TIME_S_MASK
                cmp     ax,TIME_S_MAX
                jae     @@err_corrupt
                cmp     byte ptr ds:[DFF_VERSION],DF_VERSION
                jne     @@err_corrupt
                cmp     byte ptr ds:[DFF_BLOCKSIZE_MSB],DF_BLOCKSIZE/256
                jne     @@err_corrupt
                mov     bx,ds:[DFF_NFILES]
                cmp     bx,(DF_FATSIZE/16)-1
                jae     @@err_corrupt
                mov     cx,bx
                cmp     bx,2
                jb      @@err_corrupt
                shl     bx,4
                mov     eax,ds:[DFF_DDSIZE]
                cmp     eax,ds:[bx]
                jne     @@err_corrupt
                add     si,8
                xor     eax,eax
                xor     di,di
                stosd
                lodsd
                stosd
                stosd
                lodsd
                stosd
@@movalloc2buf: lodsd
                test    ax,DF_BLOCKSIZE-1-255
                jnz     @@err_corrupt
                stosd
                lodsd
                mov     edx,eax
                stosd
                lodsd
                cmp     eax,edx
                je      @@not_corrupt   ;jbe if compression supported
@@err_corrupt:  mov     ax,fs
                mov     ds,ax
                popa
                mov     al,DF_ER0_CORRUPT
                mov     word ptr ds:[DFF_IDOPEN],DF_IDOPEN_CONST
                ret
@@not_corrupt:
                stosd
                movsd
                loop    @@movalloc2buf
                sub     si,bx
                mov     cx,(DF_FATSIZE)/16-2
                mov     bx,si
                sub     cx,ds:[DFF_NFILES]
                sub     si,16
                shl     cx,2
                push    si cx
@@constrfreef:  mov     edx,[bx]
                lodsd
                xor     eax,edx
                add     bx,4
                stosd
                loop    @@constrfreef
                pop     cx si
                add     si,di
                std
                repe cmpsd
                cld
                or      cx,cx
                jnz     @@err_corrupt
                ;sub     si,di
                xor     di,di
                mov     ebx,es:[di]
                mov     cx,(DF_FATSIZE)/4-5
                mov     di,4
                mov     edx,ebx
@@compcrc:      mov     eax,es:[di]
                add     di,4
                add     ebx,eax
                xor     edx,eax
                loop    @@compcrc
                cmp     ebx,ds:[DFF_FATADDCRC]
                jne     @@err_corrupt
                cmp     edx,ds:[DFF_FATXORCRC]
                jne     @@err_corrupt
                mov     ax,DOS_FN_FH_LSEEK*256+2
                mov     bx,ds:[DFF_HANDLE]
                xor     cx,cx
                xor     dx,dx
                int     21h
                mov     gs:[doserrn],al
                jc      @@err_corrupt
                shl     edx,16
                mov     dx,ax
                mov     ds:[DFF_DDSIZE],edx
                mov     bx,ds:[DFF_NFILES]
                shl     bx,4
                mov     eax,ds:[bx]
                or      bx,8
                add     eax,ds:[bx]
                cmp     edx,eax
                jb      @@err_corrupt
                mov     ds:[DFF_SEEKPOS],edx
@@ok:           mov     ax,fs
                mov     ds,ax
                popa
                xor     al,al
@@open_exit:    mov     word ptr ds:[DFF_IDOPEN],DF_IDOPEN_CONST
                ret
nidf_getnfo endp

;***************************************************************** nidf_close
;closes ni-datafile
; in :  ds:si - loaded fat
; out:  al: 0-no_err, 1-notopen, 2-open (ah=doserrn)
; mod:  -

nidf_close proc far
                cmp     word ptr [si+DFF_IDOPEN],DF_IDOPEN_CONST
                jne     @@err_notopen
                push    bx
                inc     byte ptr [si+DFF_IDOPEN]
                mov     ah,DOS_FN_FH_CLOSE
                mov     bx,[si+DFF_HANDLE]
                int     21h
                pop     bx
                mov     ah,al
                mov     gs:[doserrn],al
                jc      @@err_open
@@ok:           xor     al,al
                ret
@@err_notopen:  mov     al,DF_ER1_NOTOPEN
                ret
@@err_open:     mov     al,DF_ER1_OPEN
                ret
nidf_close endp