ALIGN 2

;data from gurumode by Per Ole Klemetsrud, code rewritten
vga_xmset_crtc_main label word
	dw	00e11h		;Index 11, Turn off write protect
	dw	00014h		;Index 14, Turn off dword mode
	dw	0e317h		;Index 17, Turn on byte mode
;crtc regs; 360 columns
;horizontal retrace and blanking periods are based on bios mode 10h and 12h.
vga_xmset_crtc_360c label word
	db	00h,06bh	;Horizontal Total		107
	db	01h,059h	;Horizontal Display End		 89
	db	02h,05ah	;Start Horizontal Blanking	 90
	db	03h,08eh	;End Horizontal Blanking
	db	04h,05eh	;Start Horizontal Retrace	 94
	db	05h,08ah	;End Horizontal Retrace
	db	13h,02dh	;Offset register		 90

;crtc regs; 480 rows; derived from bios mode 12h
vga_xmset_crtc_480r label word
	db	009h,01000000b	;Maximum Scan Line		  0
	db	007h,00111110b	;Overflow register
	db	006h,00bh	;Vertical Total			523
	db	010h,0eah	;Vertical Retrace Start 	490
	db	011h,08ch	;Vertical Retrace End
	db	015h,0e7h	;Start Vertical Blanking	487
	db	016h,004h	;End Vertical Blanking
	db	012h,0dfh	;Vertical Display End		479
	db	012h,0dfh	;Safe dummy for neat program flow
;crtc regs; 350 rows; derived from bios mode 10h
vga_xmset_crtc_350r label word
	db	009h,01000000b	;Maximum Scan Line		  0
	db	007h,00011111b	;Overflow register
	db	006h,0bfh	;Vertical Total    		447
	db	010h,083h	;Vertical Retrace Start 	387
	db	011h,085h	;Vertical Retrace End
	db	015h,063h	;Start Vertical Blanking	355
	db	016h,0bah	;End Vertical Blanking
	db	012h,05dh	;Vertical Display End		349
	db	012h,05dh	;Safe dummy for neat program flow
;sync polarities and horizontal clock
vga_xmset_miscout_reg label byte
	db	0a3h, 000h, 0e3h, 0a3h, 000h, 0e3h
	db	0a7h, 067h, 0e7h, 0a7h, 067h, 0e7h

vga_xmset_vertab label word
        dw	vga_xmset_crtc_350r+2,0000h,vga_xmset_crtc_480r+2
        dw      vga_xmset_crtc_350r  ,0001h,vga_xmset_crtc_480r
	dw	vga_xmset_crtc_350r+2,0000h,vga_xmset_crtc_480r+2
        dw      vga_xmset_crtc_350r  ,0001h,vga_xmset_crtc_480r
