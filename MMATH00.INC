PREPARE_SINTAB macro
local fileload_ok,mknext4parts
                OPTZER0  eax
   		OPTZER0  ecx
                mov      ebx,offset SINTAB
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,SINTAB_FILEID
                call     dfs_read
                or       al,al
                jz       fileload_ok
                GEN_ERROR err_err+err_exit1
fileload_ok:
                push     ds
                OPTZER0  ax
                mov      cx,SINTAB_SIZE/2
                mov      esi,offset SINTAB
                mov      edi,offset SINTAB+SINTAB_SIZE+SINTAB_SIZE-4
                mov      ebx,offset SINTAB+2*SINTAB_SIZE
                mov      ebp,offset SINTAB+3*SINTAB_SIZE+SINTAB_SIZE-4
                mov      ds,ax
mknext4parts:   mov      ax,word ptr ds:[esi]
                mov      word ptr ds:[esi+SINTAB_SIZE*4],ax
                mov      word ptr ds:[edi],ax
                neg      ax
                inc      ax
                mov      word ptr ds:[ebx],ax
                mov      word ptr ds:[ebp],ax
                add      esi,2
                add      ebx,2
                sub      edi,2
                sub      ebp,2
                OPTLOOP mknext4parts
                pop      ds
endm

MKDIVTAB_8_8_8x8 macro
local loopcl,loopch,loopzer0div
                push     ds
                OPTZER0  ax
                mov      ebx,offset DIVTAB_8_8_8x8
                mov      ds,ax
                ;mov      dword ptr [ebx],0FFFF0000h
                ;mov      eax,            0FFFFFFFFh
                ;add      ebx,4
                OPTZER0  eax
                mov      cx,128 ;127
loopzer0div:    mov      [ebx],eax
                add      ebx,4
                OPTLOOP  loopzer0div
                mov      ch,1
loopch:
                ;OPTZER0  cl
                mov      word ptr [ebx],0FFFFh
                mov      cl,1
                add      ebx,2
loopcl:
                mov      al,ch
                OPTZER0  ah
                div      cl
                mov      dh,al
                OPTZER0  al
                div      cl
                mov      dl,al
                mov      [ebx],dx
                add      ebx,2

                inc      cl
                jnz      loopcl

                inc      ch
                jnz      loopch

                pop      ds
endm