{$M 16384,262140,655360}

uses
 PCXunit,S_Grafik,check_hp;

const
 ZacatPal: array[0..15,1..3]of byte=
(( 0, 0, 0),( 0, 0,42),( 0,42, 0),( 0,42,42),
 (42, 0, 0),(42, 0,42),(42,21, 0),(42,42,42),
 (21,21,21),(21,21,63),(21,63,21),(21,63,63),
 (63,21,21),(63,21,63),(63,63,21),(63,63,63));


type
 PSHI = ^shortint;

 THashTable = array [0 .. 6660] of record R,G,B : shortint; Cislo: Byte end;


var
 PObrazek : UkPaper;
 R,G,B    : UkPaper;
 HashTable : ^THashTable;

 Paleta,PL2 : PRegs;

 f: file;

 p0:PByte;  p1,p2,p3 : ^shortint;

 P_LN,P_L:  ^shortint;


 cyklus,cyklus2 : word;

 Kolikaty :  Boolean;

 Dummy : longint;
 Delta : array [Boolean, 0..319] of record  r,g,b: shortint; end;
 Dummy2 : longint;

 Pomocna : word;


type
 PColBar=^ColBar;
 ColBar = record
              r1,g1,b1,r2,g2,b2 : shortint;  { rozmery krychlicky }
              Pocet : word;                   { pocet barev v ni }
              sumr,sumg,sumb : word;          { pro prumer }

              suma : longint;  {suma eukl. vzdalenosti od stredu krychle}
              Next : PColBar;
         end;

var
 RC,GC,BC,RC2,GC2,BC2 : byte;
 BitPal   : array[0..4095] of byte;
 PocetBarev : word;


 PPrvni,PPomCB,PZpracCB   : PColBar;

 _rd,_gd,_bd,_rh,_gh,_bh : shortint;
  _P : word;
  _sumR,_sumG,_sumB : word;
  _suma : longint;

 stredR,stredG,stredB : shortInt;



procedure HledejPaletu;
begin
 p1:= Pointer(r); p2:= pointer(g); p3:= pointer(b);
 for cyklus:= 0 to 63999 do begin
   Cyklus2:=  (p1^ shr 1 )*32*32 + (p2^ shr 1 )*32+(p3^ shr 1 );
   BitPal[cyklus2 shr 3]:= BitPal[cyklus2 shr 3]or(1 shl (cyklus2 and 7 ));
   Inc( p1); Inc(p2); Inc(p3)
  end;

 (*
 for Cyklus:= 0 to 15 do begin
   Cyklus2:=
      (ZacatPal[Cyklus,1] shr 1)*32*32+(ZacatPal[Cyklus,2]shr 1)*32
           +ZacatPal[Cyklus,3] shr 1;
   BitPal[cyklus2 shr 3]:= BitPal[cyklus2 shr 3]and
          { okrajove barvy jiz zarazeny }   (not (1 shl (cyklus2 and 7 )));
  end;
 *)
 New( PPrvni );
 with PPrvni^ do begin
   r1:=0; g1:=0; b1:=0;  r2:=31; g2:=31; b2:=31;
   Pocet:=0;  sumr:=0; sumg:=0; sumb:=0;  Pocet:=0; suma:=0; Next:= nil
  end;
 PZpracCB:= PPrvni;  PocetBarev:=17;


 repeat

  with PZpracCB^ do
    for RC:= 0 to 1 do
     for GC:= 0 to 1 do
      for BC:= 0 to 1 do begin
        _rd:= r1;  _rh:= r2; Pomocna:= (r1+r2) shr 1;
        if Rc = 0 then _rh:=Pomocna else _rd:= Pomocna + 1;
        _gd:= g1;  _gh:= g2; Pomocna:= (g1+g2) shr 1;
        if gc = 0 then _gh:=Pomocna else _gd:= Pomocna + 1;
        _Bd:= B1;  _Bh:= B2; Pomocna:= (B1+B2) shr 1;
        if Bc = 0 then _Bh:=Pomocna else _Bd:= Pomocna + 1;

                                 { novou krychli rovnou prohledame }
        _p:=0; _sumR:=0; _sumG:=0; _sumB:=0; _suma:=0;
        stredR:= (_Rd+_Rh) shr 1; stredG:=(_Gd+_Gh) shr 1;
        stredB:= (_Bd+_Bh) shr 1;

        for  Rc2:= _rd to _rh do
         for  Gc2:= _gd to _gh do
          for  Bc2:= _bd to _bh do begin
            Cyklus2:= Rc2*32*32+Gc2*32+Bc2;
            if BitPal[Cyklus2 shr 3] and ( 1 shl (Cyklus2 and 7))<> 0
              then begin
                inc(_P);
                inc( _sumR,Rc2); inc( _sumG,Gc2); Inc( _sumB,Bc2 );
                inc(_suma, (Rc2-stredR)*(Rc2-stredR)+(Gc2-stredG)*(Gc2-stredG)
                    + (Bc2-stredB)*(Bc2-stredB));
                end
           end;

        if (_p <> 0) and(PocetBarev <>256) then begin
          New(PPomCB ); PPomCB^.Next:= PPrvni; PPrvni:= PPomCB;
          Inc( PocetBarev );
          with PPrvni^ do begin
             r1:=_rd; r2:=_rh; g1:=_gd; g2:=_gh; b1:=_bd; b2:= _bh;
             suma:=_suma; Pocet:=_p;
             sumR:= _sumR; sumB:= _sumB; sumG:= _sumG;
            end
         end
      end;

  if (PPrvni <> PZpracCB) and( PocetBarev <> 16{256}) then
                         { to zn., ze byly nalezeny nove krychle }
   begin
     PPomCB:=PPrvni;
     while PPomCB^.Next <> PZpracCB do PPomCB:= PPomCB^.Next;
     PPomCB^.Next:= PZpracCB^.Next; Dispose(PZpracCB); Dec( PocetBarev );

     Pomocna:=0;

     PPomCB:= PPrvni;
     while PPomCB <> nil do begin
       if PPomCB^.suma >= Pomocna then
           begin Pomocna:= PPomCB^.Suma; PZpracCB:= PPomCB end;
       PPomCB:= PPomCB^.Next
      end;
   end
    else Pomocna:= 0
 until ( Pomocna = 0 { pouze krychle 1x1x1 nebo PocetBarev = 256});

 FillChar( Paleta^,3*16,0);
{ Move( ZacatPal,Paleta^,3*16);}
 PPomCB:= PPrvni;
 for Cyklus:= 0 to 15 do
   if PPrvni <> nil then
   with PPrvni^ do begin
    if Pocet <> 0 then begin
      Paleta^[Cyklus,1]:= (2*sumR) div Pocet;
      Paleta^[Cyklus,2]:= (2*sumG) div Pocet;
      Paleta^[Cyklus,3]:= (2*sumB) div Pocet;
     end;
    PPomCB:= PPrvni; PPrvni:= PPrvni^.Next;
    Dispose(PPomCB )
   end;
end;

begin
 New(POBrazek); NEw(R); New(G); New(B); New(Paleta); New(PL2);
 if Paramcount = 0 then halt;
 {if LoadPCX(paramstr(1),PObrazek,Paleta) = 1 then halt;}
 assign(f,paramstr(1));
 reset(f,1);
 Blockread(f,PL2^,768);
 close(F);
 for cyklus:=0 to 255 do begin
   Paleta^[cyklus,1]:=PL2^[cyklus,1];
   Paleta^[cyklus,2]:=PL2^[cyklus,2];
   Paleta^[cyklus,3]:=PL2^[cyklus,3]
 end;

 InitGraph;
 Screen:=PObrazek^;

 p0:= Pointer(PObrazek); p1:= Pointer(R); p2:=Pointer(G); p3:= Pointer(B);
 for cyklus:= 0 to  63999 do
  begin
   P1^:= Paleta^[P0^,1] and 254 { 11111110B };
   P2^:= Paleta^[P0^,2] and 254;
   P3^:= Paleta^[P0^,3] and 254;
   Inc(p0); Inc(p1); Inc(p2); Inc(p3)
  end;

{ asm mov ax,1017h; mov bx,0; mov cx,0100h; les dx, paleta; int 10h; end;}
{ asm xor ax,ax; int 16h end;}
 HledejPaletu;  SetPalette( Paleta^);

 p0:= ptr($a000,0); p1:= Pointer(R); p2:=Pointer(G); p3:= Pointer(B);

 New(HashTable);
 FillChar(HashTable^,SizeOf(HashTable^),0);

 for Cyklus2:= 0 to 199 do begin
     P_LN:= pointer( @Delta[not kolikaty] ); Dec(P_LN,3);
     P_L:= pointer( @Delta[ kolikaty]); Dec(P_L,3);
     for Cyklus:= 0 to 319 do begin
       Inc(P1^, (P_LN^ {div 16}));  Dec(P_LN^,P_LN^ div 16);  Inc(P_LN);
       Inc(P2^, (P_LN^ {div 16}));  Dec(P_LN^,P_LN^ div 16);  Inc(P_LN);
       Inc(P3^, (P_LN^ {div 16}));  Dec(P_LN^,P_LN^ div 16);  Inc(P_LN);

       Inc(P1^, ((5*P_LN^) div 16));  Dec(P_LN^,(5*P_LN^) div 16);  Inc(P_LN);
       Inc(P2^, ((5*P_LN^) div 16));  Dec(P_LN^,(5*P_LN^) div 16);  Inc(P_LN);
       Inc(P3^, ((5*P_LN^) div 16));  Dec(P_LN^,(5*P_LN^) div 16);  Inc(P_LN);

       Inc(P1^, ((3*P_LN^) div 16));  Dec(P_LN^,(3*P_LN^) div 16);  Inc(P_LN);
       Inc(P2^, ((3*P_LN^) div 16));  Dec(P_LN^,(3*P_LN^) div 16);  Inc(P_LN);
       Inc(P3^, ((3*P_LN^) div 16));  Dec(P_LN^,(3*P_LN^) div 16);
       Dec(P_LN,5);

       Inc(P1^, ((7*P_L^) div 16));  Dec(P_L^,(7*P_L^) div 16);  Inc(P_L);
       Inc(P2^, ((7*P_L^) div 16));  Dec(P_L^,(7*P_L^) div 16);  Inc(P_L);
       Inc(P3^, ((7*P_L^) div 16));  Dec(P_L^,(7*P_L^) div 16);  Inc(P_L);

{      if p1^>200 then p1^:=0; if p1^> 63 then p1^:=63;
       if p2^>200 then p2^:=0; if p2^> 63 then p2^:=63;
       if p3^>200 then p3^:=0; if p3^> 63 then p3^:=63; }
{$Q-}
       with HashTable^[(word(p1^)*32*16+word(p2^)*16+word(p3^))mod 6661] do begin
        if (R=P1^)and(G=p2^)and(B=p3^)
          then p0^:= Cislo
          else begin
            p0^:= FindColor(p1^,p2^,p3^,paleta);
            R:= p1^; G:=p2^; B:=p3^; Cislo:= p0^
           end
         end;
       p_l^:= +( integer(p1^) - paleta^[P0^,1]); Inc( p_l );
       p_l^:= +( integer(p2^) - paleta^[P0^,2]); Inc( p_l );
       p_l^:= +( integer(p3^) - paleta^[P0^,3]); {Inc( p_l );}
       Dec(p_l,2);

       Inc(p0); Inc(p1); Inc(p2); Inc(p3)
      end;
   Kolikaty:= not Kolikaty
  end;
 Dispose(HashTable);
 {asm xor ax,ax; int 16h end;}
 for cyklus:=15 downto 0 do begin
  for cyklus2:= 0 to 15 do begin
   Paleta^[16*cyklus+cyklus2,1]:=Paleta^[cyklus,1];
   Paleta^[16*cyklus+cyklus2,2]:=Paleta^[cyklus,2];
   Paleta^[16*cyklus+cyklus2,3]:=Paleta^[cyklus,3]
  end
 end;

 assign(f,'DIT2-OUT.PLT');
 rewrite(f,1);
 Blockwrite(f,Paleta^,768);
 close(F);

 for cyklus:=0 to 255 do begin
  cyklus2:=FindColor(PL2^[cyklus,1],PL2^[cyklus,1],PL2^[cyklus,3],Paleta);
  PL2^[cyklus,1]:=Paleta^[cyklus2,1];
  PL2^[cyklus,2]:=Paleta^[cyklus2,2];
  PL2^[cyklus,3]:=Paleta^[cyklus2,3];
 end;

 assign(f,'DITH-OUT.PLT');
 rewrite(f,1);
 Blockwrite(f,PL2^,768);
 close(F);

 closegraph;


 writeln('OK');
   dispose(POBrazek); dispose(R); dispose(G); dispose(B); dispose(Paleta );

end.
