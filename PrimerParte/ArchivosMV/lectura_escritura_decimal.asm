    mov al, 0x01
    mov edx, ds
    add edx, 4
    mov ecx, 0x0401
    sys 0x1
    sys 0x2