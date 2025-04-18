    mov al, 0x02
    mov edx, ds
    add edx, 4
    mov ecx, 0x0104
    sys 0x1
    sys 0x2