    global setTrapFlag
    global pushTime
    global appendByte2Rip
    global loadTime
    global appendByteExit
    global stepOverExit
    global callSecretWIN
    global ___chkstk_ms
    extern _GetSystemTime

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
        call callSecretWIN
        db 0x9a
        call _GetSystemTime
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
        call callSecretWIN
        db 0x9a
        call _GetSystemTime
        xor rax, rax
        mov rcx, time_st
        mov rax, [rcx + 12]
        fstp qword[rsp - 8]
        mov rcx, [rsp - 8]
        xor rax, rcx
        call appendByte2Rip
        db 0xe8
        lea rcx, [rsp + 0x16]
        xor [rcx], rax
    _exit_loadTime:
        call pushTime
        db 0x7c
        pop rax
        pop rcx
        ; ret
        call appendByteExit

;call at the end of the function
    stepOverExit:
        lea rcx, [rsp+8]
        mov rdx, [rcx]
        xor dl, 0xcc
        lea rdx, [rax - 17]
        xor rdx, rcx
        cmovz rcx, rdx
        ; xor [rcx], rdx
        ret


;<load args>
;call callSecret
;db 0x9a
;call _WIN_FUNC_
    callSecretWIN:
        pop r11
        xor rdi, rdi
        inc r11
        lea rax, [r11 + 1]
        mov rax, [rax]
        and rax, 0xffffff
        cmp rax, 0x800000
        jl cnt
        mov rdi, 0x1000000
    cnt:
        add r11, 5
        push r11
        lea r11, [r11 + rax]
        sub r11, rdi
        jmp [r11]

        mov rax, [r11]
        shl rax, 2 * 8
        shr rax, 4 * 8
        lea r11, [r11 + rax + 6]
        ; jmp [r11]
        mov r11, [r11]
        mov rax, [r11]
        shl rax, 2 * 8
        shr rax, 5 * 8
        lea r11, [r11 + rax + 7]
        mov r11, [r11]
        jmp [r11]

