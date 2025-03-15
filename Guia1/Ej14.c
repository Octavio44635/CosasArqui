#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void weekday_reset(char*, int);
void weekday_set(char*, int);
int main(){
    char *semana;
    int dia=3;

    weekday_reset(semana, dia);
    printf("%d\n", *semana);
    weekday_set(semana, dia);
    



    return 0;
}
void weekday_reset(char *semana, int dia){
    *semana = *semana & 0x00;
}
void weekday_set(char *semana, int dia){
    *semana = *semana | dia;
    printf("%d\n", *semana);
    if (*semana & 3){
        printf("El dia 4 esta seteado\n");
    }
}