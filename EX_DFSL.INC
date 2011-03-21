		PREPARE_SINTAB
                OPTZER0  eax
   		OPTZER0  ecx
                mov      ebx,offset angle2xy
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,1
                call     dfs_read
                or        al,al
                jz       rdfok1
                GEN_ERROR err_err+err_exit1
rdfok1:

                OPTZER0  eax
	        OPTZER0	 ecx
                mov      ebx,offset hopzed
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,2
                call     dfs_read
                or        al,al
                jz       rdfok2
                GEN_ERROR err_err+err_exit1
rdfok2:

                OPTZER0  eax
                OPTZER0	 ecx
                mov      ebx,offset textury
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,3
                call     dfs_read
                or        al,al
                jz       rdfok3
                GEN_ERROR err_err+err_exit1
rdfok3:

                OPTZER0  eax
                OPTZER0	 ecx
                mov      ebx,offset vejir
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,4
                call     dfs_read
                or        al,al
                jz        rdfok4
                GEN_ERROR err_err+err_exit1
rdfok4:
                OPTZER0  eax
		OPTZER0	 ecx
                mov      ebx,offset PALETTE_0
                mov      di,fs:[save_dfsfat0_handle]
                mov      edx,5
                call     dfs_read
                or       al,al
                jz       rdfok5
                GEN_ERROR err_err+err_exit1
rdfok5:
