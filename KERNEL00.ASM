;kernel00 - real flat mode
locals @@
;jumps
.486p

ASSUME  SS:STACK16,DS:DATA16,GS:DATA32

RFM_IDT_LIMIT   EQU 03FFh

RFM_GDT_CODE16  EQU 08h
RFM_GDT_DATA16  EQU 10h
RFM_GDT_STACK16 EQU 18h
;RFM_GDT_DATA32  EQU 20h

include _macros.inc
;----------------------------------------------------------------------------
STACK16    SEGMENT USE16 STACK 'STACK16'
                db    STACK_SIZE/8 DUP('STACK#$%')
STACK16    ENDS
;----------------------------------------------------------------------------
DATA16     SEGMENT PUBLIC USE16 'DATA16'

BUFFER          db BUFFER_SIZE/8 dup('BUFFER#%')
PAL             db 128 dup ('PAL#$%')
PAL_ACT         db 128 dup ('PAL&$%')
PAL_STD         db 128 dup ('PAL*$%')
PAL_DST         db 128 dup ('PAL@$%')
PAL2_ADD        db 256 dup ('PAL^$%')
PAL2_FAD        db 256 dup ('PAL!$%')


rfm_idt         db RFM_IDT_LIMIT/4+1 dup('IDT#')
;			Limit	Base	Rights	LimitHi
rfm_gdt		dw	00000h, 00000h, 00000h, 00000h	; unusable
                dw	0FFFFh, 00000h, 09B00h, 0008Fh	; code seg
                dw	0FFFFh, 00000h, 09300h, 0008Fh	; data seg
                dw	00000h, 00000h, 09700h, 00000h	; stack seg
;                dw	0FFFFh, 00000h, 09300h, 0008Fh	; data32 seg
rfm_gdtr	dw	$-rfm_gdt-1
rfm_gdt_ptr     dd      0
                dw      0

rfm_idtr	dw	RFM_IDT_LIMIT
rfm_idt_ptr     dd      0
                dw      0

sav_gdtr	dw	0
sav_gdt_ptr     dd      0
                dw      0

sav_idtr	dw	0
sav_idt_ptr     dd      0
                dw      0

new_irqs        dd      16 dup (0)
old_irqs        dd      16 dup (0)

EXT_TOP00       dd 0
EXT_TOP         dd 0
xmshandler      dd 0
XMS_SAVE_EMBLA  dd 0


EXIT_EVER_SAFE_PROC  label dword
                dw (offset EXIT_POINT),code16
EXITPROC        label dword
                dw (offset QUIT),code16
EXITOP_LFRM     dd 0
EXITOP_RDFS     dd 0
EXITOP_RKBD     dd 0
EXITOP_RMOU     dd 0
EXITOP_RVGA     dd 0


heapheadentries dd 1
heapheade0      dd 1
heapsize        dd 0
heaptop         dd 0

saveptr_mouse0  dd 0
saveptr_vgam3s0 dd 0
saveptr_vganfo0 dd 0

savelen_mouse0  dd 0
savelen_vgam3s0 dd 0
savelen_vganfo0 dd 0

DFS_FATTAB_ADDR dd 0
DFS_FATTAB_SIZE dd 0
DFS_READBUF_ADDR dd 0
DFS_READBUF_SIZE dd 0

CPU_feature     dd 0

pal_ic_tt       db (PAL_INT_MAXTAB*PAL_CON_MAXTAB*PAL_MAX_COLOR_SHADES)/2 dup ('IC')
pal_gc_tt       db (PAL_GAM_MAXTAB*COLORS)/2 dup ('GC')
pal_ic_tt_act   dw PAL_IC_TT_STD

error_msg       dw MAX_ERROR_N dup (0)

save_dfsfat0_handle dw 0

vrt_st_counter  dw 0
timer_mftimer   dw 0
timer_vrt_T     dw 0
timer_vrt_timer dw 0

palf_fadpal2off dw 0
palf_addpal2off dw 0

vga_c_saddr_dw  label word
vga_c_saddrl_db db 0
vga_c_saddrh_db db 0

VGA_SPGOFF      dw 0
XMS_VER         dw 0
XMS_DVER        dw 0
POST_EXTMSK     dw 0
INT15_EXTMSK    dw 0
XMS_EXTMSK      dw 0
XMS_MAXEMBSK    dw 0
XMS_SAVE_HANDLE dw 0
XMS_SAVE_EMBSK  dw 0
EXTMSK          dw 0

DFS_FATTAB_ENTRIES    dw 0
DFS_FATTAB_OPENS      dw 0
DFS_FATTAB_ACTHANDLE  dw 0

DOS_ERRN        dw 0

vrt_key_wait    db 0
vrt_request_flags db 0
request_flags   db 0+REQF_SYSTEM+REQF_EXIT_SYSTEM+REQF_DISABLE_INPUT
resources_flags db 0
mouse_buttons   db 0
error_n         db 0
error_t		db 0+err_req+err_exit+err_err
mms_flags       db 0+MMS_F_EXITONERR
;+MMS_F_FREEMEMONEXIT+MMS_F_FREEFATONCLOSE
;+MMS_F_FREETABONCLOSE+MMS_F_FREETABIFNFAT
CPU             db NCH
CPU_mode        db 0
CPU_vendor      db 12 dup (0),0,0,c$
CPU_model       db 0
CPU_stepping    db 0
CPU_cpuid       db 0
A20             db NCH
ERRORLEVEL      db 1
CMOS_DSB        db 0
VGA_STATUS      db 0
VGA_SMODE       db 0
VGA_SPOINTS     db 0
EXTMS_ERR       db 0
XMS_HMAAV       db 0
XMS_SAVE_STATUS db 0
save_pic0_imr   db 0
save_pic1_imr   db 0
new_pic0_imr    db 0
new_pic1_imr    db 0

palf_nsteps     db 0
palf_first128   db 128
palf_step       db 0
palf_waitstep   db 0
palf_waitback   db 0
palf_i_wait     db 0
palf_i_arcolors db 0
palf_priority   db 0

pal_overscan    db PAL_OVERSCAN_STD
pal_col_xor     db PAL_COL_XOR_STD
pal_flags       db PAL_FLG_STD
pal_intensity   db PAL_INT_STD
pal_contrast    db PAL_CON_STD
;pal_R_intensity   db PAL_R_INT_STD
;pal_R_contrast    db PAL_R_CON_STD
;pal_G_intensity   db PAL_G_INT_STD
;pal_G_contrast    db PAL_G_CON_STD
;pal_B_intensity   db PAL_B_INT_STD
;pal_B_contrast    db PAL_B_CON_STD

;pal_color_shades db PAL_STD_COLOR_SHADES

DFS_ERR_OPEN    db 0
DFS_ERR_CLOSE   db 0
DFS_ERR_READ    db 0

include _data16.inc

DATA16     ENDS
;----------------------------------------------------------------------------
CODE16A    SEGMENT USE16 PUBLIC 'CODE16A'
           ASSUME  CS:CODE16A

include _pc16a.inc

CODE16A    ENDS
;----------------------------------------------------------------------------
CODE16     SEGMENT USE16 PUBLIC 'CODE16'
           ASSUME  CS:CODE16

include _pc16.inc

;********************************************************************** start
;****************************************************************************
start:
                cld
                mov     ax,data16
                mov     bx,ds
                mov     fs,ax
                mov     ds,ax
                mov     word ptr [EXITOP_RVGA],bx
                mov     ax,cs
                mov     word ptr [EXITPROC+2],ax
                call    cpu_id
                push    dx
                ;and     al,CPM
                cmp     al,I86
                je      donttesta20xt
                call    tst_a20
                mov     A20,al
donttesta20xt:
                DPRINTS nikersig

chcpu:          DPRINTS mchcpu
                pop     dx
                mov     ah,9
                int     21h
                test    CPU_mode,CPU_mode_PM
                jz      cpumode_ok
                REG_ERROR merrpm
                DPRINTS mpm
                jmp     short cpumode_e
cpumode_ok:     DPRINTS mrm
cpumode_e:
                mov     al,CPU
                ;and     al,CPM
                cmp     al,CPU_REQ
                jae     cputype_e
                REG_ERROR merrcp
cputype_e:      cmp     CPU_cpuid,2
                jb      cpu_id_info_e
                DPRINTS mchcpu2
                mov     bx,offset CPU_vendor+12
cpu_trunc_vendor:
                dec     bx
                cmp     bx,offset CPU_vendor
                jb      cpu_tr_vend_e
                cmp     byte ptr [bx],0
                je      cpu_trunc_vendor
                cmp     byte ptr [bx],32
                je      cpu_trunc_vendor
                mov     word ptr [bx+1],','+256*32
                add     bx,2
cpu_tr_vend_e:
                mov     byte ptr [bx+1],c$
                DPRINTS CPU_vendor
                DPRINTS mcpufamily
                mov     al,CPU
                call    PS_ALX
                DPRINTS mcpumodel
                mov     al,CPU_model
                call    PS_ALX
                DPRINTS mcpustepping
                mov     al,CPU_stepping
                call    PS_ALX
cpu_id_info_e:
chcpu_e:        GEN_ERROR err_exit+err_err+err_req

chmem:          DPRINTS mchmem
                mov     al,IDX_CMOS_DSB
                out     CMOS_ADDR,al
                jmp     $+2
                in      al,CMOS_DATA
                mov     CMOS_DSB,al
                and     al,BIT_DSB_MEM
                jz      postm_eq_cmosm
                or      EXTMS_ERR,1
                REG_ERROR merrext0
                REG_ERROR merrext0a
                GEN_ERROR err_wrn
postm_eq_cmosm: mov     al,IDX_CMOS_PEXT+1
                out     CMOS_ADDR,al
                jmp     $+2
                in      al,CMOS_DATA
                mov     ah,al
                mov     al,IDX_CMOS_PEXT
                out     CMOS_ADDR,al
                jmp     $+2
                in      al,CMOS_DATA
                mov     POST_EXTMSK,ax
                mov     EXTMSK,ax

                mov     ah,88h
                int     15h
                jnc     int15hwasok
                or      EXTMS_ERR,2
                REG_ERROR merrext2
                GEN_ERROR err_wrn,1,0,0

int15hwasok:    mov     INT15_EXTMSK,ax

                mov	ax,4300h
                int	2Fh
                cmp	al,80h
                jne	noxmshandler
                mov	ax,4310h
                int	2Fh
                mov	word ptr [xmshandler],bx
                mov	word ptr [xmshandler+2],es
                xor	ah,ah
                call	[xmshandler]
                mov     XMS_HMAAV,dl
                mov     XMS_DVER,bx
                mov     XMS_VER,ax
                jmp     short xmsdetection_e
noxmshandler:
                test    EXTMS_ERR,2
                jnz     xmsdetection_e
                mov     ax,EXTMSK
                cmp     ax,EXT_REQK
                jb      xmsdetection_e
                mov     ax,INT15_EXTMSK
                cmp     ax,EXT_REQK
                jb      ext_tsr0
                mov     EXTMSK,ax
                jmp     short xmsdetection_e
ext_tsr0:       REG_ERROR merrext3
                REG_ERROR mwrncrash
                GEN_ERROR err_wrn+err_exit+err_req+err_askex
                mov     EXTMSK,EXT_REQK
xmsdetection_e:
                cmp     EXTMS_ERR,0
                jz      memtestwoerrs
                DPRINTS mchmem2
memtestwoerrs:
                DPRINTS mhex0x
                mov     ax,EXTMSK
                sub     ax,HMA_SIZK
                mov     EXTMSK,ax
                call    PR_AXX

                movzx   eax,ax
                shl     eax,10
                add     eax,EXT_START
                and     eax,not 15
                mov     EXT_TOP00,eax
                mov     EXT_TOP,eax

                DPRINTS mkbabovehma
                mov     eax,EXT_TOP00
                call    PR_EXX
                mov     ax,EXTMSK
                cmp     ax,EXT_REQK-HMA_SIZK
                jae     enough_extmem
                REG_ERROR merrext1
                GEN_ERROR err_err+err_exit+err_req
enough_extmem:

                DPRINTS mchmem2
                DPRINTS mgatea20

                mov     dx,offset mdis
                cmp     A20,0
                jz      mema20testdis
                mov     dx,offset mena
mema20testdis:  mov     ah,9
                int     21h

                DPRINTS mchmem2
                cmp	XMS_VER,0
                jz      memxmsprintna
                DPRINTS mxmsver
                mov     al,byte ptr [XMS_VER+1]
                call    PS_ALX
                DPRINTC '.'
                mov     al,byte ptr [XMS_VER]
                call    PR_ALX
                DPRINTS mxmsdver
                mov     al,byte ptr [XMS_DVER+1]
                call    PS_ALX
                DPRINTC '.'
                mov     al,byte ptr [XMS_DVER]
                call    PR_ALX

                cmp     byte ptr [XMS_VER+1],2
                jb      memxmsvbel2
                mov     ah,XMS_FN_GET_FXMSK
                call    [xmshandler]
                or      ax,ax
                jnz     memxmsgfne
                cmp     bl,0a0h
                jne     memxmsvbel2
memxmsgfne:
                mov     XMS_EXTMSK,dx
                mov     XMS_MAXEMBSK,ax
                DPRINTS mhex0x2
                mov     ax,XMS_EXTMSK
                call    PR_AXX
                DPRINTS mkb
                mov     ax,XMS_EXTMSK
                cmp     ax,EXTMSK
                jb      memxmswrnmneq
                mov     dx,XMS_EXTMSK
                mov     ah,XMS_FN_ALLO_EMB
                call    [xmshandler]
                or      ax,ax
                jz      memxmswrnmneq
                or      XMS_SAVE_STATUS,1
                mov     XMS_SAVE_HANDLE,dx
                mov     ah,XMS_FN_QUER_EMB
                call    [xmshandler]
                or      ax,ax
                jz      memxmswrnmneq
                mov     XMS_SAVE_EMBSK,dx
                DPRINTS mchmem2
                DPRINTS mhex0x
                mov     ax,XMS_SAVE_EMBSK
                call    PR_AXX
                DPRINTS mxmsallocated
                mov     dx,XMS_SAVE_HANDLE
                mov     ah,XMS_FN_LOCK_EMB
                call    [xmshandler]
                or      ax,ax
                jz      memxmsprint_e
                or      XMS_SAVE_STATUS,2
                shl     edx,16
                mov     dx,bx
                mov     XMS_SAVE_EMBLA,edx
                DPRINTS mxmslocked
                mov     eax,XMS_SAVE_EMBLA
                call    PR_EXX
                jmp short memxmsprint_e
memxmswrnmneq:
                REG_ERROR merrext4
                jmp     short memxmserrcont
memxmsvbel2:
                REG_ERROR merrext5
memxmserrcont:
                REG_ERROR mwrncrash
                GEN_ERROR err_wrn+err_exit+err_req+err_askex
                jmp     short memxmsprint_e
memxmsprintna:
                DPRINTS mnoxms
memxmsprint_e:
chmem_e:
;****************************************************************************
chvga:          DPRINTS mchvga
                mov     ax,ds
                mov     es,ax
                mov     ax,01b00h
                xor     bx,bx
                mov     di,offset BUFFER
                int     10h
                cmp     al,01bh
                je      vganexttest
vganotdet:
                DPRINTS mvganotdet
                REG_ERROR merrvga0
                GEN_ERROR err_exit+err_err+err_req
vganexttest:
                mov     ax,word ptr [BUFFER+25h]
                cmp     al,8
                je      vgadet
                cmp     al,7
                je      vgadet
                cmp     ah,8
                je      vganotact
                cmp     ah,7
                jne     vganotdet
vganotact:      or      VGA_STATUS,80h
                DPRINTS mvganotact
                DPRINTS mchvga2
                mov     ax,word ptr [BUFFER+2]
                mov     bx,word ptr [BUFFER]
                mov     es,ax
                mov     al,byte ptr es:[bx+0bh]
                test    al,8
                jnz     vgasetact
vgacantsetact:  REG_ERROR merrvga1
                GEN_ERROR err_err+err_req+err_exit
vgasetact:      mov     bx,word ptr [BUFFER+25h]
                mov     ax,1a01h
                xchg    bl,bh
                int     10h
                cmp     al,1ah
                jne     vgacantsetact
                mov     ax,ds
                mov     es,ax
                mov     ax,01b00h
                xor     bx,bx
                mov     di,offset BUFFER
                int     10h
                mov     al,byte ptr [BUFFER+25h]
                cmp     al,8
                je      vgadet
                cmp     al,7
                jne     vgacantsetact
vgadet:         mov     al,byte ptr [BUFFER+25h]
                and     al,8
                or      VGA_STATUS,al
                mov     al,byte ptr [BUFFER+31h]
                and     al,3
                or      VGA_STATUS,al
                xor     ah,ah
                cmp     al,3
                jae     vga256kb_ok
                REG_ERROR merrvga2
vga256kb_ok:    inc     al
                shl     ax,6
                push    ax
                DPRINTS mhex0x
                pop     ax
                call    PS_AXX
                DPRINTS mvgadet
                mov     dx,offset mmoncolordet
                test    VGA_STATUS,8
                jnz     vgamonprint
                REG_ERROR merrvga4
                mov     dx,offset mmonmonodet
vgamonprint:    mov     ah,9
                int     21h

                mov     ax,word ptr [BUFFER+2]
                mov     bx,word ptr [BUFFER]
                inc     bx
                mov     es,ax
                inc     bx
                mov     al,byte ptr es:[bx]
                test    al,8
                jnz     vga13hsup_e
                REG_ERROR merrvga3
vga13hsup_e:
                GEN_ERROR err_err+err_req+err_exit
chvga_e:
;****************************************************************************
chsound:        ;DPRINTS mchgus
                ;DPRINTS mnotimpl
chsound_e:
;****************************************************************************
chinput:        DPRINTS  mchinput
                mov      ax,3533h
                int      21h
                mov      ax,es
                or       bx,bx
                jnz      mousevectok
                or       ax,ax
                jz       mousenotdet
mousevectok:    xor      ax,ax
                int      33h
                or       ax,ax
                jz       mousenotdet
                mov      ax,bx
                cmp      ax,0ffffh
                jne      mousechbut0
mousechbutb2:   mov      ax,2
                jmp      short mousechbut_e
mousechbut0:    cmp      ax,1
                je       mousechbutb2
                cmp      ax,2
                je       mousechbut_e
mousechbutb3:   mov      ax,3
mousechbut_e:   mov      mouse_buttons,al
                call     PS_ALX
                DPRINTS  mmouse
                or       resources_flags,RESOURCES_MOUSE
                jmp      short mouse_e
mousenotdet:    DPRINTS  mnomouse
                REG_ERROR merrmouse0
                GEN_ERROR err_wrn ;err_exit+err_err+err_req
mouse_e:
chinput_e:
;****************************************************************************
;****************************************************************************
entrfm:         DPRINTS  mrfmenter
                cmp      A20,1
                je       settinga20ok
                call	 set_a20_on
                or       al,al
                jnz      settinga20ok
                REG_ERROR merrea20f
                GEN_ERROR err_exit+err_err+err_req,1,0,0
settinga20ok:   mov      ax,cs
                and      eax,0ffffh
		shl      eax,4
                mov      dword ptr [BUFFER],eax
		mov      word ptr [rfm_gdt+RFM_GDT_CODE16+2],ax
		ror      eax,16
                mov      byte ptr [rfm_gdt+RFM_GDT_CODE16+4],al

                mov      ax,ss
		and      eax,0ffffh
		shl      eax,4
                mov      dword ptr [BUFFER+4],eax
		mov      word ptr [rfm_gdt+RFM_GDT_STACK16+2],ax
		ror      eax,16
                mov      byte ptr [rfm_gdt+RFM_GDT_STACK16+4],al

                mov      ax,ds
		and      eax,0ffffh
		shl      eax,4
                mov      dword ptr [BUFFER+8],eax
                mov      ebx,eax
                mov      word ptr [rfm_gdt+RFM_GDT_DATA16+2],ax
                ror      eax,16
                mov      byte ptr [rfm_gdt+RFM_GDT_DATA16+4],al

                xor      eax,eax
                mov      ax,offset rfm_gdt
		add      ebx,eax
		mov      rfm_gdt_ptr,ebx

                call     rfm_enter

                mov      eax,EXITOP_RVGA
                mov      ebx,dword ptr [BUFFER]
                shl      eax,4
                mov      ecx,dword ptr [BUFFER+4]
                mov      dword ptr gs:[lapsp],eax
                mov      edx,dword ptr [BUFFER+4]
                xor      eax,eax
                mov      dword ptr gs:[lacode16],ebx
                ;mov      ax,data32c
                mov      dword ptr gs:[lastack16],ecx
                ;shl      eax,4
                mov      dword ptr gs:[ladata16],edx
                ;mov      dword ptr gs:[ladata32c],eax

                mov      eax,dword ptr EXITPROC
                mov      dword ptr EXITOP_LFRM,eax
                mov      word ptr EXITPROC,offset EXITP_LFRM
entrfm_e:
;****************************************************************************
;****************************************************************************
initmms:
                DPRINTS  minitmms
                call     initheap
;****************************************************************************
initdfs:
                DPRINTS  minitdfs
                mov      eax,dword ptr EXITPROC
                mov      dword ptr EXITOP_RDFS,eax
                mov      word ptr EXITPROC,offset EXITP_RDFS

                mov      DFS_FATTAB_ENTRIES,DATAFILES_N
                call     dfs_init
                mov      dx,offset datafile0_name
                mov      eax,DATAFILE0_DISP
                call     dfs_open
                cmp      al,0
                je       initdfs_ok
                GEN_ERROR err_err+err_exit
initdfs_ok:
                mov      save_dfsfat0_handle,dx
;;include ex_dfsl.inc

initdfs_e:
;****************************************************************************
initkbd:
                DPRINTS  minitkbd
                mov      eax,dword ptr EXITPROC
                mov      dword ptr EXITOP_RKBD,eax
                mov      word ptr EXITPROC,offset EXITP_RKBD

                mov      bx,data16
                mov      cx,KEYS_SIZE/4
                OPTZER0  eax
                mov      es,bx
                mov      di,offset KEYS
                rep      stosd
                SET_IRQ_VECT 1,ds,keyboard_handler
;DEBUG_INTERPOLL00
;DEBUG_INTERPOLL01
;DEBUG_MMS00
;DEBUG_DFS01
;DEBUG_INT00
;DEBUG_INT01
;DEBUG_VRTH01 1
                KBD_COMMAND1 KBD_SETMI_CMD,KBD_RT_MODEIND
                KBD_COMMAND1 KBD_SETTRD_CMD,KBD_RT_TRD
initkbd_e:
;****************************************************************************
mousesave:
                test     resources_flags,RESOURCES_MOUSE
                jz       mousesave_e
                mov      ax,15h
                int      33h
                test     bx,15
                jz       mousesave_a0
                or       bx,15
                inc      bx
mousesave_a0:
                xor      eax,eax
                cmp      bx,1000h
                mov      ax,bx
                ja       mousesave_e
                mov      savelen_mouse0,eax
                DPRINTS  msavemouse

                mov      cx,ds
                shr      bx,2
                mov      dx,offset BUFFER
                mov      eax,savelen_mouse0
                mov      es,cx

                call     allocp

                mov      saveptr_mouse0,eax
                mov      edi,eax
                mov      cx,bx
                mov      ax,16h
                int      33h

                mov      edi,eax
                mov      si,offset BUFFER
mousesave_si:   lodsd
                mov      gs:[edi],eax
                add      edi,4
                OPTLOOP  mousesave_si

                mov      eax,dword ptr EXITPROC
                mov      dword ptr EXITOP_RMOU,eax
                mov      word ptr EXITPROC,offset EXITP_RMOU
DEBUG_PNUM00
DEBUG_DFS01
DEBUG_MMS00
DEBUG_MMS01 1F2h,1000h,300h,400h
mousesave_e:
;****************************************************************************
vgasave:
                DPRINTS  msavevga

                mov      al,gs:[449h]
                mov      ah,gs:[485h]
                mov      word ptr [VGA_SMODE],ax

                mov      ax,ds
                mov      di,offset BUFFER
                mov      es,ax
                GET_PAL  0,256
                mov      si,offset BUFFER
                mov      cx,0c0h
                mov      edi,offset SAVEPTR_VGAPLT
vgasave_sp:     lodsd
                mov      gs:[edi],eax
                add      edi,4
                OPTLOOP  vgasave_sp

                mov      ax,1c00h
                mov      cx,VGA_BIOS_SAVE
                int      10h
                cmp      al,1ch
                jne      vgasave_2nok
                and      ebx,0ffffh
                shl      ebx,6
                cmp      ebx,BUFFER_SIZE
                ja       vgasave_2nok
                mov      savelen_vganfo0,ebx
                mov      cx,VGA_BIOS_SAVE
                mov      bx,offset BUFFER
                mov      ax,1c01h
                int      10h
                ;cmp      al,1ch
                ;jne      vgasave_2nok

                cmp      VGA_SMODE,3
                ja       vgasave_ae
                xor      eax,eax
                xor      ecx,ecx
                mov      ax,gs:[44ch]
                mov      esi,0B8000h
                mov      cx,gs:[44eh]
                mov      savelen_vgam3s0,eax
                add      esi,ecx
                mov      VGA_SPGOFF,cx
                mov      ecx,eax

                call     allocp

                mov      saveptr_vgam3s0,eax
                shr      ecx,2
                mov      edi,eax
vgasave_a0:     mov      eax,gs:[esi]
                add      esi,4
                mov      gs:[edi],eax
                add      edi,4
                OPTLOOPD vgasave_a0
vgasave_ae:
                mov      eax,savelen_vganfo0
                mov      si,offset BUFFER
                mov      ecx,eax

                call     allocp

                mov      saveptr_vganfo0,eax
                shr      ecx,2
                mov      edi,eax
vgasave_si:     lodsd
                mov      gs:[edi],eax
                add      edi,4
                OPTLOOPD vgasave_si
                ;jmp      short vgasave_2ok
vgasave_2nok:   ;mov      savelen_vganfo0,0
vgasave_2ok:

                ;mov      ax,ds
                ;mov      es,ax

DEBUG_WAIT_KEY

DEBUG_VRTH02 2
                GEN_ERROR err_err+err_exit

                mov      eax,dword ptr EXITPROC
                mov      dword ptr EXITOP_RVGA,eax
                mov      word ptr EXITPROC,offset EXITP_RVGA
                mov      word ptr EXIT_EVER_SAFE_PROC,offset EXITFO_POINT

DEBUG_MMS00
vgasave_e:
;****************************************************************************
;****************************************************************************
;****************************************************************************
;****************************************************************************
                mov      ERRORLEVEL,0

include ex_main.inc

                EXIT
;****************************************************************************
;****************************************************************************
;****************************************************************************
;****************************************************************************
EXITFO_POINT:
                ;;and     byte ptr fs:[vrt_request_flags],not VRT_RF_PF_GO
                mov     ax,data16
                mov     di,offset PAL_DST
                mov     ds,ax
                mov     es,ax
                mov     cx,768/4
                OPTZER0 eax
                mov     word ptr [exit_ever_safe_proc],offset EXIT_POINT
                rep     stosd
                ;;test    byte ptr [vrt_request_flags],VRT_RF_FLIPPAGES
                ;;jz      EXITFO_00
                ;;VRT_IMMFADE_PAL ds,PALF_NSTEPS0,PAL,PAL_DST,PAL2_ADD,PAL2_FAD
                ;;call     end_vretrace
                ;;jmp      short EXIT_POINT
;;EXITFO_00:
                or       byte ptr [request_flags],REQF_EXIT_SYSTEM+REQF_SYSTEM+REQF_DISABLE_INPUT
                call     endimm_vretrace
                ;ZER0_PAL_MACRO
                ;;mov      word ptr [palf_nsteps],8000h
                mov      si,offset BUFFER
                mov      di,offset PAL_DST
                mov      bl,PALF_STEPSYS0
                call     fade_apal
                ;;call     endimm_vretrace
EXIT_POINT:     mov      ax,data16
                mov      ds,ax
                mov      es,ax
                or       byte ptr [request_flags],REQF_EXIT_SYSTEM+REQF_SYSTEM+REQF_DISABLE_INPUT
                and      error_t,not err_exit
                and      mms_flags,not MMS_F_EXITONERR
                jmp      dword ptr EXITPROC
;****************************************************************************
QUIT:           GEN_ERROR2
                DPRINTS  mexit
                test     XMS_SAVE_STATUS,2
                jz       xms_restore_b
xms_restore_a:
                mov      ah,XMS_FN_UNLO_EMB
                mov      dx,XMS_SAVE_HANDLE
                call     [xmshandler]
                ;or       ax,ax
                ;jz       xms_restore_a
xms_restore_b:  test     XMS_SAVE_STATUS,1
                jz       xms_restore_e
                mov      ah,XMS_FN_FREE_EMB
                mov      dx,XMS_SAVE_HANDLE
                call     [xmshandler]
xms_restore_e:
                cmp      A20,0
                jne      DOSEXIT
                call     set_a20_off
DOSEXIT:        mov      ah,4Ch
                mov      al,ERRORLEVEL
                int      21h
;****************************************************************************
EXITP_RVGA:     ;VGA_DIS0
                ;VGA_DISABLE
                VGA_SADDRH_SET
                VGA_SADDRL_SET
;enable all planes
                mov      dx,VGA_IOMSBW+VGA_S_ADDR_W
                mov      ax,0f02h
                out      dx,ax
                cld
                VGA_VRAMA_ZER0
                VGA_VRAMB_ZER0
;disable all planes
                ;;mov      dx,VGA_IOMSBW+VGA_S_ADDR_W
                ;mov      ax,0002h
                ;out      dx,ax

                ;call     zer0_pal
                ;VGA_DIS0
                ;VGA_DISABLE

                ;;;;mov      ax,13h
                ;;;;int      10h
                ;;;;call     zer0_pal
                ;VGA_DIS0
                ;;;;VGA_DIS1
                ;;;;VGA_DISABLE
                ;VGA_PEL_MASK_SET

                xor      ah,ah
                mov      al,VGA_SMODE
                int      10h
                ;VGA_DISABLE
                call     zer0_pal

                mov      al,VGA_SPOINTS
                cmp      al,8
                jne      EXITP_RVGA_Z0
                mov      al,12h
                jmp      short EXITP_RVGA_ZE
EXITP_RVGA_Z0:  cmp      al,14
                jne      EXITP_RVGA_Z1
                mov      al,11h
                jmp      short EXITP_RVGA_ZE
EXITP_RVGA_Z1:  mov      al,14h
EXITP_RVGA_ZE:  mov      ah,11h
                xor      bl,bl
                int      10h

                ;call     zer0_pal
                VGA_DISABLE
                cld
                VGA_VRAMB_ZER0W
                VGA_VRAMA_ZER0W

                mov      ecx,savelen_vganfo0
                or       ecx,ecx
                jz       EXITP_RVGA_BE
EXITP_RVGA_A:   mov      bx,ds
                shr      ecx,2
                mov      di,offset BUFFER
                mov      esi,saveptr_vganfo0
                mov      es,bx
EXITP_RVGA_A0:  mov      eax,gs:[esi]
                add      esi,4
                stosd
                OPTLOOPD EXITP_RVGA_A0

                test     mms_flags,MMS_F_FREEMEMONEXIT
                jz       EXITP_RVGA_NFM0
                mov      eax,saveptr_vganfo0
                mov      edx,savelen_vganfo0
                call     free
DEBUG_MMS00
EXITP_RVGA_NFM0:
                mov	 ax,data16
                mov      bx,offset BUFFER
                mov      es,ax
                mov      cx,VGA_BIOS_SAVE
                mov      ax,1c02h
                int      10h
                VGA_DISABLE
EXITP_RVGA_AE:  call     zer0_pal
EXITP_RVGA_B:   xor      edi,edi
                mov      ecx,savelen_vgam3s0
                xor      eax,eax
                mov      di,[VGA_SPGOFF]
                or       ecx,ecx
                jz       EXITP_RVGA_BE
                mov      esi,saveptr_vgam3s0
                add      edi,0B8000h
                shr      ecx,2
EXITP_RVGA_B0:  mov      eax,gs:[esi]

                add      esi,4
                mov      gs:[edi],eax
                add      edi,4
                OPTLOOPD EXITP_RVGA_B0

                test     mms_flags,MMS_F_FREEMEMONEXIT
                jz       EXITP_RVGA_NFM1
                mov      eax,saveptr_vgam3s0
                mov      edx,savelen_vgam3s0
                call     free
DEBUG_MMS00
EXITP_RVGA_NFM1:

EXITP_RVGA_BE:
EXITP_RVGA_C:   ;VGA_ATTR1
                mov      esi,offset SAVEPTR_VGAPLT
                mov      di,offset BUFFER+300h
                mov      cx,0c0h
EXITP_RVGA_C0:  mov      eax,gs:[esi]
                add      esi,4
                stosd
                OPTLOOP  EXITP_RVGA_C0
                DPRINTS  mrestvga

                VGA_ENA0
                VGA_ENA1
                VGA_ENABLE
                mov      bl,PALF_STEPA0
                mov      si,offset BUFFER
                mov      di,offset BUFFER+300h
                call     fade_apal
                jmp      dword ptr EXITOP_RVGA
;****************************************************************************
EXITP_RMOU:     DPRINTS  mrestmouse
                cld
                mov      bx,data16
                mov      ecx,savelen_mouse0
                mov      es,bx
                mov      di,offset BUFFER
                mov      esi,saveptr_mouse0
                shr      ecx,2
EXITP_RMOU_A0:  mov      eax,gs:[esi]
                add      esi,4
                stosd
                OPTLOOPD EXITP_RMOU_A0
                mov      dx,offset BUFFER
                mov      ax,17h
                int      33h

                test     mms_flags,MMS_F_FREEMEMONEXIT
                jz       EXITP_RMOU_NFM
                mov      eax,saveptr_mouse0
                mov      edx,savelen_mouse0
                call     free
DEBUG_MMS00
EXITP_RMOU_NFM:
                jmp      dword ptr EXITOP_RMOU
;****************************************************************************
EXITP_RKBD:
                ;SET_IRQ_VECT 1,ds,empty_handler
                mov      ah,gs:[0417h]
                shr      ah,4
                and      ah,7
                KBD_COMMAND1 KBD_SETMI_CMD,ah
                KBD_COMMAND1 KBD_SETTRD_CMD,KBD_DF_TRD
                jmp      dword ptr EXITOP_RKBD
;****************************************************************************
EXITP_RDFS:
		mov      ax,DFS_HANDLE_ALLOPEN
           	call     dfs_close
                jmp      dword ptr EXITOP_RDFS
;****************************************************************************
EXITP_LFRM:	DPRINTS  mrfmleave
		call     rfm_leave
                jmp      dword ptr EXITOP_LFRM
CODE16    ENDS
;----------------------------------------------------------------------------
;DATA32C   SEGMENT PUBLIC USE32 'DATA32C'
;
;
;DATA32C   ENDS
;----------------------------------------------------------------------------
DATA32   SEGMENT PUBLIC USE32 'DATA32'
include _data32c.inc
;;	   org   0A0000h
;;vram       label dword
;;
;           ORG   10FFF0h
;lapsp      label dword
;           ORG   $+4
;lacode16   label dword
;           ORG   $+4
;
;;lacode16a  label dword
;;           ORG   $+4
;
;lastack16  label dword
;           ORG   $+4
;ladata16   label dword
;           ORG   $+4
;
;;ladata32c  label dword
;;           ORG   $+4
;
;SAVEPTR_VGAPLT label dword
;               ORG   $+300h
;SINTAB     label dword
; 	    org	  $+SINTAB_SIZE
;COSTAB	    label dword
;           org   $+(SINTAB_SIZE*4)
;;         org   200000h ;138300h ;110000h+300h+5*SINTAB_SIZE
;
;include _data32x.inc
;
;HEAPEXT    EQU $
;HEAPHEAD   label dword
;           ORG   $+HEAPHEADSIZE
;HEAPSTART  label dword
;
DATA32   ENDS
;----------------------------------------------------------------------------
END     start