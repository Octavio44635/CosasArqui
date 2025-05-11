#include <stdio.h>
#include <stdlib.h>

short int codificar(int dia, int mes, int anio);
int main()
{

    int dia = 20, mes = 9, anio = 3;
    printf("La fecha codificada es %x\n",codificar(dia, mes, anio));
    return 0;
}

short int codificar(int dia, int mes, int anio) {
    int indice[16] = {0}, i = 15;
    short int final = 0;
    // Consigo dia en bites
    while (dia >= 1) {
        indice[i] = dia % 2;
        dia /= 2;
        i--;
    }
    // Completo los bites de dia
    while (i > 10) {
        indice[i] = 0;
        i--;
    }
    // Consigo el mes en bites
    while (mes >= 1) {
        indice[i] = mes % 2;
        mes /= 2;
        i--;
    }
    // Completo los bites de mes
    while (i > 6) {
        indice[i] = 0;
        i--;
    }
    // Consigo los bites de anio
    while (anio >= 1) {
        indice[i] = anio % 2;
        anio /= 2;
        i--;
    }
    // Completo los bites de anio
    while (i >= 0) {
        indice[i] = 0;
        i--;
    }
    // Lo voy elevando a las potencias de 2
    for (int i = 15; i >= 0; i--) if (indice[i] == 1) final+= pow(2, 15-i);
    return final;
}
