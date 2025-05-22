#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CS 0
#define DS 1
#define ES 2
#define SS 3
#define KS 4
#define IP 5
#define SP 6
#define BP 7
#define CC 8
#define AC 9
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
//Cambiar las constantes en todo el codigo


#define Tamtot 16384 //16KiB
#define TamanioRegistros 16
#define TamanioTablaS 8


typedef struct{
   short int Base;
   short int Tamanio;
}TablaS;


typedef struct{
    char memoria[Tamtot];
    int registros[TamanioRegistros];
    TablaS TSeg[TamanioTablaS];} mv;

void lectura(mv*, char*[], int, int);
void lecturaVMI(mv*, int, char*);
void CargaRegistros(mv*, int[], int, int);
int sumaTamanio(int[]);
int creaPS(mv*, int, char*[]);
int buscaParametroTamanio(int, char*[]);
int buscaStringArgv(int, char*, char*[], int);
void leeRegistrosVMI(mv*);
void leeTablaVMI(mv*);
void cargaSS(mv*, int, int);
void LeeCS(mv*);
void leeOrdenCS(mv*);
void SACAREGISTRO(mv, int*, int*);
void INMEDIATO(mv, int*, int*);
void MEMORIA(mv, int*, int*);
void NULO(mv, int*, int*);

void BytesMEMORIA(mv, int*, int*);
void CAMBIACC(mv*, int);
void punteroReg(mv, int, int*);

void SYS(mv*, int, int, char);
void SYSR(mv*, int, char, char, short int);
void SYSW(mv*, int, char, char, short int);
void SYSTRINGR(mv*, int, short int, short int);
void SYSTRINGW(mv*, int, short int, short int);
void SYSF(mv*);
void EscrFormato(mv*, int, int, short int, char);
void leeFormato(mv, int*, int, char);

void MOV(mv*, int, int, char);
void ADD(mv*, int, int, char);
void SUB(mv*, int, int, char);
void MUL(mv*, int, int, char);
void DIV(mv*, int, int, char);
void JMP(mv*, int, int, char);
void JZ(mv*, int, int, char);
void JP(mv*, int, int, char);
void JN(mv*, int, int, char);
void JNZ(mv*, int, int, char);
void JNP(mv*, int, int, char);
void JNN(mv*, int, int, char);
void LDH(mv*, int, int, char);
void LDL(mv*, int, int, char);
void RND(mv*, int, int, char);
void AND(mv*, int, int, char);
void OR(mv*, int, int, char);
void XOR(mv*, int, int, char);
void STOP(mv*, int, int, char);
void SWAP(mv*, int, int, char);
void NOT(mv*, int, int, char);
void SHL(mv*, int, int, char);
void SHR(mv*, int, int, char);
void PUSH(mv*, int, int, char);
void POP(mv*, int, int, char);
void CALL(mv*, int, int, char);
void RET(mv*, int, int, char);
void CMP(mv*, int, int, char);

int getReg(mv, char, char);
void setReg(mv*, char, char, int);
int ValoropST(int, int, mv*);
void set(mv*, int, char, int);
int ObtenerTamanioCelda(mv, int);
void setMemoria(mv*, int, int);
void EMPTY(mv*, int, int, char);
void Disassembler(mv*, int);
char* EscrReg(mv, int);
char* EscrMem(mv, int);
char sectorMemoria(char);
int sumaTamaniosTabla(mv*);

int main(int argc, char *argv[]){
    //Las funciones del vector deben recibir los mismos parametros
    mv MV;
    int tamanio, ubicacion=0, TamPS=0;
    int posVMX =buscaStringArgv(1, ".vmx", argv, argc);

    int posVMI=buscaStringArgv(1, ".vmi", argv, argc);
    MV.registros[AC] = argv[posVMI];
    printf("AC: %s \n", MV.registros[AC]);

    tamanio = buscaParametroTamanio(argc, argv);
    if(tamanio<0)
        tamanio = Tamtot;

    TamPS = creaPS(&MV, argc, argv);

    if(posVMX){
        lectura (&MV, argv, tamanio, TamPS);

    }
    else if(posVMI && !posVMX) //Si esto da verdadero es por que no hay vmx
        lecturaVMI(&MV, tamanio, argv[posVMI]);

    int offsetEntry = MV.TSeg[(MV.registros[CS]>>16) & 0x0000FFFF].Base + MV.registros[IP] & 0x0000FFFF ; //Es para despues
    printf("offsetEntry: %04X \n", offsetEntry);

    if(TamPS == 0)
        cargaSS(&MV, 0, argc);
    //else
        //cargaSS(&MV,MV->memoria[]);

    LeeCS(&MV);

    if(buscaStringArgv(0, "-d",argv,argc))
        Disassembler(&MV, offsetEntry);
    //

    //
    return 0;
}


void lectura(mv* MV, char* argv[], int tamanio, int TamPS){

    FILE *arch = fopen(argv[1], "rb");
    //FILE *arch = fopen("C:/Users/Octii/Desktop/Facultad/3erAnio/ArquitecturadeComputadoras/CosasArqui/PrimerParte/ArchivosMV/ArchivosSample/sample6.vmx", "rb");
    if( arch != NULL){
        fread((*MV).memoria, sizeof(char), 8, arch); //Se leen bytes de forma arbitraria, los primeros 7 son el header, siendo los ultimos 2 el tamaño
        char* cabecera = "VMX25";
        int i=0;
        int iguales = 1;
        int tamDS=0, tamES=0, tamSS=0, tamKS=0, offsetEntry=0;
        while(i<5 && iguales){
            iguales = (cabecera[i] == (*MV).memoria[i]);
            i++;
        }
        if(!iguales ){
            printf("Error, cabecera no valida\n");
            fclose(arch);
            STOP(MV, 0, 0, 0); //Error
        }

        int tamCS = (((*MV).memoria[6]&0x00FF) << 8 )| ((*MV).memoria[7] & 0x00FF);
        //Vector de registros


        //Llamados segun la version
        int arregloTamanios[6] = {TamPS,tamKS, tamCS, tamDS, tamES, tamSS}; //Arreglo de tamanios de segmentos
        printf("version: %d \n", MV->memoria[5]);
        if((*MV).memoria[5] == 0x01){
            fread((*MV).memoria,sizeof(char),tamCS,arch);
            fclose(arch);
        }
        else if(MV->memoria[5] == 0x02){
            fread((*MV).memoria+8,sizeof(char),10,arch); //Lectura de la cabecera de Archivo VMX
            arregloTamanios[3] = (((*MV).memoria[8]&0x00FF) << 8 )| ((*MV).memoria[9] & 0x00FF); //DS
            arregloTamanios[4] = (((*MV).memoria[10]&0x00FF) << 8 )| ((*MV).memoria[11] & 0x00FF); //ES
            arregloTamanios[5] = (((*MV).memoria[12]&0x00FF) << 8 )| ((*MV).memoria[13] & 0x00FF); //SS
            arregloTamanios[1] = (((*MV).memoria[14]&0x00FF) << 8 )| ((*MV).memoria[15] & 0x00FF); //KS
            offsetEntry = ((*MV).memoria[16]&0x00FF) << 8 | ((*MV).memoria[17] & 0x00FF);

            arregloTamanios[0] = TamPS;
            arregloTamanios[2] = tamCS;


            for(int i=8; i<18; i++){
                printf("i: %d ; memoria: %02X \n",i, MV->memoria[i]);
            }
            printf("Tamanios: %d %d %d %d %d %d\n", arregloTamanios[0], arregloTamanios[1], arregloTamanios[2], arregloTamanios[3], arregloTamanios[4], arregloTamanios[5]);
            fread(MV->memoria + TamPS + tamKS,sizeof(char), tamCS, arch); //Lectura de la memoria
        }
        if(tamanio >= sumaTamanio(arregloTamanios))
            CargaRegistros(MV, arregloTamanios, offsetEntry, tamanio);
        else{
            printf("Error, tamanio de memoria insuficiente\n");
            fclose(arch);
            STOP(MV, 0, 0, 0); //Error
        }


    }
    else
        printf("Error al abrir el archivo\n");

}

void lecturaVMI(mv* MV, int tamanio, char* arch){
    FILE *archivo = fopen(arch, "rb");
    char* header = "VMI25";
    int i=0, iguales = 1;
    if(archivo != NULL){
        fread((*MV).memoria,sizeof(char),8,archivo); //Lectura header
        while(i<5 && iguales){
            iguales = (header[i] == (*MV).memoria[i]);
            i++;
        }
        if(!iguales){
            printf("Error, cabecera no valida\n");
            fclose(archivo);
            STOP(MV, 0, 0, 0); //Error
        }

        if((*MV).memoria[5] == '1'){
            tamanio = ((*MV).memoria[6]&0x00FF) << 8 | ((*MV).memoria[7] & 0x00FF);
        }
        fread(&MV->memoria+8,sizeof(char),64,archivo); //Lectura de la memoria para Registros
        leeRegistrosVMI(MV);

        fread(&MV->memoria + 8,sizeof(char), 32,archivo); //Lectura de la memoria para Tabla
        leeTablaVMI(MV);

        fread(&MV->memoria + 8,sizeof(char),tamanio,archivo); //Lectura de la memoria

        //Deberia estar todo correctamente cargado aca

        fclose(archivo);
    }
    else
        printf("Error al abrir el archivo\n");
}

void CargaRegistros(mv* MV, int ArregloTamanios[], int offsetEntry, int tamanio){
    int j=0, param,i,suma=0;

    (*MV).registros[CS] = -1;
    (*MV).registros[DS] = -1;
    (*MV).registros[ES] = -1;
    (*MV).registros[SS] = -1;
    (*MV).registros[KS] = -1;
    (*MV).registros[IP] = -1;
    (*MV).registros[SP] = -1;
    (*MV).registros[BP] = -1;
    (*MV).registros[CC] = 0;
    (*MV).registros[AC] = 0;
    (*MV).registros[EAX] = 0;
    (*MV).registros[EBX] = 0;
    (*MV).registros[ECX] = 0;
    (*MV).registros[EDX] = 0;
    (*MV).registros[EEX] = 0;
    (*MV).registros[EFX] = 0;

    for(i=0; i<TamanioTablaS; i++){
        if(i<6){
            if(ArregloTamanios[i] > 0){
                (*MV).TSeg[j].Base = suma;
                suma += ArregloTamanios[i];
                (*MV).TSeg[j].Tamanio = ArregloTamanios[i];
                j++;
            }
        }
        else{
            (*MV).TSeg[i].Base = -1;
            (*MV).TSeg[i].Tamanio = 0;
        }

    }
    i=(ArregloTamanios[0] > 0);
    if(ArregloTamanios[1]>0){
        (*MV).registros[KS] = ((i++ <<16)& 0xFFFF0000);

    }
    if(ArregloTamanios[CS+2] != 0)
        (*MV).registros[CS] = ((i++<<16) & 0xFFFF0000);
    if(ArregloTamanios[DS+2] != 0)
        (*MV).registros[DS] = ((i++<<16) & 0xFFFF0000);
    if(ArregloTamanios[ES+2] != 0)
        (*MV).registros[ES] = ((i++<<16) & 0xFFFF0000);
    if(ArregloTamanios[SS+2] != 0){
        (*MV).registros[SS] = ((i<<16) & 0xFFFF0000);
        (*MV).registros[SP] = ((i<<16) & 0xFFFF0000 | (*MV).TSeg[SS].Tamanio);
    }
    for(;j<TamanioTablaS; j++){
            (*MV).TSeg[j].Base = -1;
            (*MV).TSeg[j].Tamanio = 0;
        }

    (*MV).registros[IP]=(*MV).registros[CS] | offsetEntry; // Inicializacion IP

    printf("Registros: \n");
    for(i=0; i<7; i++){
        printf("Registro %d: %08X \n", i, (*MV).registros[i]);
    }

    printf("Tabla: \n");
    for(i=0; i<7; i++){
        printf("Indice %d: %04X %04X \n", i, (*MV).TSeg[i].Base, (*MV).TSeg[i].Tamanio);
    }
}

int sumaTamanio(int arreglo[]){
    int suma=0;
    for(int i=0; i<TamanioTablaS-2; i++){
        suma += arreglo[i];
    }
    return suma;
}

int creaPS(mv* MV, int argc, char *argv[]){
    int acum=0;
    int i=0;
    int j=0;
    while(i<argc && strcmp(argv[i],"-p") != 0){
        i++;
    }
    int nroPal = argc - i - 1;
    i = argc - nroPal;
    while(i<argc){
        for (j=0; j<strlen(argv[i]); j++){
            (*MV).memoria[acum+j] = argv[i][j];
        }
        acum += j;
        (*MV).memoria[acum] = '\0'; //Agrega el \0 al final
        acum++;
        i++;
    }
    //Hasta aca estan las palabras
    //asignacion de punteros
    int puntero = 0;
        for (i=argc - nroPal; i<argc; i++){
            for (j=0; j<4;j++){
                (*MV).memoria[acum+j] = (puntero >> ((3-j)*8)) & 0x00FF; //Guarda el puntero en la memoria
            }
            acum += 4;
            puntero += strlen(argv[i])+1;

        }
    return acum;

}

int buscaParametroTamanio(int argc, char* argv[]){
    int i=0,j=0;
    int tamanio=0;
    while(i<argc && *argv[i] != 'm'){
        i++;
    }
    //m=123
    if(i<argc){
        j=2;
        while(argv[i][j] != '\0'){ //Tiene que pasar el numero que esta despues del igual, deberia probar como funciona, la primer condicion no deberia ni existir
            tamanio += argv[i][j] - '0';
            j++;
            tamanio *= 10;
        }
        printf("Tamanio true %d\n", tamanio);
        return tamanio/10;
    }
    else
        return -1;
}

int buscaStringArgv(int archivo, char* str, char* argv[], int argc){
    int i=0;
    if(!archivo){ //No buscas archivo, buscas un string entero
        while(i<argc && strcmp(argv[i],str) != 0){
            i++;
        }
        return i<argc;
    }
    else{
        int len;
        char* extArchivo = malloc(5*sizeof(char));
        while(i<argc && strcmp(str, extArchivo)){
            len = strlen(argv[i]);

            if(len>4){
                extArchivo[0] = argv[i][len-4];
                extArchivo[1] = argv[i][len-3];
                extArchivo[2] = argv[i][len-2];
                extArchivo[3] = argv[i][len-1];
                extArchivo[4] = '\0';
                if(strcmp(extArchivo,str) != 0)
                    i++;
            }
            else
                i++;
        }
        free(extArchivo);
        return i*(i<argc); //Si no lo encuentra devuelve 0, si lo encuentra devuelve i
    }
}

void leeRegistrosVMI(mv* MV){
    int i=0, pos=8; //parte de la cabecera
    for(i=0; i<TamanioRegistros; i++){
        (*MV).registros[i] = ((*MV).memoria[pos] & 0x00FF) << 24;
        (*MV).registros[i] = (*MV).registros[i] | ((*MV).memoria[pos+1] & 0x00FF) << 16;
        (*MV).registros[i] = (*MV).registros[i] | ((*MV).memoria[pos+2] & 0x00FF) << 8;
        (*MV).registros[i] = (*MV).registros[i] | ((*MV).memoria[pos+3] & 0x00FF);
        pos += 4;
    }
    //Los registros deberian de estar cargados
}

void leeTablaVMI(mv* MV){
    int i=0, pos=8;
    for(;i<8; i++){
        MV->TSeg[i].Base = ((MV->memoria[pos] & 0x00FF)<< 8) | (MV->memoria[pos+1] & 0x00FF);
        MV->TSeg[i].Tamanio = ((MV->memoria[pos+2] & 0x00FF)<< 8) | (MV->memoria[pos+3] & 0x00FF);
        pos += 4;
    }
}

void cargaSS(mv* MV, int punt, int argc){


    PUSH(MV, 0, punt, 0b10000000); //o es -1 o es un puntero al primer puntero del PS
    PUSH(MV, 0, argc, 0b10000000);
    PUSH(MV, 0, -1, 0b10000000);

    printf("SS \n");
    int i=0;
    int posMem = MV->TSeg[SS].Base + MV->registros[SP] & 0x0000FFFF;
    for(i=0; i<3; i++){
        printf("posMem: %04X \n", posMem);
        for(int j=0; j<4; j++){
            printf("%02X \n ", MV->memoria[posMem+j]);
        }
        posMem += 4;
    }
    //Solucionado
}

void LeeCS (mv *MV){
    void (*Funciones[32])(mv*, int, int, char) = {SYS,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,PUSH,POP,CALL,RET,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,MEMORIA};


    char instruccion;
    int opA,opB,operacion,IPaux, cont=0;
    int indiceCS = (MV->registros[CS]>>16) & 0x0000FFFF;
    while ((*MV).registros[IP] < MV->TSeg[indiceCS].Tamanio){  //IP <= (*MV).TSeg[0].Tamanio
        //Eliminar todo esto si la implementacion es correcta
        leeOrdenCS(MV);
        /*instruccion = (*MV).memoria[IPaux]; //levanto el dato del CS apuntado por IP
        opB=(instruccion&0xC0)>>6;    //4 valores, 2 bits
        opA=(instruccion&0x30)>>4;
        operacion=(instruccion &0x1F);
        //En instruccion la receta de la orden
        //xx x_x_xxxx
        int BytesA=0, BytesB=0;

        //Deberias sacar los bits
        FuncOperandos[opB](*MV, &cont, &BytesB); //(MV, cont, bytesOp);
        //Deberias cortar el dato

        //En A lo mismo
        FuncOperandos[opA](*MV, &cont, &BytesA);


        cont++;
        IPaux= IPaux + cont;
        cont = 0;

        (*MV).registros[IP]=IPaux; //Graba en el IP la proxima instruccion a leer

        if(operacion >= 0x0 & operacion <= 0x8)//Solo un operando
            if(opB != 0)//Se usa B, no A. Ignoramos A
                Funciones[operacion](MV, 0, BytesB, instruccion); //Llama a la funcion correspondiente
            else{
                printf("Error, operando B nulo\n");
                STOP(MV, opA, opB, instruccion); //Error
            }
        else
            if(operacion >= 0x10 & operacion <= 0x1E)
                if(opA != 0 && opA != 2){
                    if(opB != 0)
                        Funciones[operacion](MV, BytesA, BytesB, instruccion); //Llama a la funcion correspondiente
                    else{
                        printf("Error, operando B nulo\n");
                        STOP(MV, opA, opB, instruccion); //Error
                    }
                }
                else{
                    printf("Error, operando A invalido\n");
                    STOP(MV, opA, opB, instruccion); //Error
                }
            else{
                printf("\n Fin de ejecucion");
                STOP(MV, opA, opB, instruccion); //Error
            }
        IPaux = (*MV).registros[IP]; //Graba en el IP la proxima instruccion a leer, importa al hacer jump
    */
    }
}

void leeOrdenCS(mv* MV){
    void (*Funciones[32])(mv*, int, int, char) = {SYS,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,PUSH,POP,CALL,RET,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,MEMORIA};

    char instruccion;
    int opA,opB,operacion, cont=0;


    
    instruccion = (*MV).memoria[MV->registros[IP]]; //levanto el dato del CS apuntado por IP
    opB=(instruccion&0xC0)>>6;    //4 valores, 2 bits
    opA=(instruccion&0x30)>>4;
    operacion=(instruccion &0x1F);
    //En instruccion la receta de la orden
    //xx x_x_xxxx
    int BytesA=0, BytesB=0;

    //Deberias sacar los bits
    FuncOperandos[opB](*MV, &cont, &BytesB); //(MV, cont, bytesOp);
    //Deberias cortar el dato

    //En A lo mismo
    FuncOperandos[opA](*MV, &cont, &BytesA);


    cont++;
    MV->registros[IP] += cont;
    cont = 0;

    if((operacion >= 0x0 & operacion <= 0x8) || (operacion >=0x0B && operacion <= 0x0D))//Solo un operando
        if(opB != 0)//Se usa B, no A. Ignoramos A
            Funciones[operacion](MV, 0, BytesB, instruccion); //Llama a la funcion correspondiente
        else{
            printf("Error, operando B nulo\n");
            STOP(MV, opA, opB, instruccion); //Error
        }
    else
        if(operacion >= 0x10 & operacion <= 0x1E)
            if(opA != 0 && opA != 2){
                if(opB != 0)
                    Funciones[operacion](MV, BytesA, BytesB, instruccion); //Llama a la funcion correspondiente
                else{
                    printf("Error, operando B nulo\n");
                    STOP(MV, opA, opB, instruccion); //Error
                }
            }
            else{
                printf("Error, operando A invalido\n");
                STOP(MV, opA, opB, instruccion); //Error
            }
        else{
            printf("\n Fin de ejecucion");
            STOP(MV, opA, opB, instruccion); //Error
        }
}

void SACAREGISTRO (mv MV,int *cont, int *bytesOp){
    *bytesOp=0;
    *cont += 1;
    *bytesOp = MV.memoria[MV.registros[IP] + *cont];

}

void INMEDIATO (mv MV,int *cont, int *op){
  short int inmediato = 0;
  inmediato = MV.memoria[MV.registros[IP]+*cont + 2] &0x00FF;
  inmediato = (MV.memoria[MV.registros[IP]+*cont + 1] << 8) | inmediato;
  *op = 0;
  *op = inmediato;
  *cont += 2; //Aumento el contador
}
/*
void MEMORIA (mv MV, int *cont, int *op){
    short int offset, basereg, inmediato;
    char codreg;
    int PosMem;

    //primero se lee un inmediato, dos bytes

    //Funcion INMEDIATO
    inmediato = MV.memoria[MV.registros[IP]+*cont + 2];
    inmediato = (MV.memoria[MV.registros[IP]+*cont + 1] << 8) | inmediato;
    *cont += 2;
    //
    *cont += 1; //Aumento el contador
    codreg= (MV.memoria[MV.registros[IP]+ *cont]>>4)&0x0F; //Codigo del registro en el vector
    char tamañoCelda = (MV.memoria[MV.registros[IP]+ *cont])&0x03;

    basereg = MV.registros[codreg] >> 16;
    offset= MV.registros[codreg] & 0x0000FFFF;

    //PosMem = MV.TSeg[basereg].Base + offset + inmediato;

    //*op = MV.memoria[PosMem];
    *op = 0;
    *op = MV.TSeg[basereg].Base + offset + inmediato;
    if(*op > Tamtot){
        printf("Error, direccion de memoria fuera de rango\n");
        STOP(&MV, *op, 0, 0); //Error
    }
}
    */

void NULO (mv MV, int *cont, int *op){
}

void EMPTY(mv* MV, int opA, int opB, char operacion){
    printf("Funcion invalida \n");
    STOP(MV, 0, 0, 0); //Error
}

/*
void setMemoria(mv* MV, int posMem, int valorB){
    for(int i=3; i>=0; i--){
        (*MV).memoria[posMem+i] = (valorB >> ((3-i)*8)) & 0x00FF;
    }
}
    */

void setReg(mv *MV, char posA, char sectorA, int valorB){
    switch(sectorA){
        case 0:
            (*MV).registros[posA] = valorB;
            break;
        case 1: //L
            (*MV).registros[posA] &= 0xFFFFFF00;
            (*MV).registros[posA] |= valorB & 0x000000FF ;
            break;
        case 2:
            (*MV).registros[posA] &= 0xFFFF00FF;
            (*MV).registros[posA] |= (valorB & 0x000000FF) << 8;
            break;
        case 3:
            (*MV).registros[posA] &= 0xFFFF0000;
            (*MV).registros[posA] |= valorB & 0x0000FFFF;
            break;
    }
}
/*
int ValoropST(int tipo, int op, mv* MV){ //Valor operando Segun Tipo
    char posReg = op>>4 & 0x0f;
    char sector = (op>>2) & 0x3;
    int valor=0,i;
    switch(tipo){
        case 1:
            return getReg(*MV, posReg, sector); //(mv, direccion en mv.registro, sector)
            break;
        case 2:
            return op;
            break;
        case 3:
            for (i=0; i<4; i++){
                valor = (valor << 8) | (*MV).memoria[op+i] & 0x00FF;
            }
            return valor;
            break;
    }
}
    */

int getReg(mv MV, char posReg, char sector){
    int valor=0;

    switch (sector){
        case 0:
            valor = MV.registros[posReg];
            break;
        case 1:
            valor = MV.registros[posReg] & 0x000000FF;
            valor = valor << 24;
            valor = valor >> 24;
            break;
        case 2:
            valor = MV.registros[posReg] & 0x0000FF00;
            valor = valor << 16;
            valor = valor >> 24;
            break;
        case 3:
            valor = MV.registros[posReg] & 0x0000FFFF;
            valor = valor << 16;
            valor = valor >> 16;
            break;
    }
    return valor;
}

void set(mv* MV, int opA, char tipoA, int valorB){
    if(tipoA == 1){
        setReg(MV, opA>>4 & 0x0F, (opA>>2) & 0x03, valorB); //(MV, direccion en mv.registro, sector, valorB)
    }
    else
        setMemoria(MV, opA, valorB); //(MV, direccion en memoria, valorB)
}
//Funciones de la maquina virtual
    void MOV (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        set(MV, opA, tipoA,ValoropST(tipoB, opB, MV));
    }

    void ADD(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor = ValoropST(tipoB, opB, MV) + ValoropST(tipoA, opA, MV);
        set(MV, opA, tipoA, valor);
        CAMBIACC(MV,valor);
    }

    void SUB(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor = ValoropST(tipoA, opA, MV) - ValoropST(tipoB, opB, MV);
        set(MV, opA, tipoA, valor);
        CAMBIACC(MV,valor);
    }

    void MUL(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor = ValoropST(tipoA, opA, MV)*ValoropST(tipoB, opB, MV);
        set(MV, opA, tipoA, valor);
        CAMBIACC(MV,valor);
    }

    void DIV(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valorB = ValoropST(tipoB, opB, MV), valorA = ValoropST(tipoA, opA, MV);
        if(valorB != 0){
            set(MV, opA, tipoA, valorA/valorB);
            CAMBIACC(MV,valorA/valorB);
            (*MV).registros[AC] = valorA%valorB; //Resto
        }
        else{
            printf("Error de division por 0\n");
            STOP(MV, 0, 0, 0); //Error division por 0
        }
    }

    void JMP (mv* MV, int opA, int opB, char operacion){
        (*MV).registros[IP] = opB;
    }

    void JZ (mv* MV, int opA, int opB, char operacion){
        if ((*MV).registros[CC] == 0x40000000 ){ //CC
            (*MV).registros[IP] = opB; //IP
        }
    }

    void JP(mv* MV, int opA, int opB, char operacion){
        if (((*MV).registros[CC] == 0)) {
            (*MV).registros[IP] = opB; // IP
        }
    }

    void JN (mv* MV, int opA, int opB, char operacion){
        if (((*MV).registros[CC]) == 0x80000000){ //CC
            (*MV).registros[IP] = opB; //IP
        }
    }

    void JNZ (mv* MV, int opA, int opB, char operacion){
        if ((*MV).registros[CC] != 0x40000000){ //CC
            (*MV).registros[IP] = opB; //IP
        }
    }

    void JNP (mv* MV, int opA, int opB, char operacion){
        if ((*MV).registros[CC] != 0){ //CC
            (*MV).registros[IP] = opB; //IP
        }
    }

    void JNN (mv* MV, int opA, int opB, char operacion){
        if (((*MV).registros[CC]) != 0x80000000){ //CC
            (*MV).registros[IP] = opB; //IP
        }
    }

    void LDH (mv* MV, int opA, int opB, char operacion){
        int valorA = ValoropST((operacion>>4)&0x3, opA, MV);
        int valorB = ValoropST((operacion>>6)&0x3, opB, MV);
        //los 2 bytes mas sign del primero con los 2 bytes menos sign del segundo
        valorA=valorA & 0x0000FFFF; //quedaria 11111111 11111111 000000....
        valorA=valorA | ((valorB & 0x0000FFFF) << 16);

        set(MV, opA, (operacion>>4)&0x3 ,valorA);
    }

    void LDL (mv* MV, int opA, int opB, char operacion){
        int valorA = ValoropST((operacion>>4)&0x3, opA, MV);
        int valorB = ValoropST((operacion>>6)&0x3, opB, MV);
        //los 2 menos sign del primero con los dos menos sign del segundo
        valorA=valorA & 0xFFFF0000;
        valorA= valorA | (opB & 0x0000FFFF);

        set(MV, opA, (operacion>>4)&0x3 ,valorA);
    }

    void RND (mv* MV, int opA, int opB, char operacion){
        set(MV, opA, (operacion>>4)&0x3 ,rand() % (opB + 1));
    }

    void AND (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoA, opA, MV) & ValoropST(tipoB, opB, MV);
        set(MV, opA, (operacion>>4)&0x3 ,valor);

        CAMBIACC(MV,valor);
    }

    void OR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoA, opA, MV) | ValoropST(tipoB, opB, MV);
        set(MV, opA, (operacion>>4)&0x3 ,valor);
        CAMBIACC(MV,valor);
    }

    void XOR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoA, opA, MV) ^ ValoropST(tipoB, opB, MV);
        set(MV, opA, (operacion>>4)&0x3 ,valor);
        CAMBIACC(MV,valor);
    }

    void STOP(mv* MV, int opA, int opB, char operacion){
        //STOP
        (*MV).registros[IP] = (*MV).TSeg[0].Base + (*MV).TSeg[0].Tamanio; //IP
    }

    void SWAP (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int aux;
        if ((tipoA==3 && tipoB==3) || (tipoA==3 && tipoB==2) || (tipoA==2 && tipoB==2) || (tipoA==2 && tipoB==3)){
        aux=ValoropST(tipoA, opA, MV);
        set(MV, opA, tipoA, ValoropST(tipoB, opB, MV));
        set(MV, opB, tipoB, aux);
        }

    }

    void NOT (mv* MV, int opA, int opB, char operacion){
        char tipoB=(operacion >> 6) & 0x3;
        set(MV, opB, tipoB, ~ValoropST(tipoB, opB, MV));
        CAMBIACC(MV,opA);
    }

    void SHL (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        int valorB = ValoropST((operacion>>6) & 0x3, opB, MV);
        int aux = ValoropST(tipoA, opA, MV)<< valorB;

        set(MV, opA, tipoA, aux);
        CAMBIACC(MV,aux);
    }

    void SHR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        int mascara, valorB = ValoropST((operacion>>6) & 0x3, opB, MV);
        int valorA = ValoropST(tipoA, opA, MV) >> valorB;
        /*
        if (opA & 0x80000000){
            mascara = ((0xFFFFFFFF) << (32 - valorB)) & 0xFFFFFFFF;
            valorB|= mascara;
        }
        */

        int base = ceil(log(valorB)/log(2));
        valorA = valorA << (32-base);
        valorA = valorA >> (32-base+valorB);
        set(MV, opA, tipoA, valorA);
        CAMBIACC(MV,valorA);
    }

    void PUSH (mv* MV, int opA, int opB, char operacion){
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoB,opB,MV);
        int indice;

        (*MV).registros[SP]-=4;
        indice=((*MV).registros[SP] >>16) &0x0000FFFF; //Entrada segmento SS

        if ((MV->registros[SP]&0x0000FFFF) <= 0){
            printf("Error: Stack overflow\n");
            STOP(MV, 0, 0, 0);
        }
        int punteroSP=0;
        punteroReg(*MV, SP, &punteroSP);
        setMemoria(MV,punteroSP<<2, valor);
    }

    void POP (mv* MV, int opA, int opB, char operacion){
        char tipoB=(operacion>>6) & 0x3;
        int  indice= (*MV).registros[SS]>>16; // Entrada Tabla Segmentos
        int  base= (*MV).TSeg[indice].Base;
        int  limite= base + (*MV).TSeg[indice].Tamanio;
        int  valor;

        if ((*MV).registros[SP]> limite){
            printf("Error: Stack underflow\n");
            STOP(MV, 0, 0, 0);
        }

        valor = ValoropST(3, (*MV).registros[SP], MV); //Extrae valor de la memoria

        set(MV, opB, tipoB, valor);  // Escribe valor en el destino.
        (*MV).registros[SP] +=4;  //Incremento SP
    }

    void CALL (mv* MV, int opA, int opB, char operacion){
        char tipoB= (operacion>>6) & 0x3;
        int Dirdestino= ValoropST(tipoB,opB,MV); //Direccion a la que salta

        //IP apunta ya a la direccion a la siguiente direccion.

         PUSH (MV,0,(*MV).registros[IP], 0x80); //preguntar eso del 0x8 q no lo entiendo.

         //Salto
        JMP(MV, 0, Dirdestino, 0);
    }

    void RET(mv* MV, int opA, int opB, char operacion){

        POP(MV, 0, 0x50, 0x40);

    //Hacemos POP IP  ---->  IP= registro 5, y el sector es 0 --> opB= 0x50 ----> 0101 0000
    //operacion= 0x40 ----> tipoB=1 (Registro) --> 0100 0000
    // se hace esto ya que en POP, se calcula el tipo y el sector cuando se llama a set
    }

    void CMP (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valorA=ValoropST(tipoA, opA, MV);
        int valorB=ValoropST(tipoB, opB, MV);
        CAMBIACC(MV, valorA-valorB);
    }



////////
void punteroReg(mv MV, int dirReg, int* op){

    short int basereg = (MV.registros[dirReg] >> 16) & 0x0000FFFF;
    short int offset= MV.registros[dirReg] & 0x0000FFFF;

    *op = 0;
    *op = MV.TSeg[basereg].Base + offset;

}

void SYS (mv* MV, int opA, int opB, char operacion){
    //SYS
    int valorB = ValoropST((operacion >> 6) & 0x03,opB , MV); //Se almacena el valor de B
    int dirReg = EDX; //Esta variable esta hecha por que no puedo mandar una constante como parametro
    int puntEdx;    //Se almacena el puntero a EDX, puesto que es la porcion de memoria en la que se escribira

    int CX = getReg(*MV, 12, 3);
    char CH = (CX & 0X0000FF00) >> 8, CL = CX & 0xFF; //Esto se hace para no buscarlos repetidas veces en los ciclos
    //Almacenan el tamaño (CH) y numero de celdas (CL)

    short int AL = getReg(*MV, 10, 3);  //Aca se almacena el formato

    punteroReg(*MV, dirReg, &puntEdx);    //Se extrae la direccion de memoria a la que apunta edx
    //Puede ocurrir que la direccion de EDX sea negativa, entiendo hay que quitarle el signo

    //Hasta aca todo claro

    printf("SYS: %d\n", valorB);
    if (valorB == 1)
        SYSR(MV, puntEdx, CH, CL, AL);
    else
        if(valorB == 2)
            SYSW(MV, puntEdx, CH, CL, AL);
        else{
            if(valorB == 3){
                SYSTRINGR(MV, puntEdx, ((CH<<8) & 0xFF00) | CL ,0); //AL no nos sirve por que el formato es String o char en su defecto

            }
            else if(valorB == 4){
                SYSTRINGW(MV, puntEdx, ((CH<<8) & 0xFF00) | CL, 0);
            }
            else if(valorB == 7){
                system("clear");
            }
            else if(valorB == 0x0F){
                SYSF(MV);
            }
            else{
                printf("Error: syscall no implementada\n");
                STOP(MV, 0, 0, 0); //Error
            }
        }
}

void SYSR(mv* MV, int punt, char CH, char CL, short int AL){
    int lectura;
    for (int i=0; i<CL; i++){
        leeFormato(*MV, &lectura, AL, CH); //Lee el valor
        for (int j=0; j<CH; j++){
            (*MV).memoria[punt + CH-(j+1) + CH*i] = lectura & 0x000000FF;
            lectura = lectura >> 8;
        }//va a guardar los CH bytes menos significativos
    }
}

void leeFormato(mv MV, int *lectura, int AL, char CH){
    char * cadenaBin ;
    int valor;
    char * Cadena = malloc(sizeof(char)*CH);
    switch (AL)
    {
    case 0x01://decimal, se lee y se guarda en CH bytes
        scanf("%d", lectura);
        break;
    case 0x02: //caracteres, se leen CH char
        scanf("%s", Cadena);
        *lectura = Cadena[0];
        *lectura = *lectura & 0x000000FF;
        break;
    case 0x04: //Octal
        scanf("%o", lectura);
        break;
    case 0x08: //Hexadecimal
        scanf("%x", lectura);
        break;
    case 0x10:
        cadenaBin = malloc(CH*8);
        scanf("%s", cadenaBin);
        valor = 0;
        int maL;

        if (strlen(cadenaBin) > CH*8)
            maL = CH*8; //En esta situacion los bits menos significativos seran cortados, ideal seria que se introduzca un numero correcto de bit
        else
            maL = strlen(cadenaBin);

        maL--; //Le "saca" el caracter nulo

        for (int i=maL; i>0; i--){
            if(cadenaBin[i] == '0' || cadenaBin[i] == '1')
                valor = valor | ((cadenaBin[i]-'0')<<(maL-i));
        }
        if(cadenaBin[0] == '1'){ //negativo
            valor = ~valor + 1;
        }

        free(cadenaBin);
        *lectura = valor;
        break;
    default:
        printf("Error, formato no valido\n");
        STOP(&MV, 0, 0, 0); //Error
    }

}

void SYSW(mv* MV, int punt, char CH, char CL, short int AL){
    for(int i=0;i<CL; i++){
        EscrFormato(MV, i,punt, AL, CH);
        printf("\n");
    }

}

void EscrFormato(mv* MV, int i,int punt, short int AL, char CH){
    int escritura = 0;
    char* cadenaBits = malloc(CH*8 + 1);
    int stop = 1;


    printf("[%04X]      ", punt + i*CH);
    if((AL & 0x01) == 1){
        for(int j=0; j<CH; j++){
            escritura |= ((*MV).memoria[punt + i*CH + j]&0x00FF) << ((CH-1-j)*8);
        }
        printf("  %d  ", escritura);
        stop = 0;
    }
    if ((AL & 0x02) == 2){
        for(int j=0; j<CH; j++){
            char carAux = (*MV).memoria[punt + i*CH + j];
            if(carAux < 32 || carAux == 127){
                printf("  .  ");
            }
            else{
                printf("  %c  ", (*MV).memoria[punt + i*CH + j]);
            }
        }
        stop = 0;
    }
    if((AL & 0x04) == 4){
        escritura = 0;
        printf("  0o");
        for(int j=0; j<CH; j++){
            escritura = escritura | ((*MV).memoria[punt + i*CH + j] & 0x00FF) << ((CH-1-j)*8);
        }
        printf("%0o  ", escritura );
        stop = 0;
    }
    if((AL & 0x08) == 8){
        escritura = 0;
        printf("  0x");
        for(int j=0; j<CH; j++){
            escritura = escritura | ((*MV).memoria[punt + i*CH + j] & 0x00FF) << ((CH-1-j)*8);
        }
        printf("%08X  ", escritura );
        stop = 0;
    }
    if((AL & 0x10) == 16){
        escritura = 0;
        char bit;
        int j;
        for (int j=0; j<CH*8; j++){
            cadenaBits[j] = '0';
        }
        cadenaBits[CH*8] = '\0';

        for(j=0; j<CH; j++){
            escritura |= (*MV).memoria[punt + i*CH + j]&0x00FF << ((CH-1-j)*8);
        }
        j=0;
        while(abs(escritura)>1){
            bit = abs(escritura) % 2;
            bit = bit+'0';
            cadenaBits[strlen(cadenaBits)-1-j] = bit;
            escritura = escritura >> 1;
            j++;
        }
        cadenaBits[strlen(cadenaBits)-1-j] = abs(escritura) + '0';
        if(escritura<0){
            for(;j<strlen(cadenaBits);j++){
                cadenaBits[strlen(cadenaBits)-1-j] = '1';
            }
        }
        printf("  0b%s  ", cadenaBits);
        stop = 0;
    }

    if(stop){
        printf("Error, formato no valido\n");
        STOP(MV, 0, 0, 0); //Error
    }
}

void SYSTRINGR(mv* MV, int punt, short int CX, short int AL){
    char *string;

    printf("Escribir cadena de %d largo: \n" ,CX);
    scanf("%s", string);

    if (strlen(string) <= CX){
        CX = strlen(string);
    }
    int i=0;
    for (;i<CX;i++){
        MV->memoria[punt+i] = string[i];
    }
    MV->memoria[punt+i+1] = '\0';

//Entiendo esta bien
}
//Se hicieron cambios en estas funciones, revisar
void SYSTRINGW(mv* MV, int punt, short int CX, short int AL){
    char *string;
    for (int i=0; i<CX; i++){
        printf("%c", (*MV).memoria[punt+i]);
    }
    printf("\n");
}

void SYSF(mv* MV){
    char* ext = ".vmi";
    //int Vmi = ;
    printf("Archivo: %s\n", MV->registros[AC]);
    FILE *archivo = fopen(MV->registros[AC], "wb");
    char orden='\n';
    if(orden == '\n'){
        if(archivo){
            printf("Archivo abierto\n");
            fwrite(MV->memoria, sizeof(char), 8, archivo);
            for (int i=0; i<16; i++)
                fwrite(&MV->registros[i], sizeof(char), 4, archivo);
            for (int i=0; i<8; i++)
                fwrite(&MV->TSeg[i].Base + MV->TSeg[i].Tamanio, sizeof(char), 8, archivo);
            fwrite(&MV->memoria, sizeof(char), sumaTamaniosTabla(MV), archivo);
            fclose(archivo);
        }
        else{
            printf("Error, no se pudo abrir el archivo .vmi \n");
        }
        printf("Inserte orden para continuar: g, q o enter\n");

        //scanf("%c", orden);
        orden = 'g';
        if(orden == 'g'){
            //no hace nada
        }
        else if(orden == 'q'){
            printf("Programa finalizado\n");
            STOP(MV, 0, 0, 0);

        }else if(orden == '\n')
            leeOrdenCS(MV);
        else
            printf("caracter invalido \n");
    }
    else{
        printf("Error, no se pudo abrir el archivo .vmi \n");
        STOP(MV, 0, 0, 0); //Error
    }
}

int sumaTamaniosTabla(mv* MV){
    int suma=0;
    for (int i=0; i<TamanioTablaS; i++){
        suma += (*MV).TSeg[i].Tamanio;
    }
    return suma;
}
///////

void CAMBIACC (mv* MV, int valor){
    (*MV).registros[CC] &= 0x00000000; //Apaga los bits de estado
    if (valor < 0){
        (*MV).registros[CC] |= 0x80000000; // Si el If da 1, es negativo, Activo N (bit 15)
        //(*MV).registros[8] &= ~0x40000000; // Apaga bit Z (bit 14)
    }
    else{
        if(valor==0){
            (*MV).registros[CC] |= 0x40000000; // Si el If da 1, es cero, Activo Z (bit 14)
            //(*MV).registros[8] &= ~0x80000000;// Apaga bit N (bit 15)
        }
        else{
            (*MV).registros[CC] = 0;
        }
    }
}

void BytesMEMORIA(mv MV, int *cont, int *operando){
    int todo=0;
    todo = MV.memoria[MV.registros[IP]+*cont + 3] & 0xFF;
    todo = (MV.memoria[MV.registros[IP]+*cont + 2] << 8) | todo;
    todo = (MV.memoria[MV.registros[IP]+*cont + 1] << 16) | todo;


    *cont += 3;
    *operando = todo;
}

void Disassembler(mv* MV, int offset){
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,BytesMEMORIA};
    int cont=0; //Este contador es para saber cuantos bytes se leyeron
    (*MV).registros[IP]=(*MV).TSeg[0].Base; //IP = CS

    char* nomFun[32] = {"SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","NOT","","","PUSH","POP","CALL","RET","STOP","MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","LDL","LDH","RND"};

    int indiceCS = (MV->registros[CS] >> 16) & 0x0000FFFF; //Entrada Tabla Segmentos
    while((*MV).registros[IP] < (*MV).TSeg[indiceCS].Base + (*MV).TSeg[indiceCS].Tamanio){
        char instruccion = (*MV).memoria[MV->TSeg[indiceCS].Base + (*MV).registros[IP]];
        //Ya se mostro la orden, ahora se procede a mostrar los operandos
        char tipoA = (instruccion >> 4) & 0x3, operacion = instruccion & 0x1F,tipoB = (instruccion >> 6) & 0x3;
        int operandoA, operandoB, i;
        instruccion = instruccion & 0x00FF;
        //El tamaño y el tipo son iguales

        if(MV->registros[IP] == offset)
            printf(">");
        FuncOperandos[tipoB](*MV, &cont, &operandoB);
        FuncOperandos[tipoA](*MV, &cont, &operandoA);

        printf("[%04X] %02X ", (*MV).registros[IP], instruccion&0x00FF);

        (*MV).registros[IP] += cont+1;
        cont = 0; //Reinicio el contador
        //Aumento el IP

        if(tipoA == 1)
            operandoA &= 0X00FF;
        else
            if(tipoA == 2)
                operandoA &= 0x0000FFFF;
            else
                if(tipoA == 3)
                    operandoA &= 0x00FFFFFF;
                else
                    operandoA = 0;
        if(tipoB == 1)
            operandoB &= 0X00FF;
        else
            if(tipoB == 2)
                operandoB &= 0x0000FFFF;
            else
                if(tipoB == 3)
                    operandoB &= 0x00FFFFFF;
                else
                    operandoB = 0;

        //primer parte del formato
        if(tipoB != 0){
            unsigned char b;
            for(i=tipoB; i>0; i--){
                b = (operandoB >> ((i-1)*8)) & 0x00FF;
                printf("%02X ", b);
            }
        }
        if(tipoA != 0){
            unsigned char a;

            for(i=tipoA; i>0; i--){
                a = (operandoA >> ((i-1)*8)) & 0x00FF;
                printf("%02X ", a);
            }

        }
        for(i=tipoB + tipoA; i<7; i++){
            printf("   ");
        }
        printf("|");


        //
        printf(" %s" , nomFun[instruccion & 0x1F]);
        if(tipoA == 2)
            printf(" %d, ", operandoA);
            else
            if(tipoA == 1)
                printf(" %s, ", EscrReg(*MV, operandoA));
            else
                if(tipoA == 3)
                    printf(" %c%s, ",sectorMemoria(operandoA & 0x03), EscrMem(*MV, operandoA));
                else
                    printf("");
        if(tipoB == 2){
            short int valor;
            if (operandoB & 0x8000){
                valor =  (~operandoB) + 1;
                printf(" -%d ", valor);
            }
            else{
                valor = operandoB;
                printf(" %d ", valor);
            }

        }
            else
            if(tipoB == 1)
                printf (" %s ", EscrReg(*MV, operandoB));
            else
                if(tipoB == 3)
                    printf (" %c%s ",sectorMemoria(operandoB & 0x03), EscrMem(*MV, operandoB));
                else
                    printf("");
        printf("\n");

    }
}

char* EscrReg(mv MV, int operando){
    char* nombresRegistros[16] = {"CS", "DS", "ES", "SS", "KS", "IP", "SP", "BP", "CC", "AC", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX"};
    static char palabra[16] = "";
    char sector = (operando >> 2) & 0x3, registro = (operando >> 4) & 0x0F;
    char aux[3] = {'\0', '\0', '\0'};
    switch(sector){
        case 0:
            strcpy(palabra,nombresRegistros[registro]);
            break;
        case 1:
            strcpy(palabra,nombresRegistros[registro]);
            aux[0] = palabra[1];
            aux[1] = 'L';
            strcpy(palabra, aux);
            break;
        case 2:
            strcpy(palabra,nombresRegistros[registro]);
            aux[0] = palabra[1];
            aux[1] = 'H';
            strcpy(palabra, aux);
            break;
        case 3:
            strcpy(palabra,nombresRegistros[registro]);
            aux[0] = palabra[1];
            aux[1] = palabra[2];
            strcpy(palabra, aux);
            break;
    }
    return palabra;
}

char* EscrMem(mv MV, int operando){

    static char palabra[16] = "";
    char  registro=0;
    short int inmediato=0;

    registro = operando & 0x000000FF;
    inmediato = (operando >> 8);
    if(inmediato > 0){
        snprintf(palabra, sizeof(palabra), "[%s + %d]", EscrReg(MV, registro), inmediato);
        inmediato &= 0x0000FFFF;
    }
    else if(inmediato < 0){
        inmediato = (~inmediato) + 1;
        snprintf(palabra, sizeof(palabra), "[%s - %d]", EscrReg(MV, registro), inmediato);
    }
    else
        snprintf(palabra, sizeof(palabra), "[%s]", EscrReg(MV, registro));
    return palabra;
}

char sectorMemoria(char sector){
    switch(sector){
        case 0:
            return ' '; //Muestro l o ''?
            break;
        case 1:
            return 'b';
            break;
        case 2:
            return 'w';
            break;
        case 3:
            return '#'; //Por motivos de seguridad esto va a estar aca hasta el dia de la entrega
            break;
    }
}
//Codigos de Luciano, por lo visto estan bien pero hay que hacer pruebas

void MEMORIA(mv MV, int *cont, int *op){
    short int offset, basereg, inmediato;
    char codreg;
    int tamcelda, PosMem, TercerByte;

    //primero se lee un inmediato, dos bytes

    //Funcion INMEDIATO
    inmediato = MV.memoria[MV.registros[IP] + *cont + 2] & 0x00FF;
    inmediato = (MV.memoria[MV.registros[IP] + *cont + 1] << 8) | inmediato;
    *cont += 2;
    //
    *cont += 1; //Aumento el contador
    TercerByte = MV.memoria[MV.registros[IP] + *cont];

    codreg = (TercerByte >> 4) & 0x0F;  //Codigo del registro en el vector
    tamcelda = TercerByte & 0x03;

    basereg = MV.registros[codreg] >> 16;
    offset = MV.registros[codreg] & 0x0000FFFF;


    PosMem = MV.TSeg[basereg].Base + offset + inmediato;
    //*op = MV.memoria[PosMem];

    if (PosMem >= Tamtot) {
        printf("Error: dirección de memoria fuera de rango\n");
        STOP(&MV, PosMem, 0, 0);
    }

    // Shift para obtener espacio para agregar el tamanio de la celda ademas de tener la direccion de memoria.
    *op = (PosMem << 2) | tamcelda;
}

int ObtenerTamanioCelda(mv MV,int op){

    switch(op & 0x3){
      case 0: return 4; //long
      case 1:
            printf ("Error: tamanio de operando no valido");
            STOP (&MV,0,0,0);
      case 2: return 2; //word
      case 3: return 1; //byte
    }
}

int ValoropST(int tipo, int op, mv* MV){  //Valor operando Segun Tipo
    char posReg = (op >> 4) & 0x0F;
    char sector = (op >> 2) & 0x03;
    int valor = 0, tamcelda, direccion;

    switch(tipo){
        case 1: // Registro
            return getReg(*MV, posReg, sector);  //(mv, direccion en mv.registro, sector)
        case 2: // Inmediato
            return op;
        case 3: { // Memoria
            int tamcelda = ObtenerTamanioCelda(*MV, op);
            int direccion = op >> 2;
            for (int i = 0; i < tamcelda; i++) {
                valor = (valor << 8) | ((*MV).memoria[direccion + i] & 0xFF);
            }
            return valor;
        }
    }
}

void setMemoria(mv* MV, int posMem, int valorB){
    int tamcelda = ObtenerTamanioCelda(*MV, posMem);
    int direccion = posMem >> 2;
    for(int i=tamcelda-1; i>=0; i--){
        (*MV).memoria[direccion + i] = (valorB >>((tamcelda-1- i)*8)) & 0x00FF;
    }
}
