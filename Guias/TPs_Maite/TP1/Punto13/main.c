#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int funcion0(int, int);
int funcion1(int, int);
int funcion2(int);
int funcion3(int);

int main(int argc, char *argv[])
{
    int A, B, func;
    int (*ptr_func[4])(int, int) = {funcion0, funcion1};
    int (*ptr2_func[4])(int) = {funcion2, funcion3};

    func = atoi(argv[1]);
    if (argc < 3) {
        if (func <=3) printf("Error alta parametro A\n");
        else printf("error operación invalida\n");
        return 1;
    } else {
        if ((*argv[1] == 0 || *argv[1] == 1) && argc <= 3) {
            printf("Para usar esas funciones es necesario ingresar valores de A y B\n");
            return 1;
        }
        else {
            A = atoi(argv[2]);
            if (func == 0 || func == 1) { // si requiere 2 args
                B = atoi(argv[3]);
                printf("resultado = %d\n",ptr_func[func](A, B));
            } else printf("resultado = %d\n",ptr2_func[func-2](A));
        }
    }


    return 0;
}

int funcion0(int A, int B) {
    return A+B;
}

int funcion1(int A, int B){
    return A&B;
}

int funcion2(int A){
    return A;
}

int funcion3(int A){
    return ~A;
}
