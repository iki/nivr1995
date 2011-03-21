;****************************************************************** fade_apal
;*** fades actual palette
;*** in :  ds:si=768b buffer; ds:di=pallete1; bl=step
;*** ass:  -
;*** out:  -
;*** mod:  ax-dx,si,di,es
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
         ;jmp    short fade_pstart
;******************************************************************* fade_pal
;*** fades palette
;*** in :  ds:si=pallete0; ds:di=pallete1; bl=step
;*** ass:  -
;*** out:  -
;*** mod:  ax-dx,si,di,es
fade_pal proc far

         cld
;fade_pstart:
         or     bl,bl
         jnz    @@not_0step
         inc    bl
@@not_0step:

@@fadeloop:
         mov    cx,300h
         xor    bh,bh
@@addstep:
         mov    al,[si]
         mov    ah,[di]
         and    ax,DAC_MASK*257
         cmp    ah,al
         je     @@as_l
         mov    bh,1
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
         ;cmp    bh,0
         ;je     @@fadeloop_e

         pushf
         cli

	 ;mov	dx,03c6h
         ;mov    al,0ffh

         xor    al,al

         mov    dx,03c8h
         sub    si,300h
         out    dx,al
         sub    di,300h

         mov    cx,300h;180h
         WAIT_VRTE
         WAIT_VRT
         mov    dl,0c9h
         rep    outsb
         mov    cx,180h
         ;WAIT_VRTE
         ;WAIT_VRT
         ;mov    dl,0c9h
         ;rep    outsb
         popf
         sub    si,300h
         cmp    bh,0
         jnz    @@fadeloop
@@fadeloop_e:
         ret
fade_pal endp
fade_apal endp

;******************************************************************* zer0_pal
;*** zer0es palette
;*** in :  -
;*** ass:  -
;*** out:  -
;*** mod:  ax,cx,dx
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

;****************************************************************** set_xmode
;*** sets xmode (an u320x200x256x4 mode only available at this time)
;*** in :  -
;*** ass:  -
;*** out:  -
;*** mod:  -
set_xmodefk proc far
           push         ax dx di

           mov          ax,0013h
           int          10h

           mov          dx,VGA_IOMSBW+VGA_G_ADDR_W
           mov          al,5
           out          dx,al
           inc          dl
           in           al,dx
           and          al,11101111b
           out          dx,al
           dec          dl

           mov          al,6
           out          dx,al
           inc          dl
           in           al,dx
           and          al,11111101b
           out          dx,al

           mov          dl,VGA_S_ADDR_W
           mov          al,4
           out          dx,al
           inc          dl
           in           al,dx
           and          al,11110111b
           or           al,4
           out          dx,al
           dec          dl

;enable all planes
           mov          ax,0f02h
           out          dx,ax

           VGA_VRAMA_ZER0W

           mov          dl,VGA_C_ADDR_W
           mov          al,9
           out          dx,al
           inc          dl
           in           al,dx
           and          al,01110000b
           out          dx,al
           dec          dl

           mov          al,14h
           out          dx,al
           inc          dl
           in           al,dx
           and          al,10111111b
           out          dx,al
           dec          dl

           mov          al,17h
           out          dx,al
           inc          dl
           in           al,dx
           and          al,10111111b
           out          dx,al

           pop          di dx ax
           ret
set_xmodefk endp

;****************************************************************** set_xmode
;*** sets xmode (unchained, 256 colors)
;*** in :  bl:
;           0      320x175         6       360x175
;           1      320x200         7       360x200
;           2      320x240         8       360x240
;           3      320x350         9       360x350
;           4      320x400         a       360x400
;           5      320x480         b       360x480
;*** ass:  -
;*** out:  -
;*** mod:  -

set_xmode proc far
           pusha
           push         ds es
           mov          dx,data16
           mov          ax,0013h
           OPTZER0      bh
           mov          ds,dx
           int          10h
           ZER0_PAL_MACRO
           VGA_DISABLE
           mov          cl,byte ptr [vga_xmset_miscout_reg+bx]
           mov          dx,VGA_IOMSBW+VGA_S_ADDR_W
           OPTTESTZER0  cl
           jz           @@no_new_hor_clock

;reset
           mov          ax,0100h
           out          dx,ax

           mov          dl,VGA_M_MISC_W
           mov          al,cl
           out          dx,al

;release reset
           mov          dl,VGA_S_ADDR_W
           mov          ax,0300h
           out          dx,ax
@@no_new_hor_clock:
;disable chain 4

;           mov          al,4
;           out          dx,al
;           inc          dx
;           in           al,dx
;           and          al,not 8
;           out          dx,al
;           dec          dx

           mov          ax,0604h
           out          dx,ax

;enable all planes
           mov          ax,0f02h
           out          dx,ax

           VGA_VRAMA_ZER0W

           mov          dl,VGA_C_ADDR_W
           mov          si,offset vga_xmset_crtc_main
;tweaked mode
	   mov          cx,3
           cmp          bl,5
           jbe          @@set_crtc_hor
;tweaked mode + horizontal retrace for 360 columns modes
	   mov          cx,10
@@set_crtc_hor:
           mov          ax,[si]
           out          dx,ax
           add          si,2
           OPTLOOP      @@set_crtc_hor

           shl          bx,1
           mov	        si,word ptr [vga_xmset_vertab+bx]
           cmp          si,1
           jb           @@return
           je           @@set_vert400

           mov          cx,8
@@set_crtc_vert:
           mov          ax,[si]
           out          dx,ax
           add          si,2
           OPTLOOP      @@set_crtc_vert
@@return:
           pop          es ds
           popa
           ret
@@set_vert400:
           mov          ax,04009h
           out          dx,ax
           pop          es ds
           popa
           ret
set_xmode endp