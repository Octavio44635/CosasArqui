#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    FILE *archo, *archd;
    char letra;
    int i = 0, max;

    if (argc < 4) printf("Error, faltan parametros\n");
    else {
        max = strlen(argv[1]);

        archo = fopen(argv[2], "rb");
        if (archo != NULL) {
            archd = fopen(argv[3], "w");
            if (archd != NULL) {
                fread(&letra, sizeof(char), 1, archo);
                while (!feof(archo)) {
                    letra = letra ^ argv[1][i];
                    if (i == max - 1) i = 0;
                    else i++;
                    fwrite(&letra, sizeof(char), 1, archd);
                    fread(&letra, sizeof(char), 1, archo);
                }
                fclose(archd);
            } else printf("Error con arcihvo de destino\n");
            fclose(archo);
        } else printf("Error con arcihvo de origen\n");

    }
    return 0;
}
