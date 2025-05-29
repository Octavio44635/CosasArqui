text1 equ "Holaholaholahola"
valor equ 4

push valor
push valor
pop [ds+valor]
mov edx, ds
add edx,2
mov ecx, 8
sys 3
mov edx, ks
add edx,text1
sys 4
mov edx, ds
add edx,2
sys 4