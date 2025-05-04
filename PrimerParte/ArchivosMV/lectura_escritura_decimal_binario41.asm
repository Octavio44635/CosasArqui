    mov ax, 0x01
    mov edx, ds
    add edx, 4
    mov ecx, 0x0401
    sys 0x1
    mov ax, 0x11
    sys 0x2