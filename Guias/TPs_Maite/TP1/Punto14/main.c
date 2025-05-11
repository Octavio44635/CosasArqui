#include <stdio.h>
#include <stdlib.h>

int main()
{
    char semana = 0x7F; // 0000 1010
    char domingo = 0x01, lunes = 0x02, martes = 0x04, miercoles = 0x08, jueves = 0x10, viernes = 0x20, sabado = 0x40;

    printf("%x\n", semana);


    if ((semana&domingo) != 0) printf("domingo\n");
    if ((semana&lunes) != 0) printf("lunes\n");
    if ((semana&martes) != 0) printf("martes\n");
    if ((semana&miercoles) != 0) printf("miercoles\n");
    if ((semana&jueves) != 0) printf("jueves\n");
    if ((semana&viernes) != 0) printf("viernes\n");
    if ((semana&sabado) != 0) printf("sabado\n");

    return 0;
}
