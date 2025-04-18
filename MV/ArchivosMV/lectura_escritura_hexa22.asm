    mov al, 0x08
    mov edx, ds
    add edx, 4
    mov ecx, 0x0202
    sys 0x1
    sys 0x2