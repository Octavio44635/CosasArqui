#include <stdio.h>
#include <stdlib.h>

// me cago en los punteros
int main()
{
    char *pchar;
    int nro = 1;

    pchar = malloc(sizeof(char));
    pchar = 'K';

    printf("c = %c\n", pchar);

    weekday_set(pchar, nro);
    //weekday_reset(pchar, nro);

    printf("c = %x\n", pchar);

    return 0;
}

void weekday_set(char *pchar, int nro) {
    printf("nro es %d\n", nro);
    switch(nro) {
        case 0:
            *pchar = (*pchar)&0x00;
            break;
        case 1:
            printf("Lunes\n");
            *pchar = (*pchar)&0x01;
            printf("Llegue\n");
            break;
        case 2:
            *pchar = (*pchar)&0x02;
            break;
        case 3:
            *pchar = (*pchar)&0x04;
            break;
        case 4:
            *pchar = (*pchar)&0x08;
            break;
        case 5:
            *pchar = (*pchar)&0x10;
            break;
        case 6:
            *pchar = (*pchar)&0x20;
            break;
    }
    printf("c = %c\n", pchar);
}
