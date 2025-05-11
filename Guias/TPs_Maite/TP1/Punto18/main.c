#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // X & 0x20 -> mayuscula

    char cadena[99];

    strcpy(cadena, "hola");
    convertirmayusc(cadena);

    // Asume que esta todo en minuscula
    printf("%s\n", cadena);

    return 0;
}

void convertirmayusc(char cadena[]) {
    int x = strlen(cadena);
    for (int i = 0; i < x; i++) cadena[i] = cadena[i] & ~ ' ';
}
