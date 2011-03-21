nikersig        db cr,lf,c@,c@,' ni operating system'
crlf            db cr,lf,c$
merr            db cr,lf,c?,' [error] ','::',' ',c$
merr2           db cr,lf,'          ','::',' ',c$
mwrn            db cr,lf,c?,' [warning] ','::',' ',c$
mwrn2           db cr,lf,'            ','::',' ',c$
mexit           db cr,lf,c@,' exiting..',cr,lf,c$
mcont           db 'press y to continue, any other key to terminate ',c$
mreq            db cr,lf,c@,' hardware required :',cr,lf
                db '    ',c_,' an 80486 in real mode (windoze or emm386 are not welcome)',cr,lf
                db '    ',c_,' all the memory above hma free - at least 0x1BC0 kb (8mb machine)',cr,lf
                db '    ',c_,' a 100% register compatible 256kb vga',cr,lf
                ;db '    ',c_,' a 512kb gravis ultrasound card [opt.]',cr,lf
                db '    ',c_,' a mouse with installed driver',cr,lf
                db '    ',c_,' a monitor (color one) is highly recommended (not for HMD owners :)',cr,lf
                db '    ',c_,' that',c%,'s all for today..',c$
mchcpu          db cr,lf,c@,' performing processor check : ',c$
mchcpu2         db cr,lf,  '                               ',c$
mchmem          db cr,lf,c@,' performing memory check : ',c$
mchmem2         db cr,lf,  '                            ',c$
mchvga          db cr,lf,c@,' performing vga check : ',c$
mchvga2         db cr,lf,  '                         ',c$
;mchgus          db cr,lf,c@,' performing gravis ultrasound check : ',c$
mchinput        db cr,lf,c@,' performing input device check : ',c$
;;mchcpu2         label byte
;;mchmem2         label byte
;;mchvga2         label byte
;mchinput2       db cr,lf, '                                  ',c$
mrfmenter       db cr,lf,c@,' entering real flat mode..',c$
minitmms        db cr,lf,c@,' init memory management system..',c$
minitdfs        db cr,lf,c@,' init file system..',c$
minitkbd        db cr,lf,c@,' startup keyboard handler..',c$
msavemouse      db cr,lf,c@,' saving mouse state..',c$
msavevga        db cr,lf,c@,' saving video state..',c$
mloadata        db cr,lf,c@,' loading data..',c$
mrestmouse      db cr,lf,c@,' restoring mouse state..',c$
mrestvga        db cr,lf,c@,' restoring video state..',c$
mrfmleave       db cr,lf,c@,' leaving real flat mode..',c$

mwrncrash       db 'your system may crash if you choose continue',c$
merrpm          db 'cannot run in protected mode',c$
merrcp          db 'cannot run on an 80386 or below',c$
merrext0        db 'post states memory size miscopare',c$
merrext0a       db 'using the value determined by post',c$
merrext1        db 'not enough extended memory',c$
merrext2        db 'get_ext_mem_size (int15h/fn88h) returned error',c$
merrext3        db 'there seems to be a TSR in extended memory below 8MB',c$
merrext4        db 'xms handler reports some emb(s) allocated',c$
merrext5        db 'there might be some emb(s) allocated trough xms',c$
merrvga0        db 'vga not detected',c$
merrvga1        db 'cannot set active display to vga',c$
merrvga2        db 'not a 256kb vga',c$
merrvga3        db 'mode 13h not supported by your vgabios (??)',c$
merrvga4        db 'mono vga/monitor not supported',c$
;merrvga5        db 'can',c%,'t enable irq 2',c$
merrmouse0      db 'mouse driver not found',c$
merrea20f       db 'enabling a20 failed',c$
;merrda20f       db 'disabling a20 failed',c$
merrmmsa0       db 'can',c%,'t allocate memory',c$
;merrmmsp0       db 'can',c%,'t allocate permanent memory',c$
merrmmsf0       db 'can',c%,'t deallocate memory',c$
merrdfs0        db 'unable to open ',DATAFILE0_CNAME,' for reading',c$
datafile0_name  label byte
merrdfs1        db DATAFILE0_CNAME,' is corrupted',c$
merrdfs2        db DATAFILE0_CNAME,0
                db 'is corrupted or unsupported datafile version',c$
merrdfs3        db DATAFILE0_CNAME,0
                db 'is corrupted or some parts are missing',c$
merrdfs4        db 'reading ',DATAFILE0_CNAME,' failed',c$
merrdfs5        db 'internal [out of fat-cache handles]',c$
merrdiv0        db 'division overflow',c$

mi86            db '8086/88',c$
mi286           db '80286',c$
mi386           db '80386',c$
mi486           db '80486',c$
mi586           db 'pentium or above',c$
mpm             db ' in protected mode',c$
mrm             db ' in real mode',c$
mcpufamily      db 'family ',c$
mcpumodel       db ', model ',c$
mcpustepping    db ', stepping ',c$

mhex0x2         db ', '
mhex0x          db '0x',c$
mkbabovehma     db ' kb above hma, ext_top=0x',c$
mnon            db 'none',c$
mkb             db ' kb',c$
mgatea20        db 'gate a20 ',c$
mnoxms          db 'xms handler not installed',c$
mxmsver         db 'xms version ',c$
mxmsdver        db ', driver revision ',c$
mxmsallocated   db ' kb allocated',c$
mxmslocked      db ' at 0x',c$

mvganotdet      db 'not a vga',c$
mvganotact      db 'vga not active',c$
mvgadet         db ' kb allocated',c$
mmonmonodet     db ', mono monitor detected',c$
mmoncolordet    db ', color monitor detected',c$

mnomouse        db 'no mouse',c$
mmouse          db '-button mouse',c$

mdis            db 'disabled',c$
mena            db 'enabled',c$
msuc            db 'succeeded',c$
mfai            db 'failed',c$
;mnotimpl        db 'not implemented yet',c$
;minvcom         db 'invalid command',c$
;mfnotsup        db 'function not supported',c$
