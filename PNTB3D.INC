
; spocita promene pro Brensenhamuv algoritmus kresleni usecky
; vstup:
; esi ->+0  xz,yz ( zacatek odkud se bude usecka kreslit )
;       +4  xk,yk ( konecny bod usecky )
; vystup:
; edi ->+0  x,y ( aktualni pozice bodu )
;       +4  xinc,yinc ( hodnota, ktera se bude pricitat k x-ove nebo y-ove sour.)
;                  ( +1 nebo -1 coz je krok )
;       +8  x|y ( jeli 1 pak je dx > dy )
;       +10 bincr ( chyba, ktera se pricita )
;       +12 p ( ukazatel velikosti chyby jeli p>=0 pak chyba dostatecne velika )
;       +14 aincr ( usmerni chybu, kdyz je prilis velika )


inic            proc    near
                mov     ax,[esi]             ; ax <- xz
                mov     bx,[esi+2]           ; bx <- yz
                mov     cx,[esi+4]           ; cx <- xk
                mov     dx,[esi+6]           ; dx <- yk
                mov     [edi],ax             ; uloz pocatecni bod jako
                mov     [edi+2],bx           ; aktualni
                mov     word ptr [edi+4],-1  ; nastav xincr na -1
                sub     ax,cx                ; a zjisti, ktery bod lezi
                jnc     @@1$                 ; blize ( xz-xk )
                neg     ax                   ; abs(dx)
                                             ; jestlize xk>xz pak prirustek
                mov     word ptr [edi+4],1   ; ve smeru osy X je +1
@@1$:           mov     word ptr [edi+6],-1  ; nastav yincr na -1
                sub     bx,dx                ; a zjisti, ktery bod lezi
                jnc     @@2$                 ; blize ( yz-yk )
                neg     bx                   ; abs(dy)
                mov     word ptr [edi+6],1   ; jeslize yk>yz pak prirustek
                                             ; ve smeru osy Y je +1
@@2$:           mov     word ptr [edi+8],0   ; nastav, ze vetsi je dy
                cmp     ax,bx                ; porovnej dx a dy
                jb      @@y_ova              ; a nenili dy>dx
                xchg    ax,bx
                mov     word ptr [edi+8],1   ; pak nastav, ze vetsi je dx
@@y_ova:        add     ax,ax                ; spocti bincr
                mov     word ptr [edi+10],ax ; uloz
                sub     ax,bx                ; spocti p ( rel. chyba )
                mov     word ptr [edi+12],ax ; uloz
                sub     ax,bx                ; spocti aincr
                mov     word ptr [edi+14],ax ; uloz
                ret
inic            endp


; zjisti nasledujici pozici bodu na usecce pomoci Bresenhamova alg.
; esi ->+0  x,y ( aktualni pozice bodu )
;       +4  xinc,yinc ( hodnota, ktera se bude pricitat k x-ove nebo y-ove sour.)
;                  ( +1 nebo -1 coz je krok )
;       +8  x|y ( jeli 1 pak je dx > dy )
;       +10 bincr ( chyba, ktera se pricita )
;       +12 p ( ukazatel velikosti chyby jeli p>=0 pak chyba dostatecne velika )
;       +14 aincr ( usmerni chybu, kdyz je prilis velika )


brn             proc    near
                cmp     word ptr [esi+8],1   ; zjisti zda dx>dy
                jnz     @@y                  ; jestize ne pak pokracuj na dy>dx
                mov     ax,[si+4]            ; udelej krok ve smeru osy X
                add     [si],ax
                cmp     word ptr [esi+12],0  ; jeli chyba jiz dost velka
                js      @@dal
                mov     ax,[esi+6]           ; pak udelej krok ve smeru osy Y
                add     [esi+2],ax
                mov     ax,[esi+14]
                add     [esi+12],ax          ; a usmerni chybu
                ret
@@dal:          mov     ax,[esi+10]          ; jinak zvetsi chybu
                add     [esi+12],ax
                ret
@@y:            mov     ax,[esi+6]           ; udelej krok ve smeru osy Y
                add     [esi+2],ax
                cmp     word ptr [esi+12],0  ; jeli chyba dost velka
                js      @@dal1
                mov     ax,[esi+4]           ; pak udelej ve smeru osy X
                add     [esi],ax
                mov     ax,[esi+14]          ; a usmerni chybu
                add     [esi+12],ax
                ret
@@dal1:         mov     ax,[esi+10]          ; jinak zvetsuj chybu
                add     [esi+12],ax
@@semto:        ret
brn             endp


; zjisti do, ktere pulky kvadrantu spada aktualni uhel pohledu
; vstup:
; [akt_angle] -> aktualni uhel pohledu
; vystup:
; si -> adresa rutiny, ktera tuto pulku kvadrantu obslouzi
; [hupik] -> zredukovana velikost uhlu

inih            proc    near
                mov     ax,word ptr [akt_angle]
                cmp     ax,32768
                ja      nad_180
                cmp     ax,16384
                jb      pod_90
                cmp     ax,16384+8192
                ja      nad_135
                sub     ax,16384
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_pod_135
                ret
nad_135:        sub     ax,16384+2*8192
		neg	ax
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_nad_135
                ret
pod_90:         cmp     ax,8192
                ja      nad_45
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_pod_45
                ret
nad_45:         sub     ax,2*8192
		neg	ax
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_nad_45
                ret
nad_180:        cmp     ax,32768+16384
                jb      pod_270
                cmp     ax,32768+16384+8192
                ja      nad_315
                sub     ax,32768+16384
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_pod_315
                ret
nad_315:        sub     ax,65535;32768+16384+8192*2
		neg	ax
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_nad_315
                ret
pod_270:        cmp     ax,32768+8192
                ja      nad_225
                sub     ax,32768
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_pod_225
                ret
nad_225:        sub     ax,32768+8192*2
		neg	ax
                shr     ax,5
                mov     word ptr [hupik],ax
                mov     si,offset uhel_nad_225
                ret
inih            endp


uhel_pod_45     proc    near
                add     ax,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                add     bx,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_pod_45     endp


uhel_nad_45     proc    near
                add     bx,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                add     ax,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_nad_45     endp


uhel_pod_135    proc    near
                add     bx,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                sub     ax,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_pod_135    endp


uhel_nad_135    proc    near
                sub     ax,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                add     bx,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_nad_135    endp


uhel_pod_225    proc    near
                sub     ax,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                sub     bx,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_pod_225    endp


uhel_nad_225    proc    near
                sub     bx,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                sub     ax,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_nad_225    endp


uhel_pod_315    proc    near
                sub     bx,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                add     ax,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_pod_315    endp


uhel_nad_315    proc    near
                add     ax,word ptr [_hop+2*ebp]
                mov     cx,word ptr [hupik]
                shl     bp,9
                sub     bx,word ptr gs:[vejir+ebp+2*ecx]
                shr     bp,9
                ret
uhel_nad_315    endp


; zobrazi 3D scenu
; vstup:
; [man_x] -> x-ova pozice clobrdika v mape
; [man_y] -> y-ova pozice clobrdika v mape
; [man_angle] -> uhel pohledu clobrdika v mape
; [delta_x] -> velikost x-ove steny
; [delta_y] -> velikost y-ove steny
; vystup:
; [MAP_VRAM] -> spoctena 3D scena


_view           proc    far
                push    ds
		push    es
		xor	eax,eax
		xor	ebx,ebx
		xor	ecx,ecx
		xor	edx,edx
                xor     esi,esi
                xor     edi,edi
		xor	ebp,ebp
                mov     ax,gs
                mov     ds,ax                   ; potrebne nastaveni registru
                mov     si,offset pom_poz
                mov     ax,word ptr [_man_angle]
                mov     word ptr [akt_angle],ax
                sub     ax,uhel_odklonu*koef_uhlu
                shr     ax,2
                mov     eax,dword ptr [angle2xy+4*eax]
                mov     dword ptr [esi],7fff7fffh
                mov     [esi+4],eax
                mov     di,offset man
                call    inic                    ; usecka smeru clobrdika
                mov     eax,dword ptr [_man_x]  ; pozice clobrdika
                mov     [edi],eax
                mov     word ptr [RAY_X],0      ; aktualni cislo hor. paprs.

paprs:          xor     ebx,ebx
                xor     ecx,ecx
                xor     ebp,ebp                 ; nastaveni reg.
                mov     ax,word ptr [akt_angle]
                call    inih                    ; zjisti v kterem pulkvadrantu
                mov     ax,word ptr [man]       ; lezi aktualni paprsek
                mov     bx,word ptr [man+2]
                mov     dx,[RAY_X]
                add     dx,dx                   ; nastav ukazatele do MAP_VRAM
                mov     di,dx
                add     edx,VRAM_X*(VRAM_Y-1)*2
@@0:            cmp     ax,ZAKL_POS             ; uz se akt. paprsek protl s
                jl      texturuj                ; nejakou stenou
                cmp     bx,ZAKL_POS
                jl      texturuj
                cmp     ax,word ptr [_delta_x]
                jg      texturuj
                cmp     bx,word ptr [_delta_y]
                jg      texturuj
                mov     ch,bl                   ; pokud ne pokracuj
                mov     cl,al                   ; v posilani vertikalnich
                and     ch,07fh                 ; paprsku
                shl     cl,1                    ; a jejic cestu zaznamenej
                shr     cx,1                    ; do 3D sceny
                push    ax
                mov     ax,word ptr [textury+2*ecx]
                mov     word ptr [MAP_VRAM+edx],ax
                mov     ax,word ptr [textury+2*16384+2*ecx]
                mov     word ptr [MAP_VRAM+edi],ax
                pop     ax
                ; mapovani
                call    si
                inc     ebp
                sub     edx,VRAM_X*2
                add     edi,VRAM_X*2
                jmp     @@0
texturuj:
                add     bp,bp                    ; uz jsme narazily a
                sub     bp,VRAM_Y                ; ted zanamename stenu
                neg     bp
                cmp     ax,word ptr [_delta_x]
                jg      druha
                cmp     ax,ZAKL_POS
                jl      druha
                mov     cl,al
		add	cl,bl
                and     cl,127
                jmp     vse_ok
druha:          mov     cl,bl
		add	cl,al
                and     cl,127
vse_ok:         xor     ch,ch
                mov     di,word ptr [poz_zed+2*ebp]

semka:          and     cx,16383             ; da se optimizovat pric zap c.
                mov     ax,word ptr [textury+2*32768+2*ecx]
                mov     word ptr [MAP_VRAM+edx],ax
		sub	edx,VRAM_X*2
                add     cx,word ptr [hopzed+edi]
		add	di,2
                cmp     cx,32767
                jb      semka


                mov     si,offset man             ; spocti
                call    brn                       ; vychozi bod pro nasl. h. papr.
                sub     word ptr [akt_angle],delta_paprsku ; uhel dalsiho h. papr.
                inc     word ptr [RAY_X]  ; aktualni cislo h. paprsku
                cmp     word ptr [RAY_X],MAX_X ; uz jsme rozprostreli celi vejir
                jnz     paprs                  ; paprsku
		pop 	es
                pop     ds
                retf
_view           endp


