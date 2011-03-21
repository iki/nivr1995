DEBUG_ENABLE_DEBUG_TEXT macro
IF 1 EQ 0
                mov      ax,3
                int      10h
ENDIF
endm

DEBUG_WAIT_KEY macro
IF 1 EQ 0
                WAIT_KEY
ENDIF
endm

DEBUG_PRINT_VRT_T macro
IF 1 EQ 0
                mov      ax,fs:[timer_vrt_T]
                call     PR_AXX
ENDIF
endm


DEBUG_PRINT_PAL00 macro PAL_PTR
IF 1 EQ 0
                mov      si,offset PAL_PTR
                mov      dx,0310h
                mov      bx,0710h
                call     print_palette
ENDIF
endm

DEBUG_INTERPOLL01 macro
IF 1 EQ 0
                mov      ah,PAL_MAX_COLOR_SHADES
                mov      cx,PAL_CON_MAXTAB+256*PAL_INT_MAXTAB
                mov      di,offset pal_ic_tt
                call     prepare_pal_ic_tt
;;                mov      cl,ah
;;                call     print_pal_int_tt
                call     print_pal_ic_tt
ENDIF
endm

DEBUG_INTERPOLL02 macro
IF 1 EQ 0
                mov      ah,PAL_MAX_COLOR_SHADES
                mov      cx,PAL_CON_MAXTAB+256*PAL_INT_MAXTAB
                mov      di,offset pal_ic_tt
;;                call     prepare_pal_ic_tt
                call     print_pal_ic_tt
ENDIF
endm

DEBUG_INTERPOLL01OLD macro
IF 0 EQ 1
                mov      cx,PAL_MAX_COLOR_SHADES+256*PAL_INT_MAXTAB
                mov      di,offset pal_ic_tt
                call     prepare_pal_ic_tt
                call     print_pal_ic_tt
ENDIF
endm

DEBUG_INTERPOLL02OLD macro
IF 0 EQ 1
                mov      cx,PAL_MAX_COLOR_SHADES+256*PAL_INT_MAXTAB
                mov      di,offset pal_ic_tt
;;                call     prepare_pal_ic_tt
                call     print_pal_ic_tt
ENDIF
endm


DEBUG_INTERPOLL00 macro
local printloop,printloop2,printloop3
IF 1 EQ 0
                mov      ax,data16
                mov      di,offset BUFFER
                mov      dx,1
                mov      al,1
                mov      ah,40h
                mov      cx,16
                push     di
                call     interpol_lin
                pop      di
printloop:      mov      al,[di]
                call     PR_ALX
                inc      di
                OPTLOOP  printloop

                mov      al,40h
                mov      ah,1
                mov      cx,16
                push     di
                call     interpol_lin
                pop      di
printloop2:     mov      al,[di]
                call     PR_ALX
                inc      di
                OPTLOOP  printloop2

                mov      al,3Fh
                mov      ah,al
                mov      cx,16
                push     di
                call     interpol_lin
                pop      di
printloop3:     mov      al,[di]
                call     PR_ALX
                inc      di
                OPTLOOP  printloop3

                mov      al,40h
                mov      ah,1
                mov      cx,1
                push     di
                call     interpol_lin
                pop      di
                mov      al,[di]
                call     PR_ALX

                mov      al,20h
                mov      ah,1
                mov      cx,0
                push     di
                call     interpol_lin
                pop      di
                mov      al,[di]
                call     PR_ALX

                WAIT_KEY
ENDIF
endm

DEBUG_INT00     macro
IF 0 EQ 1
                mov      ax,[rfm_gdtr]
                call     PR_AXX
                mov      eax,[rfm_gdt_ptr]
                call     PR_EXX
                mov      ax,[rfm_idtr]
                call     PR_AXX
                mov      eax,[rfm_idt_ptr]
                call     PR_EXX
                mov      ax,[sav_gdtr]
                call     PR_AXX
                mov      eax,[sav_gdt_ptr]
                call     PR_EXX
                mov      ax,[sav_idtr]
                call     PR_AXX
                mov      eax,[sav_idt_ptr]
                call     PR_EXX
		WAIT_KEY
ENDIF
endm

DEBUG_INT01     macro
IF 0 EQ 1
                mov      ah,[save_pic1_imr]
                mov      al,[save_pic0_imr]
                call     PR_AXB
                mov      ah,[new_pic1_imr]
                mov      al,[new_pic0_imr]
                call     PR_AXB
		WAIT_KEY
ENDIF
endm

DEBUG_VRTH01    macro irqnum
IF 0 EQ 1
                SET_IRQ_VECT irqnum,ds,vrtsth1
                SET_IRQ_VECT 1,ds,vrtsth1
ENDIF
endm

DEBUG_VRTH02    macro irqnum
IF 0 EQ 1
;;;                  SET_IRQ_VECT irqnum,ds,vrtsth2
                  SET_IRQ_VECT 2,ds,vrtsth2
;;                  SET_IRQ_VECT 9,ds,vrtsth2
                  call     vga_vr_ena_irq2
ENDIF
endm

DEBUG_DFS00     macro
IF 0 EQ 1
                call     printdfsinfo
ENDIF
endm

DEBUG_DFS01     macro
IF 0 EQ 1
DISPLAY "DEBUG_DFS01 no more implemented"
ENDIF
endm

DEBUG_MMS00     macro
IF 0 EQ 1
;;;                mov      eax,EXT_TOP
;;;                call     PR_EXX
                call     printheapinfo
ENDIF
endm

DEBUG_MMS01     macro size1,size2,size3,size4
IF 0 EQ 1
                call     printheapinfo
                mov      eax,size1
                call     alloc
                mov      ebx,eax
                call     printheapinfo

                mov      eax,size2
                call     alloc
                mov      ecx,eax
                call     printheapinfo

                mov      eax,size3
                call     alloc
                mov      esi,eax
                call     printheapinfo

                mov      eax,size4
                call     alloc
                xchg     eax,ebx
                call     printheapinfo

                mov      edx,size1
                call     free
                call     printheapinfo

                mov      edx,size2
                mov      eax,ecx
                call     free
                call     printheapinfo

                mov      edx,size4
                mov      eax,ebx
                call     free
                call     printheapinfo

                mov      edx,size3
                mov      eax,esi
                call     free
                call     printheapinfo
ENDIF
endm

DEBUG_OLD_FADEPAL macro
local test_fp0g,test_fp0rgbg,test_fp0rb
IF 0 EQ 1
                ;;mov	 ax,1
		;;int	 33h

                mov      ax,ds
                mov      es,ax
                mov      di,offset BUFFER
                mov      ax,3F00h
                mov      cx,100h
test_fp0g:      stosw
                stosb
                OPTLOOP  test_fp0g

                mov      cx,80h
		mov	 al,3fh
                OPTZER0  bx
test_fp0rgbg:   stosw
                mov      [di],bx
                inc      di
                inc      di
                stosw
                OPTLOOP  test_fp0rgbg

                mov      cx,100h
		mov	 ax,003fh
                OPTZER0  ah
test_fp0rb:     stosw
                stosb
                OPTLOOP  test_fp0rb

                mov      cx,180h
		OPTZER0	 ax
                rep      stosw

                mov      si,offset PAL
                mov      di,offset BUFFER
                mov      bl,PALF_STEP0
                call     fade_apal

                mov      si,offset PAL
                mov      di,offset BUFFER+300h
                mov      bl,0
                call     fade_pal

                mov      si,offset PAL
                mov      di,offset BUFFER+600h
                mov      bl,PALF_STEP0
                call     fade_pal

                mov      si,offset PAL
                mov      di,offset BUFFER+900h
                mov      bl,PALF_STEP0
                call     fade_pal
ENDIF
endm

DEBUG_VGA00     macro
IF 0 EQ 1
                mov      ax,0a000h
                OPTZER0  di
                mov      es,ax
                mov      cx,256
                rep      stosb
ENDIF
endm

DEBUG_PNUM00    macro
IF 0 EQ 1
                mov      eax,001230ABCh
                call     PR_EXX
                call     PS_EXX
                mov      eax,000000ABCh
                call     PR_EXX
                call     PS_EXX
                mov      eax,001230000h
                call     PR_EXX
                call     PS_EXX
                mov      eax,000000000h
                call     PR_EXX
                call     PS_EXX
                mov      al,00Fh
                call     PR_ALX
                call     PS_ALX
                call     PR_ALB
                call     PS_ALB
                mov      al,000h
                call     PR_ALX
                call     PS_ALX
                call     PR_ALB
                call     PS_ALB
		WAIT_KEY
ENDIF
endm

DEBUG_FILL_AREA macro size_reg,fill_reg,add_fill,seg_reg,addr_reg,add_addr
local fillarealoop
;;IFNB <add_addr>
fillarealoop:
                mov      seg_reg:[addr_reg],fill_reg
                add      fill_reg,add_fill
                add      addr_reg,add_addr
                dec      size_reg
                jnz      fillarealoop
;;ENDIF
endm

DEBUG_FILL_AREA2 macro size_reg,fill_reg,add_fill,seg_reg,addr_reg,add_addr
local fillarealoop
;;IFNB <add_addr>
                mov      cx,seg_reg
                mov      di,offset addr_reg
                mov      ds,cx
                mov      al,fill_reg
                mov      cx,size_reg
fillarealoop:
                mov      ds:[di],al
                add      al,add_fill
                add      di,add_addr
                dec      cx
                jnz      fillarealoop
;;ENDIF
endm

DEBUG_FKM1_FMV macro
local fillah,fillal
                mov      edi,MAP_VRAM
                OPTZER0  ah
fillah:
                OPTZER0  al
fillal:
REPT 8
                mov      gs:[edi],ax
                add      edi,2
ENDM

                inc      al
                test     al,16
                jz       fillal

                inc      ah
                jnz      fillah
endm

DEBUG_FKM0_FMV macro
local fillah,fillal
                mov      esi,MAP_VRAM
                mov      al,16
fillal:
                OPTZER0  ah
                mov      edi,esi
fillah:
REPT 8
                mov      gs:[edi],ax
                add      edi,RESX*2
ENDM
                inc      ah
                test     ah,16
                jz       fillah

                add      esi,2

                inc      al
                jnz      fillal
endm