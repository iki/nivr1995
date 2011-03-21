;******************************************** pltview (THIS IS TO BE REMOVED)
pltview    proc far
           mov         ax,ds
           mov         es,ax

           ;mov         di,offset BUFFER
           ;GET_PAL     0,256

           mov         eax,lapsp
           shr         eax,4
           mov         ds,ax

           mov         si,81h
@@skip_bl: ;mov         al,byte ptr es:[si]
           ;inc         si
           lodsb
           cmp         al,32
           je          @@skip_bl
           cmp         al,9
           je          @@skip_bl
           cmp         al,13
           jne         @@par_ok
           mov         ax,1
           DPRINTC     'P'
           jmp         @@quit
@@par_ok:
           mov         di,offset BUFFER+600h
           mov         si,5ch
           mov         cx,08h
           repne       movsd
           mov         ax,es
           mov         ds,ax

           mov         dx,offset BUFFER+600h
           mov         ah,0fh
           int         21h
           cmp         al,0
           je          @@open_ok
           DPRINTC     'O'
           jmp         @@quit
@@open_ok:
           cmp         dword ptr ds:[BUFFER+0610h],64768
           je          @@len_ok
           DPRINTC     'L'
           jmp         @@quit
@@len_ok:
           mov         byte ptr ds:[BUFFER+620h],0

           mov         cx,500
           mov         ax,0a000h
           mov         es,ax
           xor         di,di
@@read_raw:mov         si,offset BUFFER+300h
           mov         dx,si
           mov         ah,1ah
           int         21h
           mov         dx,offset BUFFER+600h
           mov         ah,14h
           int         21h
           push        cx
           mov         cx,20h
           rep         movsd
           pop         cx
           loop        @@read_raw

           mov         cx,6
           mov         di,offset BUFFER+300h
@@read_plt:mov         dx,di
           mov         ah,1ah
           int         21h
           mov         dx,offset BUFFER+600h
           mov         ah,14h
           int         21h
           add         di,80h
           loop        @@read_plt

@@VGA_ok:  mov         si,offset PAL
	   mov         di,offset BUFFER+300h
           mov         bl,FADE_CONST0
           call        fade_pal
@@quit:    mov         ax,data16
           mov         ds,ax
	   ret
pltview    endp