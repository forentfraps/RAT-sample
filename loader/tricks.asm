    global setTrapFlag
    global pushTime
    global appendByte2Rip
    global loadTime

    extern GetSystemTime

    section .data
    time_st db 16 dup (0)

    section .text
    setTrapFlag:
        pushfq
        or qword [rsp], 0x100
        popfq
        ret
    appendByte2Rip:
        pop r11
        add r11, 1
        jmp r11
    appendByteExit:
        add rsp, 16
        mov r11, [rsp - 8]
        add r11, 1
        jmp r11
; fld load on to the stack?
; fstp get back from the stack?
    pushTime:
        push rcx
        push rax
        mov rcx, time_st
        call appendByte2Rip
        db 0x9a
        call GetSystemTime
        mov rcx, time_st
        mov rax, [rcx + 12]
        mov [rsp - 8], rax
        fld qword [rsp - 8]
        pop rax
        pop rcx
        call appendByteExit

    loadTime:
        push rcx
        push rax
        mov rcx, time_st
        mov rax, [rcx + 12]
        call appendByte2Rip
        db 0x9a
        cmp rax, 0
        je _exit_loadTime
        call appendByte2Rip
        db 0x9a
        call GetSystemTime
        xor rax, rax
        mov rcx, time_st
        mov rax, [rcx + 12]
        fstp qword[rsp - 8]
        mov rcx, [rsp - 8]
        xor rax, rcx
        xor [rsp], rax
    _exit_loadTime:
        call pushTime
        db 0x7c
        pop rax
        pop rcx
        ; ret
        call appendByteExit