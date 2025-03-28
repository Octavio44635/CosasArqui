#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE *arch = fopen("sample.vmx", "rb");
    char lectura[100]; //Se crea el vector para la lectura, tiene que ser de char puesto que los char pesan 1 byte
    if( arch != NULL){
        fread(lectura, sizeof(char), 8, arch); //Se leen bytes de forma arbitraria, los primeros 7 son el header, siendo los ultimos 2 el tamaño 
        printf("Lectura: %0x %0x \n", lectura[6], lectura[7]);
        
        int tamCS = (lectura[6] << 8 )| lectura[7];
        printf("%d",tamCS);

    }
    else
        printf("Error al abrir el archivo\n");
    
    return 0;
}