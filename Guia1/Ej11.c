#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rellena(int, char []);
int main(){
    int nro = -15;
    char s[33];
    

    rellena(nro, s);
    
    return 0;

}

void rellena(int nro, char *s){
    //Basicamente lo que dice es pasar el numero a binario
    int i = 0, n=sizeof(nro)*8; //sizeof(nro) da el numero de BYTES

    for (i=n-1; i>0; i--){
        s[i] = (nro & 1);
        nro = nro >> 1;        
        
    }
    for (i=1; i<n; i++){
        printf("%d", s[i]);
    }
    
    
}

//El ejercicio 12 es tan simple como hacerlo negativo y ver que efectivamente usa complemento a 2