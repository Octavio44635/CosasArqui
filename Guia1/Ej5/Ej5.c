#include <stdio.h>
#include <stdlib.h>
int negativo(int, int);
int main(){
    int x,n=4,y[4] = {0x1A6E , 0X3DE3, 0X9BCB, 0XDAD0},i;
    //Operaciones en hexadecimal
    //printf("%x \n", (0x25A5 | 0x007F));

    //printf("%04x \n", (0x24AF << 16) & 0xFFFF);

    //x = 0x0f00;
    //for (i=0;i<4;i++){
    //    printf("%04x \n", x | y[i]);
    //}
    x = 0xfA6e;
    n=4;
    if(negativo(x,n-1)){
        printf("Es negativo");
    }
    else{
        printf("No es negativo");
    }
    
}

int negativo(int x, int n){
    //deberia ver como hacer para que la cadena hexagecimal entera tome los negativos
    int m=4*n;
    x = x >> m;
    return x & 0xf;
}