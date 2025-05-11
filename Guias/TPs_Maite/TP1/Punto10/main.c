#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    unsigned int n;
    int number, i;
    n = 0x02A3;
    printf("n es %d (%x) \n", n, n);

    // Punto a
    n = 0x02A3;
    n = (n >> 8) & 0x00FF;

    printf("n es %x\n", n);

    // Punto b
    n = 0x02A3;
    n = n & 0x00FF;

    printf("n es %x\n", n);

    // Punto c
    n = 0x02A3;
    if ( n & 0x0001 == 1)
        printf("Impar\n");
    else printf("Par\n");

    // Punto d
    n = 0x02A3;
    if ((n & 0x8000 >> 15) == 1) printf("Negativo\n");
    else printf("Positivo\n");

    // Punto e
    n = 0x02A3;
    number = 0;
    n = n >> 4;
    for (i = 0; i < 12; i++) {
        if (n&0x0001 == 1) number += pow(2, i);
        n = n >> 1;
    }
    printf("Numero es %d\n", number);

    // Punto f
    n = 0x02A3;
    number = 0;
    for (i = 0; i < 4; i++) {
        if (n&0x0001 == 1) number += pow(2, i);
        n = n >> 1;
    }
    printf("Numero es %d\n", number);

    return 0;
}
