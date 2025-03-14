#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    char semana[7];
    const char *diasemana[7] = {"lunes", "martes", "miercoles", "jueves", "viernes", "sabado", "domingo"};
    int dias = 10, i, m=sizeof(char)*8;

    for (i=m-1; i>0; i--){
        semana[i] = (dias & 1);
        dias = dias >> 1;
    }
    i=0;
    for (i=0; i<m-1; i++){
        if (semana[i] == '1'){
            printf("%s\n", diasemana[i]);
        }
        else{
            printf("%d", i);
        }
    }
    







    return 0;
}