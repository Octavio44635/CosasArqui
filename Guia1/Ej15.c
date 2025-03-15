#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//la fecha tendra este formato DDDD DMMM MYYY YYYY
int main(){
    short int fecha = 8500;
    int nba = 7, nbm = 4, nbd = 5, i;  //numero de bits de año, dia y mes
    char maska = pow(2,nba)-1, maskd = pow(2,nbd)-1, maskm = pow(2,nbm)-1;
    
    maska = maska & fecha;
    printf("%d\n", maska);
    

    for (i=0; i<nba; i++){
        fecha = fecha >> 1;
    }
    maskm = maskm & fecha;
    printf("%d\n", maskm);

    for (i=0; i<nbm; i++){
        fecha = fecha >> 1;
    }
    maskd = maskd & fecha;
    printf("%d\n", maskd);







    return 0;
}