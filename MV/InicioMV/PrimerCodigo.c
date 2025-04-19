#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#

#define EDX 13;



typedef struct{
   short int Base;
   short int Tamanio;
}TablaS;
 

typedef struct{
    char memoria[16384];
    int registros[16];  //Del elemento 8 al 15 son valores enteros, los demas son referencias a memoria
    TablaS TSeg[2];} mv;

void lectura(mv*, char*[]);
void CargaRegistros(mv*, int);

void SYS(mv*,int, int,char );
void JMP(mv*,int, int,char );
void JZ(mv*,int, int,char );
void JP(mv*,int, int,char );
void JZ(mv*,int, int,char );
void JNZ(mv*,int, int,char );
void JNP(mv*,int, int,char );
void JNN(mv*,int, int,char );
void NOT(mv*,int, int,char );
void EMPTY(mv*,int, int,char );
void STOP(mv*,int, int,char );
void MOV(mv*,int, int,char );
void ADD(mv*,int, int,char );
void SUB(mv*,int, int,char );
void SWAP(mv*,int, int,char );
void MUL(mv*,int, int,char );
void DIV(mv*,int, int,char );
void CMP(mv*,int, int,char );
void SHL(mv*,int, int,char  );
void SHR(mv*,int, int,char  );
void AND(mv*,int, int,char );
void OR(mv*,int, int,char );
void XOR(mv*,int, int,char  );
void LDL(mv*,int, int,char  );
void LDH(mv*,int, int,char );
void RND(mv*,int, int,char );

void SYSR(mv* , int , char , char, short int);
void SYSW(mv* , int , char, char, short int );

void LeeCS(mv*);
void CAMBIACC(mv*, int);

int punteroReg(mv , int , int* );

void EscrFormato(mv* , int ,int , short int, char);
void leeFormato(mv , int *, int , char );

void SACAREGISTRO(mv, int *, int *);
void INMEDIATO(mv, int *, int *);
void MEMORIA(mv, int *, int * );
void NULO(mv, int *, int *);
void BytesMEMORIA(mv, int *, int *);

int getReg(mv , char, char);

char* EscrReg(mv , int );
char* EscrMem(mv , int );

void Disassembler(mv*);

int main(int argc, char *argv[]){
    //Las funciones del vector deben recibir los mismos parametros

    mv MV; //Variable maquina virtual
    lectura (&MV, argv); //Llamada a la funcion lectura
    printf("%s %s %s", argv[0], argv[1], argv[2]);
    LeeCS(&MV);

    printf("\n");
    if(strcmp(argv[2],"-d") == 0)
        Disassembler(&MV);
        

    return 0;
}

void CargaRegistros(mv* MV, int tamCS){
    
    (*MV).TSeg[0].Base = 0; //Base CS
    (*MV).TSeg[0].Tamanio = tamCS; //Tamanio CS
    (*MV).TSeg[1].Base = tamCS; //Base DS
    (*MV).TSeg[1].Tamanio = 16384 - tamCS; //Tamanio DS

    (*MV).registros[0] = 0x0;
    (*MV).registros[1] = 0x0;
    (*MV).registros[10] = 0x0;
    (*MV).registros[11] = 0x0;
    (*MV).registros[12] = 0x0;
    (*MV).registros[13] = 0x0;
    (*MV).registros[14] = 0x0;
    (*MV).registros[15] = 0x0;

    (*MV).registros[1] = (*MV).TSeg[1].Base ; //CS

    (*MV).registros[1] = (*MV).TSeg[1].Base ; //DS
    (*MV).registros[5] = ((*MV).TSeg[0].Base << 16) & 0x0; //IP

    //Los demas registros se inicializan al usarlos
}

void lectura(mv* MV, char* argv[]){

    FILE *arch = fopen(argv[1], "rb");
    //FILE *arch = fopen("c:/Users/Octii/Desktop/Maquina_Virtual_1/pruebalau.vmx", "rb");
    if( arch != NULL){
        fread((*MV).memoria, sizeof(char), 8, arch); //Se leen bytes de forma arbitraria, los primeros 7 son el header, siendo los ultimos 2 el tamaño
        char* cabecera = "VMX25";
        int i=0;
        int iguales = 1;
        while(i<5 && iguales){
            iguales = cabecera[i] == (*MV).memoria[i];
            i++;
        }
        if(!iguales ){
            printf("Error, cabecera no valida\n");
            fclose(arch);
            STOP(MV, 0, 0, 0); //Error
        }
        
        int tamCS = ((*MV).memoria[6] << 8 )| (*MV).memoria[7];
        printf("%d \n",tamCS);
        //Vector de registros
        CargaRegistros(MV, tamCS);

        fread((*MV).memoria,sizeof(char),tamCS,arch);
        fclose(arch);
        //Memoria ya esta lleno, sale del procedimiento

        
    }
    else
        printf("Error al abrir el archivo\n");
    
}

void LeeCS (mv *MV){
    void (*Funciones[32])(mv*, int, int, char) = {SYS,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,MEMORIA};


    char instruccion;
    int opA,opB,operacion,IPaux, cont=0;


    IPaux=(*MV).registros[5];
    while ((*MV).registros[5]< (*MV).TSeg[0].Tamanio){  //IP <= (*MV).TSeg[0].Tamanio
        instruccion = (*MV).memoria[IPaux]; //levanto el dato del CS apuntado por IP
        opB=(instruccion&0xC0)>>6;    //4 valores, 2 bits
        opA=(instruccion&0x30)>>4;
        operacion=(instruccion &0x1F);
        //En instruccion la receta de la orden
        //xx x_x_xxxx
        int BytesA=0, BytesB=0;
        
        FuncOperandos[opB](*MV, &cont, &BytesB); //(MV, cont, bytesOp);
        
        FuncOperandos[opA](*MV, &cont, &BytesA);
        cont++;
        IPaux= IPaux + cont;
        cont = 0;
        (*MV).registros[5]=IPaux; //Graba en el IP la proxima instruccion a leer
        
        if(operacion >= 0x0 & operacion<= 0x8)//Solo un operando
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
        IPaux = (*MV).registros[5]; //Graba en el IP la proxima instruccion a leer, importa al hacer jump
    }

}

void SACAREGISTRO (mv MV,int *cont, int *bytesOp){
    *bytesOp=0;
    *cont += 1;
    *bytesOp = MV.memoria[MV.registros[5] + *cont]; //Posicion del CS ####################
    //Sacamemoria = byte a leer
    
} // opA = 1100 01 00

void INMEDIATO (mv MV,int *cont, int *op){
  short int inmediato = 0;
  inmediato = MV.memoria[MV.registros[5]+*cont + 2];
  inmediato = (MV.memoria[MV.registros[5]+*cont + 1] << 8) | inmediato;
  *op = 0;
  *op = inmediato;
  *cont += 2; //Aumento el contador
}

void MEMORIA (mv MV, int *cont, int *op){
    short int offset, basereg, inmediato;
    char codreg;
    int PosMem;

    //primero se lee un inmediato, dos bytes

    //Funcion INMEDIATO
    inmediato = MV.memoria[MV.registros[5]+*cont + 2];
    inmediato = (MV.memoria[MV.registros[5]+*cont + 1] << 8) | inmediato;
    *cont += 2;
    //
    *cont += 1; //Aumento el contador
    codreg= (MV.memoria[MV.registros[5]+ *cont]>>4)&0x0F; //Codigo del registro en el vector
    
    basereg = MV.registros[codreg] >> 16;
    offset= MV.registros[codreg] & 0x0000FFFF;

    //PosMem = MV.TSeg[basereg].Base + offset + inmediato;

    //*op = MV.memoria[PosMem];
    *op = 0;
    *op = MV.TSeg[basereg].Base + offset + inmediato;
    if(*op > 16384){
        printf("Error, direccion de memoria fuera de rango\n");
        STOP(&MV, *op, 0, 0); //Error
    }
}

void NULO (mv MV, int *cont, int *op){
}

void EMPTY(mv* MV, int opA, int opB, char operacion){
    printf("Funcion invalida \n");
    STOP(MV, 0, 0, 0); //Error
}

//SYS.
//Terminar de revisar

void setMemoria(mv* MV, int posMem, int valorB){
    for(int i=3; i>=0; i--){
        (*MV).memoria[posMem+i] = (valorB >> ((3-i)*8)) & 0xFF;
    }
}

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
            (*MV).registros[posA] |= valorB & 0x000000FF << 8;
            break;
        case 3:
            (*MV).registros[posA] &= 0xFFFF0000;
            (*MV).registros[posA] |= valorB & 0x0000FFFF;
            break;
        default: //Es necesario???
            printf("Error en el sector\n");
            STOP(MV, 0, 0, 0); //Error
            break;
    }
}

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
        case 3: //Se corrige esta sentencia ya que se devolvia un byte de memoria, hay que devolver 4
            for (i=0; i<4; i++){
                valor = (valor << 8) | (*MV).memoria[op+i];
            }
            return valor;
            break;
    }
}

int getReg(mv MV, char posReg, char sector){
    int valor=0;

    switch (sector){
        case 0:
            valor = MV.registros[posReg];
            break;
        case 1:
            valor = MV.registros[posReg] & 0x000000FF;
            break;
        case 2:
            valor = MV.registros[posReg] & 0x0000FF00;
            valor >> 8;
            break;
        case 3:
            valor = MV.registros[posReg] & 0x0000FFFF;
            break;
        default:
            printf("Error en el sector\n");
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
            (*MV).registros[9] = valorA%valorB; //Resto
        }
        else{
            printf("Error de division por 0\n");
            STOP(MV, 0, 0, 0); //Error division por 0
        }
    }

    void JMP (mv* MV, int opA, int opB, char operacion){
        //JMP
        (*MV).registros[5] = opB;
    }

    void JZ (mv* MV, int opA, int opB, char operacion){
        //JZ
        
        if ((*MV).registros[8] == 0x40000000 ){ //CC
            (*MV).registros[5] = opB; //IP
        }
    }

    void JP(mv* MV, int opA, int opB, char operacion){
        if (((*MV).registros[8] == 0x80000000) && ((*MV).registros[8] == 0x40000000)) {
            (*MV).registros[5] = opB; // IP
        }
    }

    void JN (mv* MV, int opA, int opB, char operacion){
            
        if (((*MV).registros[8]) == 0x80000000){ //CC
            (*MV).registros[5] = opB; //IP
        }
    }

    void JNZ (mv* MV, int opA, int opB, char operacion){
        if ((*MV).registros[8] == 0){ //CC
            (*MV).registros[5] = opB; //IP
        }
    }

    void JNP (mv* MV, int opA, int opB, char operacion){
        //JN
        
        if ((((*MV).registros[8]) == 0x80000000 &&  (*MV).registros[8] == 0x40000000) || ((*MV).registros[8]) == 0x80000000 &&  (*MV).registros[8] == 0x40000000){ //CC
            (*MV).registros[5] = opB; //IP
        }
    }

    void JNN (mv* MV, int opA, int opB, char operacion){
        //JN

        if (((*MV).registros[8]) == 0x80000000){ //CC
            (*MV).registros[5] = opB; //IP
        }
    }

    void LDH (mv* MV, int opA, int opB, char operacion){
        int valorA = ValoropST((operacion>>4)&0x3, opA, MV);
        int valorB = ValoropST((operacion>>6)&0x3, opB, MV);
        //los 2 bytes mas sign del primero con los 2 bytes menos sign del segundo
        valorA=valorA & 0x0000FFFF; //quedaria 11111111 11111111 000000....
        valorA=valorA | ((valorB << 16 )& 0xFFFF0000);

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

        CAMBIACC(MV,opA);
    }

    void OR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoA, opA, MV) | ValoropST(tipoB, opB, MV);
        set(MV, opA, (operacion>>4)&0x3 ,valor);
        CAMBIACC(MV,opA);
    }

    void XOR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valor= ValoropST(tipoA, opA, MV) ^ ValoropST(tipoB, opB, MV);
        set(MV, opA, (operacion>>4)&0x3 ,valor);
        CAMBIACC(MV,opA);
    }

    void STOP(mv* MV, int opA, int opB, char operacion){
        //STOP
        (*MV).registros[5] = (*MV).TSeg[0].Base + (*MV).TSeg[0].Tamanio; //IP
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
        char tipoA=(operacion >> 4) & 0x3;
        set(MV, opA, tipoA, ~ValoropST(tipoA, opA, MV));
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
        int mascara, valorB = ValoropST((operacion>>6) & 0x3, opB, MV), aux;
        aux = ValoropST(tipoA, opA, MV) >> valorB;

        if (opA & 0x80000000){
            mascara = ((0xFFFFFFFF) << (32 - valorB)) & 0xFFFFFFFF;
            valorB|= mascara;
        }
        set(MV, opA, tipoA, aux);
        CAMBIACC(MV,aux);
    }

    void CMP (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valorA=ValoropST(tipoA, opA, MV);
        int valorB=ValoropST(tipoB, opB, MV);
        CAMBIACC(MV, valorA-valorB);
        /*
        (*MV).registros[8] &= 0x0000;
        if(valorA == valorB) //Activa el Z
            (*MV).registros[8] |= 0x4000;   //Las mascaras estan bien?
        else
            if(valorA < valorB) //Activa el N
                (*MV).registros[8] |= 0x8000;
        //Sino no pasa nada por que el CC ya esta en 0 */
    }
    

////////
int punteroReg(mv MV, int dirReg, int* op){

    short int basereg = MV.registros[dirReg] >> 16;
    short int offset= MV.registros[dirReg] & 0x0000FFFF;

    *op = 0;
    *op = MV.TSeg[basereg].Base + offset;

    //Ver de implementar esto en el procedimiento de MEMORIA
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
    printf("\n CL: %d,  CH: %d, AL: %d \n", CL, CH, AL);
    if (CL == 0 || CH == 0){
        printf("Error, syscall no valida, debido a las celdas\n");
        STOP(MV, 0, 0, 0); //Error
    }
    else
        if (valorB == 1)
            SYSR(MV, puntEdx, CH, CL, AL);
        else
            if(valorB == 2)
                SYSW(MV, puntEdx, CH, CL, AL);
            else{
                printf("Error, syscall no valida\n");
                STOP(MV, 0, 0, 0); //Error
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
    case 0x10: //Binario, dios me ampare
        cadenaBin = malloc(CH*8);
        scanf("%s", cadenaBin);
        //strcpy(cadenaBin,"10011001");
        valor = 0;
        int max;

        if (strlen(cadenaBin) > CH*8)
            max = CH*8; //En esta situacion los bits menos significativos seran cortados, ideal seria que se introduzca un numero correcto de bit
        else
            max = strlen(cadenaBin);

        max--; //Le "saca" el caracter nulo

        for (int i=max; i>0; i--){
            if(cadenaBin[i] == '0' || cadenaBin[i] == '1')
                valor = valor | ((cadenaBin[i]-'0')<<(max-i));
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
    

    printf("[%04X]      ", punt - (*MV).TSeg[1].Base + i*CH);

    switch (AL)
    {
    case 0x01://decimal, se lee y se guarda en CH bytes
        for(int j=0; j<CH; j++){
            escritura |= (*MV).memoria[punt + i*CH + j] << ((CH-1-j)*8);
        }
        printf("%d", escritura);
        break;
    case 0x02: //caracteres, se leen CH char
        for(int j=0; j<CH; j++){
            printf("%c", (*MV).memoria[punt + i*CH + j]);
        }
        break;
    case 0x04: //Octal
        for(int j=0; j<CH; j++){
            printf("%o", (*MV).memoria[punt + i*CH + j]);
        }
        break;
    case 0x08: //Hexadecimal
        for(int j=0; j<CH; j++){
            printf("%x", (*MV).memoria[punt + i*CH + j]);
        }
        break;
        
    case 0x10: //Binario, dios me ampare
        printf("Escritura en binario\n");
        char bit;
        int j;
        for (int j=0; j<CH*8; j++){
            cadenaBits[j] = '0';
        }
        cadenaBits[CH*8] = '\0';
        //printf("%d", MV.memoria[punt + i]);
        for(j=0; j<CH; j++){
            escritura |= (*MV).memoria[punt + i*CH + j] << ((CH-1-j)*8);
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
        printf("%s", cadenaBits);
        break;
        
    default:
        printf("Error, formato no valido\n");
        STOP(&MV, 0, 0, 0); //Error
    }
}

///////
void CAMBIACC (mv* MV, int valor){
    if (valor < 0){
        (*MV).registros[8] |= 0x80000000; // Si el If da 1, es negativo, Activo N (bit 15)
        (*MV).registros[8] &= ~0x40000000; // Apaga bit Z (bit 14)
    }
    else{
        if(valor==0){
            (*MV).registros[8] |= 0x40000000; // Si el If da 1, es cero, Activo Z (bit 14)
            (*MV).registros[8] &= ~0x80000000;// Apaga bit N (bit 15)
        }
        else{
            (*MV).registros[8] = 0;
        }
    }
}

void BytesMEMORIA(mv MV, int *cont, int *operando){
    int todo=0;
    todo = MV.memoria[MV.registros[5]+*cont + 3] & 0xFF;
    todo = (MV.memoria[MV.registros[5]+*cont + 2] << 8) | todo;
    todo = (MV.memoria[MV.registros[5]+*cont + 1] << 16) | todo;


    *cont += 3;
    *operando = todo;
}

void Disassembler(mv* MV){
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,BytesMEMORIA};
    int cont=0; //Este contador es para saber cuantos bytes se leyeron
    (*MV).registros[5]=(*MV).TSeg[0].Base; //IP = CS

    char* nomFun[32] = {"SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","NOT","","","","","","","STOP","MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","LDL","LDH","RND"};

    while((*MV).registros[5] < ((*MV).TSeg[0].Base + (*MV).TSeg[0].Tamanio)){
        char instruccion = (*MV).memoria[(*MV).registros[5]];
        //Ya se mostro la orden, ahora se procede a mostrar los operandos
        char tipoA = (instruccion >> 4) & 0x3, operacion = instruccion & 0x1F,tipoB = (instruccion >> 6) & 0x3;
        int operandoA, operandoB, i;

        //El tamaño y el tipo son iguales
        FuncOperandos[tipoB](*MV, &cont, &operandoB);
        FuncOperandos[tipoA](*MV, &cont, &operandoA);
        (*MV).registros[5] += cont+1; //Aumento el IP
        cont = 0; //Reinicio el contador
        printf("[%04X] %02X ", (*MV).registros[5], instruccion&0x00FF);

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
                    printf(" %s, ", EscrMem(*MV, operandoA));
                else
                    printf("");
        if(tipoB == 2){
            short int valor;
            if (operandoB & 0x8000){
                valor =  (~operandoB) + 1;   //Preguntar
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
                    printf (" %s ", EscrMem(*MV, operandoB));
                else
                    printf("");
        printf("\n");

       //printf("[%x] %x     |    %s , %s ", (*MV).registros[5],(), instruccion, escritura(A), escritura(B));
        
    }
}

char* EscrReg(mv MV, int operando){
    char* nombresRegistros[16] = {"CS", "DS", "", "", "", "IP", "", "", "CC", "AC", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX"};
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
        inmediato &= 0x0000FFFF; //Atento a esto que no me da seguridad
    }
    else if(inmediato < 0){
        inmediato = (~inmediato) + 1;
        snprintf(palabra, sizeof(palabra), "[%s - %d]", EscrReg(MV, registro), inmediato);
    }
    else
        snprintf(palabra, sizeof(palabra), "[%s]", EscrReg(MV, registro));
    //palabra = "[%x + %d]", bytesReg, inmeidato;
    return palabra;
}