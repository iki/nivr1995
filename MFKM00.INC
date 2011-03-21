;;************************************************************ set_fkm_pal_ic
;;** sets fakemode palette contrast and intensity
;;** in :  di=palette, al=add_to_intensity, ah=add_to_contrast
;;** ass:  -
;;** out:  data16:di=palette
;;** mod:  -
SET_FKM_PAL_IC macro add_int,add_con,palette_ptr,sreg
            SET_FKM_ICV add_int,add_con,sreg
            SET_FKM_ICP palette_ptr,sreg
endm



SET_FKM_PAL_IC_STD macro add_int,add_con,palette_ptr,sreg
            SET_FKM_ICV_STD add_int,add_con,sreg
            SET_FKM_ICP palette_ptr,sreg
endm



SET_FKM_ICV  macro add_int,add_con,sreg
local no_intensity,no_contrast


IF NOT add_int EQ 0
;;            OPTTESTZER0 al
;;            jz          no_intensity
            mov         al,byte ptr sreg:[pal_intensity]
            cmp         al,(PAL_INT_MAX-(add_int))
            jae         no_intensity
            cmp         al,(PAL_INT_MIN-(add_int))
            jl          no_intensity
            mov         si,sreg:[pal_ic_tt_act]
            add         al,(add_int)
            add         si,(PAL_CON_MAXTAB*PAL_MAX_COLOR_SHADES*(ADD_INT))
            mov         byte ptr sreg:[pal_intensity],al
            mov         sreg:[pal_ic_tt_act],si
no_intensity:
ENDIF

IF NOT add_con EQ 0
;;            OPTTESTZER0 ah
;;            jz          no_contrast
            mov         al,byte ptr sreg:[pal_contrast]
            cmp         al,(PAL_CON_MAX-(add_con))
            jae         no_contrast
            cmp         al,(PAL_CON_MIN-(add_con))
            jl          no_contrast
            mov         si,sreg:[pal_ic_tt_act]
            add         al,(add_con)
            ;;and         al,(PAL_CON_MAXTAB-1)
            add         si,(PAL_MAX_COLOR_SHADES*(ADD_CON))
            mov         byte ptr sreg:[pal_contrast],al
            mov         sreg:[pal_ic_tt_act],si
no_contrast:
ENDIF

;;IF NOT (add_int EQ 0 AND add_con EQ 0)
;;            mov         sreg:[pal_ic_tt_act],si
;;ENDIF
endm



SET_FKM_ICV_STD macro add_int,add_con,sreg
local int_a,int_al,int_b,int_bl,int_e
local con_a,con_al,con_b,con_bl,con_e

IF (NOT add_int EQ 255) AND (add_con EQ 255)
            mov         bl,add_int ;;PAL_INT_STD
            mov         cl,byte ptr sreg:[pal_intensity]
            mov         si,sreg:[pal_ic_tt_act]
            mov         ax,PAL_CON_MAXTAB*PAL_MAX_COLOR_SHADES
            sub         cl,bl
            ja          int_a
            je          int_e
            ;;neg         cl
int_bl:     add         si,ax
            inc         cl
            jnz         int_bl
            mov         byte ptr sreg:[pal_intensity],bl
            mov         sreg:[pal_ic_tt_act],si
            jmp         short int_e
int_a:
int_al:     sub         si,ax
            dec         cl
            jnz         int_al
            mov         byte ptr sreg:[pal_intensity],bl
            mov         sreg:[pal_ic_tt_act],si
            ;;jmp         short int_e
int_e:
ENDIF

IF (NOT add_con EQ 255) AND (add_int EQ 255)
            mov         bl,add_con ;;PAL_CON_STD
            mov         cl,byte ptr sreg:[pal_contrast]
            mov         si,sreg:[pal_ic_tt_act]
            mov         ax,PAL_MAX_COLOR_SHADES
            sub         cl,bl
            ja          con_a
            je          con_e
            ;;neg         cl
con_bl:     add         si,ax
            inc         cl
            jnz         con_bl
            mov         byte ptr sreg:[pal_contrast],bl
            mov         sreg:[pal_ic_tt_act],si
            jmp         short int_e
con_a:
con_al:     sub         si,ax
            dec         cl
            jnz         con_al
            mov         byte ptr sreg:[pal_contrast],bl
            mov         sreg:[pal_ic_tt_act],si
            ;;jmp         short con_e
con_e:
ENDIF

IF (NOT add_con EQ 255) AND (NOT add_int EQ 255)
            mov         byte ptr sreg:[pal_intensity],add_int ;;PAL_INT_STD
            mov         byte ptr sreg:[pal_contrast],add_con ;;PAL_CON_STD
            mov         sreg:[pal_ic_tt_act],(offset pal_ic_tt)+((add_int)*PAL_CON_MAXTAB+(add_con))*PAL_MAX_COLOR_SHADES  ;;PAL_IC_TT_STD
ENDIF

;;IF NOT (add_int EQ 0 AND add_con EQ 0)
;;            mov         sreg:[pal_ic_tt_act],si
;;ENDIF
endm

SET_FKM_ICP macro palette_ptr,sreg
local end_of_macro,set_pal_g,set_pal_r,set_pal_b,set_pal_br
local I,Iset_pal_g,Iset_pal_r,Iset_pal_b,Iset_pal_br
local C,Cset_pal_g,Cset_pal_r,Cset_pal_b,Cset_pal_br
local CI,CIset_pal_g,CIset_pal_r,CIset_pal_b,CIset_pal_br

            mov         al,sreg:[pal_flags]
            mov         si,sreg:[pal_ic_tt_act]
            mov         cl,PAL_MAX_COLOR_SHADES
            test        al,PAL_FLG_INVCOL
            jnz         C
            test        al,PAL_FLG_INVINT
            jnz         I
            mov         dl,sreg:[si]
            mov         di,offset palette_ptr
            mov         ah,dl
set_pal_g:  mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            inc         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         set_pal_g

            sub         si,PAL_MAX_COLOR_SHADES
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,dl
set_pal_r:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         set_pal_r

            sub         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-2
            mov         bx,si
            inc         bx
set_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
set_pal_br:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         set_pal_br
            sub         si,PAL_MAX_COLOR_SHADES
            inc         bx
            dec         ch
            jnz         set_pal_b
            jmp         end_of_macro


I:
            mov         dl,sreg:[si]
            mov         di,offset palette_ptr
            add         si,PAL_MAX_COLOR_SHADES-1
            mov         ah,dl
Iset_pal_g: mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            dec         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         Iset_pal_g

            add         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-2
            mov         bx,si
            ;dec         bx
Iset_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
Iset_pal_br:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            dec         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         Iset_pal_br
            add         si,PAL_MAX_COLOR_SHADES
            dec         bx
            dec         ch
            jnz         Iset_pal_b

            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,dl
Iset_pal_r:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            dec         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         Iset_pal_r
            ;add         si,PAL_MAX_COLOR_SHADES
            jmp         end_of_macro


C:
            test        al,PAL_FLG_INVINT
            jnz         CI
            mov         dl,sreg:[si]
            xor         dl,DAC_MASK
            mov         di,offset palette_ptr
            mov         ah,dl
Cset_pal_g:  mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            xor         al,DAC_MASK
            inc         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         Cset_pal_g

            sub         si,PAL_MAX_COLOR_SHADES
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,dl
Cset_pal_r:
            mov         al,sreg:[si]
            xor         al,DAC_MASK
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         Cset_pal_r

            sub         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-2
            mov         bx,si
            inc         bx
Cset_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
            xor         dh,DAC_MASK
Cset_pal_br:
            mov         al,sreg:[si]
            xor         al,DAC_MASK
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         Cset_pal_br
            sub         si,PAL_MAX_COLOR_SHADES
            inc         bx
            dec         ch
            jnz         Cset_pal_b
            jmp         end_of_macro


CI:
            mov         dl,sreg:[si]
            mov         di,offset palette_ptr
            xor         dl,DAC_MASK
            add         si,PAL_MAX_COLOR_SHADES-1
            mov         ah,dl
CIset_pal_g: mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            xor         al,DAC_MASK
            dec         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         CIset_pal_g

            add         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-2
            mov         bx,si
            ;dec         bx
CIset_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
            xor         dh,DAC_MASK
CIset_pal_br:
            mov         al,sreg:[si]
            xor         al,DAC_MASK
            mov         sreg:[di],al
            dec         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         CIset_pal_br
            add         si,PAL_MAX_COLOR_SHADES
            dec         bx
            dec         ch
            jnz         CIset_pal_b

            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,dl
CIset_pal_r:
            mov         al,sreg:[si]
            xor         al,DAC_MASK
            mov         sreg:[di],al
            dec         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         CIset_pal_r
            ;add         si,PAL_MAX_COLOR_SHADES
            ;jmp         end_of_macro
end_of_macro:
endm



SET_FKM_ICP_OLD macro palette_ptr,sreg
local set_pal_g,set_pal_r,set_pal_b,set_pal_br
            OPTZER0     dl
            mov         al,sreg:[pal_intensity]
            mov         si,sreg:[pal_ic_tt_act]
            mov         di,offset palette_ptr
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         ah,dl
set_pal_g:  mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            inc         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         set_pal_g

            sub         si,PAL_MAX_COLOR_SHADES
            mov         cl,PAL_MAX_COLOR_SHADES
            OPTZER0     dx
set_pal_r:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         set_pal_r

            sub         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-2
            mov         bx,si
            inc         bx
set_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
set_pal_br:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         set_pal_br
            sub         si,PAL_MAX_COLOR_SHADES
            inc         bx
            dec         ch
            jnz         set_pal_b
endm


;;************************************************************ set_fkm_pal_ic
;;** sets fakemode palette contrast and intensity
;;** in :  di=palette, al=add_to_intensity, ah=add_to_contrast
;;** ass:  -
;;** out:  data16:di=palette
;;** mod:  -
SET_FKM_PAL_IC_OLD macro add_int,add_con,palette_ptr,sreg
local set_pal_g,set_pal_r,set_pal_b
            mov         si,sreg:[pal_ic_tt_act]

IF NOT add_int EQ 0
;;            OPTTESTZER0 al
;;            jz          @@no_intensity
            mov         al,byte ptr sreg:[pal_intensity]
            cmp         al,PAL_INT_MAX-add_int
            jae         @@no_intensity
            add         al,add_int
            add         si,PAL_CON_MAXTAB*PAL_MAX_COLOR_SHADES*ADD_INT
            mov         byte ptr sreg:[pal_intensity],al
;;@@no_intensity:
ENDIF

IF NOT add_con EQ 0
;;            OPTTESTZER0 ah
;;            jz          @@no_contrast
            mov         al,byte ptr sreg:[pal_contrast]
            cmp         al,PAL_CON_MAX-add_con
            jae         @@no_contrast
            add         al,add_con
            add         si,PAL_MAX_COLOR_SHADES*ADD_CON
            mov         byte ptr sreg:[pal_contrast],al
;;@@no_contrast:
ENDIF

IF NOT (add_int EQ 0 AND add_con EQ 0)
            mov         sreg:[pal_ic_tt_act],si
ENDIF

            mov         di,offset palette_ptr
            mov         cl,PAL_MAX_COLOR_SHADES
            OPTZER0     dl
            OPTZER0     ah
set_pal_g:  mov         sreg:[di],dl
            mov         al,sreg:[si]
            inc         di
            inc         si
            mov         sreg:[di],ax
            add         di,2
            dec         cl
            jnz         set_pal_g

            sub         si,PAL_MAX_COLOR_SHADES
            mov         ch,PAL_MAX_COLOR_SHADES-1
            mov         bx,si
            inc         bx
set_pal_b:
            mov         cl,PAL_MAX_COLOR_SHADES
            mov         dh,sreg:[bx]
set_pal_r:
            mov         al,sreg:[si]
            mov         sreg:[di],al
            inc         si
            inc         di
            mov         sreg:[di],dx
            add         di,2
            dec         cl
            jnz         set_pal_r
            sub         si,PAL_MAX_COLOR_SHADES
            inc         bx
            dec         ch
            jnz         set_pal_b
endm
