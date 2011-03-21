KEYS            db      KEYS_SIZE/4 dup ('KEY$')
KEY_NP          db      0
KEY_LAST        db      0
KEY_LAST2       db      0
KEY_DISABLED    db      0
key_e0flag      db      0
key_e1flag      db      0

;KEY_FIRST       db      0
;KEY_BUF_HEAD    db      0
;KEY_BUF_TAIL    db      0
;KEYS_RF         db      256 dup (1)
;KEY_BUFFER      db      KEY_BUFFER_SIZE/4 dup ('KEY#')
;KEY_REPEAT      db      KEY_BUFFER_SIZE/4 dup ('KEY%')
;key2ascii       db      0,27,'1234567890-=',14,15
;                db      'qwertyuiop',0,0,13,0,'as'
;                db      'dfghjkl',0,0,'''',0,0,'zxcv'
;                db      'bnm',0,'./',0,0,0,' ',0,1,2,3,4,5
;                db      6,7,8,9,10,0,0,24,25,26,'-',21,22,23,0,18
;                db      19,20,16,17,0,0,0,11,12,0,0,0,0,0,0,0
;                db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;                db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;                db      0,27,'!',0,'#',0,'%',0,0,0,0,0,0,0,14,15
;                db      'QWERTYUIOP',0,0,13,0,'AS'
;                db      'DFGHJKL:',0,'"',0,0,'ZXCV'
;                db      'BNM',0,0,'?',0,0,0,' ',0,1,2,3,4,5
;                db      6,7,8,9,10,0,0,24,25,26,'-',21,22,23,0,18
;                db      19,20,16,17,0,0,0,11,12,0,0,0,0,0,0,0
;                db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;                db      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0