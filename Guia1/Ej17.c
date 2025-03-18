#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//la mascara para pasar de min a may es 32 (decimal) y al reves es 223 (decimal)
int main(){
    char *s = "PALABRA", *mayus;
    printf("%s", s);
    int i, longitud = strlen(s);
    char maymin = 32, minmay = 223;
    mayus = (char*)malloc(longitud*sizeof(char));

    for (i = 0; i<longitud; i++){
        if (s[i] >= 97 && s[i] <= 122){
            mayus[i] = s[i] & minmay;
        }
        else
        if (s[i] >= 65 && s[i] <= 90){
            mayus[i] = s[i] | ' '; //muy importante recordar que el espacio es la mascara
            
        }
    }

    
    printf("%s", mayus);







    return 0;
}