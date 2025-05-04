#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    short int fecha = 0b0000011100110100; //0000 0111 0011 0100
    // 5 bits mas sig -> dia
    // 4 bits -> mes
    // 7 bits -> año (si >50 es 1900, si es menor es 2000)

    formatoISO(fecha);

    return 0;
}

void formatoISO(short int fecha) {
    int dia = 0, mes = 0, anio = 0;
    printf("la fecha es %x\n", fecha);
    // fecha
    for (int i = 0; i < 5; i++) {
        if (fecha&0x0001 == 1) dia += pow(2,i);
        fecha = fecha >> 1;
    }
    for (int i = 0; i < 4; i++) {
        if (fecha&0x0001 == 1) mes += pow(2,i);
        fecha = fecha >> 1;
    }
    for (int i = 0; i < 7; i++) {
        if (fecha&0x0001 == 1) anio += pow(2,i);
        fecha = fecha >> 1;
    }

    if (anio > 50) printf("19%d - %d - %d\n", anio, mes, dia);
    else {
        if (anio < 10) printf("200%d - %d - %d\n", anio, mes, dia);
        else printf("20%d - %d - %d\n", anio, mes, dia);
    }
}
