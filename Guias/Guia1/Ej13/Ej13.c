#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    int semana[7];
    char *diasemana[7] = {"lunes", "martes", "miercoles", "jueves", "viernes", "sabado", "domingo"};
    int i, m=sizeof(char)*8;
    char dias = 0x7f;

    for (i=m-1; i>0; i--){
        semana[i] = (dias & 0x1);
        //printf("%d \n",(dias & 0x1));
        dias = dias >> 1;
    }
    m = (sizeof(diasemana)/4)-1;
    for (i=m;i>=0;i--){
        if (semana[i]==1){
            printf("%s \n",diasemana[i]);
        }
    }

    return 0;
}
//Hay que tener cuidado con los tipos y especialmente con el tamaño de los ciclos for, estoy acostumbrado a python y fortran