#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main()
{
    char cadena[100];
    strcpy(cadena, "123");
    printf("%d\n", convertirInt(cadena));
    return 0;
}

int convertirInt(char cadena[]) {
    int x, f = 0;
    x = strlen(cadena);
    for(int i = 0; i < x; i++) f += pow(10, x-i-1) * (cadena[i] & 0x0F);
    return f;
}
