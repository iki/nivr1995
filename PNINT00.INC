empty_handler proc far
        iret
empty_handler endp

division_handler proc far
        REG_ERROR merrdiv0
        GEN_ERROR err_exit+err_err
        iret
division_handler endp