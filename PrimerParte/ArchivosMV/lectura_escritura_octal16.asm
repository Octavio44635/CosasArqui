    mov al, 0x04
    mov edx, ds
    add edx, 4
    mov ecx, 0x0106
    sys 0x1
    sys 0x2