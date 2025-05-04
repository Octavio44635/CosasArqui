#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int nro;
    char* s;
    // 16 bits
    nro = 10;
    convertirB(nro, s);
    printf("s es %s\n", s);
    printf("%X = %s\n", nro, s);

    return 0;
}


void convertirB(int nro, char* s) {
    char aux[16];
    printf("nro es %x\n", nro);
    strcpy(s, "");
    for (int i = 0; i < 16; i++) {
        if((nro & 0x0001) == 1) strcpy(aux, "1");
        else strcpy(aux, "0");

        strcat(aux, s);
        strcpy(s, aux);
        nro = nro >> 1;
    }
}
