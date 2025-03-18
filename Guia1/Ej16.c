#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    //la fecha tendra este formato DDDD DMMM MYYY YYYY
    int dia, mes, anio;
    short int fecha = 0;

    dia = 17; // 5bits
    mes = 2; // 4bits
    anio = 3; //7bits

    fecha = (fecha | dia);
    fecha = fecha << 4; //4 se liberan 4 espacios para el mes
    fecha = (fecha | mes);
    fecha = fecha << 7; //7 se liberan 7 espacios para el anio
    fecha = (fecha | anio);
    printf("%d \n", fecha);
    
    print_binary(fecha);
    return 0;
}

void print_binary(short int val) {
    for (int i = 15; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
    }
}