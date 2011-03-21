fade_apal proc far
         cld
         mov    dx,ds
         mov    ah,bl
         mov    es,dx
         mov    bx,di
         mov    di,si
         GET_PAL 0,256
         mov    di,bx
         mov    bl,ah
         jmp    short fade_pstart
fade_pal proc far

;ds:si - palette 0, ds:di - palette 1
;bl..step
         cld
fade_pstart:
         mov    cx,300h
         xor    dl,dl
@@maxdif:
         mov    al,[si]
         mov    ah,[di]
         and    ax,DAC_MASK*256+DAC_MASK
         mov    [si],al
         mov    [di],ah
         cmp    ah,al
         jae    @@md_end_gr
         sub    al,ah
         jmp    short @@md_cmp_max
@@md_end_gr:
         sub    ah,al
         mov    al,ah
@@md_cmp_max:
         cmp    dl,al
         jae    @@md_l
         mov    dl,al
@@md_l:  inc    si
         inc    di
         loop   @@maxdif

         or     dl,dl
         jnz    @@not_0times
         inc    dl
@@not_0times:
         xor    ah,ah
         mov    al,dl
         cmp    bl,1
         je     @@notrc
         jb     @@no0step
         div    bl
         inc    al
	 jmp	short @@notrc
@@no0step:
	 inc	bl
@@notrc:
         mov    bh,al
	 ;mov	ax,bx
	 ;call	PR_AXX
         sub    si,300h
         sub    di,300h

@@fadeloop:
         mov    cx,300h
@@addstep:
         mov    al,[si]
         mov    ah,[di]
         cmp    ah,al
         je     @@as_l
         ja     @@as_end_gr2
         sub    al,bl
         jbe    @@as_rplc
         cmp    ah,al
         jae    @@as_rplc
         jmp    short @@as_rplc2
@@as_end_gr2:
         add    al,bl
         jo     @@as_rplc
         cmp    ah,al
         jbe    @@as_rplc
@@as_rplc2:
         mov    ah,al
@@as_rplc:
         mov    [si],ah
@@as_l:  inc    di
         inc    si
         loop   @@addstep

	 xor	al,al
         mov    dx,03c8h
         sub    si,300h
         out    dx,al
         sub    di,300h

         pushf
         cli
         mov    cx,180h
         VSYNC0
         VSYNC1
         mov    dl,0c9h
         rep    outsb
         mov    cx,180h
         VSYNC0
         VSYNC1
         mov    dl,0c9h
         rep    outsb
         popf
         sub    si,300h
         dec    bh
         jnz    @@fadeloop
@@fadeloop_e:
         ret
fade_pal endp
fade_apal endp

zer0_pal proc far
         mov    dx,3c8h
         xor    al,al
         out    dx,al
         mov    cx,300h
         inc    dl
@@zer0_dac:
         out    dx,al
         loop   @@zer0_dac
         ret
zer0_pal endp