VGA_IOMSB       EQU 3
VGA_IOMSBW      EQU 300h

VGA_M_MISC_R    EQU 0CCh
VGA_M_MISC_W    EQU 0C2h
VGA_M_INS0_R    EQU 0C2h
VGA_M_INS1_R    EQU 0DAh
VGA_M_FEAT_R    EQU 0CAh
VGA_M_FEAT_W    EQU 0DAh
VGA_M_VSSE_RW   EQU 0C3h

VGA_C_ADDR_W    EQU 0D4h
VGA_C_DATA_RW   EQU 0D5h
VGA_CI_HT       EQU 0
VGA_CI_HDEE     EQU 1
VGA_CI_HBS      EQU 2
VGA_CI_HBE      EQU 3
VGA_CI_HRS      EQU 4
VGA_CI_HRE      EQU 5
VGA_CI_VT       EQU 6
VGA_CI_OVFL     EQU 7
VGA_CI_PRS      EQU 8
VGA_CI_MAXSL    EQU 9
VGA_CI_CURS     EQU 0ah
VGA_CI_CURE     EQU 0bh
VGA_CI_SADDRH   EQU 0ch
VGA_CI_SADDRL   EQU 0dh
VGA_CI_CURPH    EQU 0eh
VGA_CI_CURPL    EQU 0fh
VGA_CI_VRS      EQU 10h
VGA_CI_VRE      EQU 11h
VGA_CI_VDEE     EQU 12h
VGA_CI_OFFSET   EQU 13h
VGA_CI_UNDLINL  EQU 14h
VGA_CI_VBS      EQU 15h
VGA_CI_VBE      EQU 16h
VGA_CI_MODECTRL EQU 17h
VGA_CI_LINCMP   EQU 18h

VGA_S_ADDR_W    EQU 0C4h
VGA_S_DATA_RW   EQU 0C5h
VGA_SI_RESET    EQU 0
VGA_SI_CLKMOD   EQU 1
VGA_SI_MAPMSK   EQU 2
VGA_SI_CHRMAPSL EQU 3
VGA_SI_MEMMOD   EQU 4

VGA_G_ADDR_W    EQU 0CEh
VGA_G_DATA_RW   EQU 0CFh
VGA_GI_SRESET   EQU 0
VGA_GI_ESR      EQU 1
VGA_GI_COLCMP   EQU 2
VGA_GI_DATROT   EQU 3
VGA_GI_RMAPSEL  EQU 4
VGA_GI_GRMODE   EQU 5
VGA_GI_MISC     EQU 6
VGA_GI_COLNCR   EQU 7
VGA_GI_BITMSK   EQU 8

VGA_A_ADDR_RW   EQU 0C0h
VGA_A_DATA_RW   EQU 0C0h
VGA_AI_EGAPAL   EQU 0
VGA_AI_AMODCN   EQU 10h
VGA_AI_OVSCOL   EQU 11h
VGA_AI_COLPLE   EQU 12h
VGA_AI_HPELPAN  EQU 13h
VGA_AI_COLSEL   EQU 14h

VGA_DAC_ST_R    EQU 0C7h
VGA_PEL_RA_W    EQU 0C7h
VGA_PEL_WA_RW   EQU 0C8h
VGA_PEL_DATA_RW EQU 0C9h
VGA_PEL_MASK_RW EQU 0C6h

VRT_FADE_PAL macro sreg,nsteps,srcpal,dstpal,addpal2,fadpal2 ;additionalflags
local end_m,fillpalette,invranges,go_fade
;;         test   byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
;;         jz     end_m
         cmp    byte ptr sreg:[palf_nsteps],0
         jnz    end_m

         mov    di,offset addpal2
         mov    bx,offset fadpal2
         mov    bp,offset dstpal
         mov    si,offset srcpal
;;         mov    byte ptr sreg:[palf_i_arcolors],0
;;         mov    byte ptr sreg:[palf_i_wait],1
;;         mov    byte ptr sreg:[palf_step],step
;;;         mov    byte ptr sreg:[palf_nsteps],al
;;         mov    byte ptr sreg:[palf_waitstep],waitstep
;;         mov    byte ptr sreg:[palf_waitback],waitback
         mov    word ptr sreg:[palf_addpal2off],di
         mov    word ptr sreg:[palf_fadpal2off],bx

         OPTZER0 eax
         mov    cx,300h
         mov    dl,80h
fillpalette:
         mov    al,nsteps
         mov    dh,sreg:[si]
         mov    ah,sreg:[bp]
         sub    ah,dh
         jb     invranges
         mov    ax,gs:[2*eax+DIVTAB_8_8_8x8]
         inc    si
         inc    bp
         mov    sreg:[bx],dx
         mov    sreg:[di],ax
         add    bx,2
         add    di,2
         OPTLOOP fillpalette
         jmp    short go_fade
invranges:
         neg    ah
         ;inc    ah
         mov    ax,gs:[2*eax+DIVTAB_8_8_8x8]
         inc    si
         neg    ax
         inc    bp
         ;inc    ax
         mov    sreg:[bx],dx
         mov    sreg:[di],ax
         add    bx,2
         add    di,2
         OPTLOOP fillpalette
go_fade:
;;IFNB <additionalflags>
;;         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO+additionalflags
;;ELSE
;;         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
;;ENDIF
         ;mov    byte ptr sreg:[palf_nsteps],nsteps
         mov    word ptr sreg:[palf_nsteps],128*256+nsteps
end_m:
endm


VRT_IMMFADE_PAL macro sreg,nsteps,srcpal,dstpal,addpal2,fadpal2 ;additionalflags
local end_m,fillpalette,invranges,go_fade

;;         and    byte ptr sreg:[vrt_request_flags],not VRT_RF_PF_GO
         mov    byte ptr sreg:[palf_nsteps],0

         mov    di,offset addpal2
         mov    bx,offset fadpal2
         mov    bp,offset dstpal
         mov    si,offset srcpal
;;         mov    byte ptr sreg:[palf_i_arcolors],0
;;         mov    byte ptr sreg:[palf_i_wait],1
;;         mov    byte ptr sreg:[palf_step],step
;;;         mov    byte ptr sreg:[palf_nsteps],al
;;         mov    byte ptr sreg:[palf_waitstep],waitstep
;;         mov    byte ptr sreg:[palf_waitback],waitback
         mov    word ptr sreg:[palf_addpal2off],di
         mov    word ptr sreg:[palf_fadpal2off],bx

         OPTZER0 eax
         mov    cx,300h
         mov    dl,80h
fillpalette:
         mov    al,nsteps
         mov    dh,sreg:[si]
         mov    ah,sreg:[bp]
         sub    ah,dh
         jb     invranges
         mov    ax,gs:[2*eax+DIVTAB_8_8_8x8]
         inc    si
         inc    bp
         mov    sreg:[bx],dx
         mov    sreg:[di],ax
         add    bx,2
         add    di,2
         OPTLOOP fillpalette
         jmp    short go_fade
invranges:
         neg    ah
         ;inc    ah
         mov    ax,gs:[2*eax+DIVTAB_8_8_8x8]
         inc    si
         neg    ax
         inc    bp
         ;inc    ax
         mov    sreg:[bx],dx
         mov    sreg:[di],ax
         add    bx,2
         add    di,2
         OPTLOOP fillpalette
go_fade:
;;IFNB <additionalflags>
;;         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO+additionalflags
;;ELSE
;;         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
;;ENDIF
         ;mov    byte ptr sreg:[palf_nsteps],nsteps
         mov    word ptr sreg:[palf_nsteps],128*256+nsteps
end_m:
endm


VRT_FADEB_PAL macro sreg,step,waitstep,waitback,dstpal,additionalflags
local end_m
         test   byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
         jz     end_m
         mov    byte ptr sreg:[palf_i_wait],1
         mov    byte ptr sreg:[palf_step],step
         mov    byte ptr sreg:[palf_waitstep],waitstep
         mov    byte ptr sreg:[palf_waitback],waitback
         mov    word ptr sreg:[palf_dstpaloff],offset dstpal
IFNB <additionalflags>
         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO+additionalflags
ELSE
         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
ENDIF

end_m:
endm


VRT_IMMFADEB_PAL macro sreg,step,waitstep,waitback,dstpal,additionalflags
local end_m
         and    byte ptr sreg:[vrt_request_flags],not VRT_RF_PF_GO
         mov    byte ptr sreg:[palf_i_arcolors],0
         mov    byte ptr sreg:[palf_i_wait],1
         mov    byte ptr sreg:[palf_step],step
         mov    byte ptr sreg:[palf_waitstep],waitstep
         mov    byte ptr sreg:[palf_waitback],waitback
         mov    word ptr sreg:[palf_dstpaloff],offset dstpal
IFNB <additionalflags>
         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO+additionalflags
ELSE
         or     byte ptr sreg:[vrt_request_flags],VRT_RF_PF_GO
ENDIF

end_m:
endm


GET_PAL  macro  start_reg,n_reg
         mov    dx,VGA_IOMSBW+VGA_PEL_RA_W
         mov    al,start_reg
         out    dx,al
         mov    cx,3*(n_reg-start_reg)
         inc    dl
         inc    dl
         rep insb
endm

SET_PAL  macro  start_reg,n_reg
         mov    dx,VGA_IOMSBW+VGA_PEL_WA_RW
         mov    al,start_reg
         out    dx,al
         mov    cx,3*(n_reg-start_reg)
         inc    dl
         rep outsb
endm


GET_PAL_VAR macro  start_reg,n_reg
         mov    dx,VGA_IOMSBW+VGA_PEL_RA_W
         mov    al,start_reg
         out    dx,al
         mov    cl,n_reg
         inc    dl
         xor    ah,ah
         sub    cl,start_reg
         inc    dl
         mov    al,cl
         shl    ax,1
         add    cx,ax
         rep insb
endm

SET_PAL_VAR macro  start_reg,n_reg
         mov    dx,VGA_IOMSBW+VGA_PEL_WA_RW
         mov    al,start_reg
         out    dx,al
         mov    cl,n_reg
         xor    ah,ah
         sub    cl,start_reg
         inc    dl
         mov    al,cl
         shl    ax,1
         add    cx,ax
         rep outsb
endm

ZER0_PAL_MACRO macro
         mov    dx,3c8h
         xor    al,al
         out    dx,al
         mov    cx,300h
         inc    dl
@@zer0_dac:
         out    dx,al
         loop   @@zer0_dac
endm

WAIT_VRTE macro
         local VRTE_wait
         mov    dx,VGA_IOMSBW+VGA_M_INS1_R
VRTE_wait:
         in     al,dx
         test   al,8
         jnz    VRTE_wait
endm

WAIT_VRT macro
         local VRT_wait
         mov    dx,VGA_IOMSBW+VGA_M_INS1_R
VRT_wait:
         in     al,dx
         test   al,8
         jz     VRT_wait
endm

VGA_DIS0 macro
         mov    dx,VGA_IOMSBW+VGA_M_INS1_R
         in     al,dx
	 mov	dl,VGA_A_ADDR_RW
         xor    al,al
         out    dx,al
         mov	dl,VGA_M_INS1_R
         in	al,dx
endm

VGA_ENA0 macro
         mov    dx,VGA_IOMSBW+VGA_M_INS1_R
         in     al,dx
         mov    dl,VGA_A_ADDR_RW
         mov    al,32
         out    dx,al
         mov	dl,VGA_M_INS1_R
         in	al,dx
endm

VGA_DIS1 macro
         mov    dx,VGA_IOMSBW+VGA_C_ADDR_W
         mov    al,VGA_CI_MODECTRL
         out    dx,al
         inc    dx
         in     al,dx
         and    al,7fh
         out    dx,al
endm

VGA_ENA1 macro
         mov    dx,VGA_IOMSBW+VGA_C_ADDR_W
         mov    al,VGA_CI_MODECTRL
         out    dx,al
         inc    dx
         in     al,dx
         or     al,80h
         out    dx,al
endm

VGA_DISABLE macro
         mov    dx,VGA_IOMSBW+VGA_S_ADDR_W
         mov    al,VGA_SI_RESET
         out    dx,al
         inc    dx
         in     al,dx
         or     al,00100000b
         out    dx,al
endm

VGA_ENABLE macro
         mov    dx,VGA_IOMSBW+VGA_S_ADDR_W
         mov    al,VGA_SI_RESET
         out    dx,al
         inc    dx
         in     al,dx
         and    al,11011111b
         out    dx,al
endm

VGA_PEL_MASK_SET macro val
IFNB <val>
                mov      al,val
ELSE
                mov      al,0ffh
ENDIF
                mov      dx,VGA_IOMSBW+VGA_PEL_MASK_RW
                out      dx,al
endm

VGA_SADDRH_SET macro val
IFNB <val>
                mov      ah,val
ELSE
                OPTZER0  ah
ENDIF
                mov      al,VGA_CI_SADDRH
                mov      dx,VGA_IOMSBW+VGA_C_ADDR_W
                out      dx,ax
endm

VGA_SADDRL_SET macro val
IFNB <val>
                mov      ah,val
ELSE
                OPTZER0  ah
ENDIF
                mov      al,VGA_CI_SADDRH
                mov      dx,VGA_IOMSBW+VGA_C_ADDR_W
                out      dx,ax
endm

VGA_XOR_SADDR macro xorval,sreg,pagevar
                mov      ah,byte ptr sreg:[pagevar]
                mov      al,VGA_CI_SADDRH
                xor      ah,xorval
                mov      dl,VGA_C_ADDR_W
                ;mov      dl,VGA_C_DATA_RW
                mov      byte ptr sreg:[pagevar],ah
                out      dx,ax
                ;;out      dx,al
                ;mov      al,byte ptr sreg:[pagevar]
                ;xor      al,xorval
                ;mov      dl,VGA_C_DATA_RW
                ;mov      byte ptr sreg:[pagevar],al
                ;out      dx,al
ENDM

VGA_S_DEFADDR_SET macro
                mov      al,VGA_S_DEFADDR
                mov      dx,VGA_IOMSBW+VGA_S_ADDR_W
                out      dx,al
endm

VGA_C_DEFADDR_SET macro
                mov      al,VGA_C_DEFADDR
                mov      dx,VGA_IOMSBW+VGA_C_ADDR_W
                out      dx,al
endm

VGA_VRAMA_ZER0W macro
                mov    ax,0a000h
                mov    cx,08000h
                OPTZER0 di
                mov    es,ax
                rep    stosw
endm

VGA_VRAMB_ZER0W macro
                mov    ax,0b000h ;0b800h
                mov    cx,08000h ;04000h
                OPTZER0 di
                mov    es,ax
                rep    stosw
endm

VGA_VRAMA_ZER0 macro
                mov    ax,0a000h
                mov    cx,0ffffh
                OPTZER0 di
                mov    es,ax
                rep    stosb
                stosb
endm

VGA_VRAMB_ZER0 macro
                mov    ax,0b000h ;0b800h
                mov    cx,0ffffh ;08000h
                OPTZER0 di
                mov    es,ax
                rep    stosb
                stosb
endm


MOVE2VRAMPB macro m2v_calcseg,m2v_moveseg,m2v_vramseg,m2v_pagesize
local blastloop,blastloopend
         mov    ax,m2v_vramseg
         mov    bx,m2v_calcseg
         mov    fs,ax
         mov    es,bx
         mov    ax,m2v_moveseg
         mov    bx,m2v_pagesize
         mov    ds,ax
blastloop:
         mov    al,es:[bx]
         cmp    al,[bx]
         je     blastloopend
         mov    fs:[bx],al
         mov    [bx],al
blastloopend:
         dec    bx
         jnz    blastloop

         mov    ax,data16
         mov    fs,ax
endm

MOVE2VRAMPN macro m2v_calcseg,m2v_vramseg,m2v_pagesize
         mov    ax,m2v_vramseg
         mov    bx,m2v_calcseg
         mov    cx,m2v_pagesize-1
         OPTZER0 si
         OPTZER0 di
         mov    es,ax
         mov    ds,bx

         rep    movsd
         movsd
endm


MOVE2VRAMUN macro m2v_calcseg,m2v_vramseg,m2v_pagesize
local blastplloop,blastloop
         mov    bh,byte ptr fs:[vga_c_saddrh_db]
         mov    ax,m2v_vramseg
         xor    bh,80h
         mov    cx,m2v_calcseg
         and    bx,8000h
         mov    dx,VGA_IOMSBW+VGA_S_DATA_RW
         mov    bp,bx
         mov    es,ax
         mov    ds,cx
         ;;mov    ax,100h+VGA_SI_MAPMSK
         mov    ah,bh
         mov    al,1
         OPTZER0 bx

blastloop:
         ;;shl    ah,bl
         out    dx,al
         mov    si,bx
         mov    di,bp

         mov    cx,m2v_pagesize/64
blastplloop:
REPT 16
         movsb
         add    si,3
ENDM
         OPTLOOP blastplloop

         inc    bl
         shl    al,1
         test   bl,4
         jz     blastloop

         mov    al,ah
         mov    dl,VGA_C_DATA_RW
         mov    byte ptr fs:[vga_c_saddrh_db],al
         out    dx,al
endm

MOVE2VRAMFN_OLD4 macro m2v_calcbuf,m2v_vramseg,m2v_pagesize
local blastplloop,blastloop,blastloopenter
         mov    bh,byte ptr fs:[vga_c_saddrh_db]
         mov    ax,m2v_vramseg
         xor    bh,80h
         mov    ds,ax
         and    bx,8000h
         ;mov    dx,VGA_IOMSBW+VGA_S_ADDR_W
         mov    dx,VGA_IOMSBW+VGA_S_DATA_RW
         mov    bp,bx

         ;mov    ax,100h+VGA_SI_MAPMSK
         ;out    dx,ax
         ;;mov    ah,bh
         mov    ebx,m2v_calcbuf
         mov    al,1
         ;inc    dl
         ;jmp    short blastloopenter
blastloop:
         ;;shl    ah,bl
         out    dx,al
blastloopenter:
         mov    esi,ebx
         mov    di,bp

         push   ax
         mov    cx,m2v_pagesize/64 ;word(dword(m2v_pagesize/64))
blastplloop:
REPT 8
         mov    ax,gs:[esi]
         mov    [di],ah
         mov    [di+4000h],al
         add    esi,8
         inc    di
         mov    ax,gs:[esi]
         mov    [di],al
         mov    [di+4000h],ah
         add    esi,8
         inc    di
ENDM
         OPTLOOP blastplloop
         pop    ax

         add    bl,2
         shl    al,1
         test   bl,8
         jz     blastloop

         cli
            ;;;mov         ah,byte ptr fs:[vga_c_saddrh_db]
            ;;;mov         al,VGA_CI_SADDRH
            ;;;xor         ah,80h
            ;;;mov         dl,VGA_C_ADDR_W
            ;;;mov         byte ptr fs:[vga_c_saddrh_db],ah
            ;;;out         dx,ax
            ;;mov         al,byte ptr fs:[vga_c_saddrh_db]
            ;;xor         al,80h
            ;;mov         dl,VGA_C_DATA_RW
            ;;mov         byte ptr fs:[vga_c_saddrh_db],al
            ;;out         dx,al
            VGA_XOR_SADDR 80h,fs,vga_c_saddrh_db
         sti
endm

MOVE2VRAMFN macro m2v_calcbuf,m2v_vramseg,m2v_res_x,m2v_res_y ;m2v_pagesize
local blastplla,blastpllb,blastpllc,blastplld
local blaseplla,blasepllb,blasepllc,blaseplld
         mov    bh,byte ptr fs:[vga_c_saddrh_db]
         mov    ax,m2v_vramseg
         xor    bh,80h
         mov    ds,ax
         and    bx,8000h
         ;mov    dx,VGA_IOMSBW+VGA_S_ADDR_W
         mov    dx,VGA_IOMSBW+VGA_S_DATA_RW
         mov    bp,bx

         ;mov    ax,100h+VGA_SI_MAPMSK
         ;out    dx,ax
;---------------------------------
         mov    esi,m2v_calcbuf
         mov    CL,m2v_res_y/2 ;m2v_pagesize/64
         mov    bx,bp
         mov    di,bp
         mov    al,1
         or     bh,40h
         out    dx,al
ALIGN 16
blastplla:
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],al
         mov    [bx],ah
         add    esi,8
         inc    di
         inc    bx
ENDM
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],ah
         mov    [bx],al
         add    esi,8
         inc    di
         inc    bx
ENDM
         dec    cl
         jnz    blastplla
         ;jmp    blastplla
blaseplla:
;---------------------------------
         mov    esi,m2v_calcbuf+2
         mov    CL,m2v_res_y/2 ;m2v_pagesize/64
         mov    bx,bp
         mov    di,bp
         mov    al,2
         or     bh,40h
         out    dx,al
ALIGN 16
blastpllb:
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],ah
         mov    [bx],al
         add    esi,8
         inc    di
         inc    bx
ENDM
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],al
         mov    [bx],ah
         add    esi,8
         inc    di
         inc    bx
ENDM
         dec    cl
         jnz    blastpllb
         ;jmp    blastpllb
blasepllb:
;---------------------------------
         mov    esi,m2v_calcbuf+4
         mov    CL,m2v_res_y/2 ;m2v_pagesize/64
         mov    bx,bp
         mov    di,bp
         mov    al,4
         or     bh,40h
         out    dx,al
ALIGN 16
blastpllc:
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],al
         mov    [bx],ah
         add    esi,8
         inc    di
         inc    bx
ENDM
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],ah
         mov    [bx],al
         add    esi,8
         inc    di
         inc    bx
ENDM
         dec    cl
         jnz    blastpllc
         ;jmp    blastpllc
blasepllc:
;---------------------------------
         mov    esi,m2v_calcbuf+6
         mov    CL,m2v_res_y/2 ;m2v_pagesize/64
         mov    bx,bp
         mov    di,bp
         mov    al,8
         or     bh,40h
         out    dx,al
ALIGN 16
blastplld:
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],ah
         mov    [bx],al
         add    esi,8
         inc    di
         inc    bx
ENDM
REPT (m2v_res_x/4)
         mov    ax,gs:[esi]
         mov    [di],al
         mov    [bx],ah
         add    esi,8
         inc    di
         inc    bx
ENDM
         dec    cl
         jnz    blastplld
         ;jmp    blastplld
blaseplld:
;---------------------------------
         cli
            ;;;mov         ah,byte ptr fs:[vga_c_saddrh_db]
            ;;;mov         al,VGA_CI_SADDRH
            ;;;xor         ah,80h
            ;;;mov         dl,VGA_C_ADDR_W
            ;;;mov         byte ptr fs:[vga_c_saddrh_db],ah
            ;;;out         dx,ax
            ;;mov         al,byte ptr fs:[vga_c_saddrh_db]
            ;;xor         al,80h
            ;;mov         dl,VGA_C_DATA_RW
            ;;mov         byte ptr fs:[vga_c_saddrh_db],al
            ;;out         dx,al
            VGA_XOR_SADDR 80h,fs,vga_c_saddrh_db
         sti
endm