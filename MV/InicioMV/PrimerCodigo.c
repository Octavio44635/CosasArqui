#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
   short int Base;
   short int Tamanio;
}TablaS;
 

typedef struct{
    char memoria[16384];
    int registros[16];  //Del elemento 8 al 15 son valores enteros, los demas son referencias a memoria
    TablaS TSeg[2];} mv;

void lectura(mv*);
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

void LeeCS(mv*);
void CAMBIACC(mv*, int);

void SACAREGISTRO(mv, int *, int *);
void INMEDIATO(mv, int *, int *);
void MEMORIA(mv, int *, int * );
void NULO(mv, int *, int *);
void BytesMEMORIA(mv, int *, int *);

int getReg(mv , char, char);

char* EscrReg(mv , int );
char* EscrMem(mv , int );

int main(){
    //Las funciones del vector deben recibir los mismos parametros

    mv MV; //Variable maquina virtual
    lectura (&MV); //Llamada a la funcion lectura
    LeeCS(&MV);

    Disassembler(&MV);
    /*if(args >= 4 && argv[4] == "-d")
        Disassembler(&MV);

    return 0;*/
}

void CargaRegistros(mv* MV, int tamCS){
    
    (*MV).TSeg[0].Base = 0; //Base CS
    (*MV).TSeg[0].Tamanio = tamCS; //Tamanio CS
    (*MV).TSeg[1].Base = tamCS; //Base DS
    (*MV).TSeg[1].Tamanio = 16384 - tamCS; //Tamanio DS

    (*MV).registros[0] = 0x0; //CS
    (*MV).registros[1] = 0x10000; //DS
    (*MV).registros[5] = ((*MV).TSeg[0].Base << 16) & 0x0; //IP

    //Los demas registros se inicializan al usarlos
}

void lectura(mv* MV){

    FILE *arch = fopen("sample.vmx", "rb");
    if( arch != NULL){
        fread((*MV).memoria, sizeof(char), 8, arch); //Se leen bytes de forma arbitraria, los primeros 7 son el header, siendo los ultimos 2 el tamaño 
        
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
    void (*Funciones[32])(mv*, int, int, char) = {EMPTY,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
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

        
        printf("TipoA: %d, TipoB: %d\n", opA, opB);

        FuncOperandos[opB](*MV, &cont, &opB); //(MV, cont, bytesOp);
        //Primero el opB, bien
        
        FuncOperandos[opA](*MV, &cont, &opA);

        printf("Byte: %0x,  opA: %0x, opB: %0x\n",instruccion, opA, opB);
        
        if(operacion >= 0x0 & operacion<= 0x8)//Solo un operando
            if(opA != 0)//Ignoramos B?????
                Funciones[operacion](MV, opA, 0, instruccion); //Llama a la funcion correspondiente
            else{
                printf("Error, operando A nulo\n");
                STOP(MV, opA, opB, instruccion); //Error
            }
        else
            if(operacion >= 0x10 & operacion <= 0x1E)
                if(opA != 0 && opA != 2){
                    if(opB != 0)
                        Funciones[operacion](MV, opA, opB, instruccion); //Llama a la funcion correspondiente
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
                printf("Fin de ejecucion");
                STOP(MV, opA, opB, instruccion); //Error
            }
        cont++;
        printf("IPAux: %d, cont: %d\n", IPaux, cont);
        printf("\n");
        IPaux= IPaux + cont;
        cont = 0;
        (*MV).registros[5]=IPaux; //Graba en el IP la proxima instruccion a leer
        
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
    codreg= (MV.memoria[MV.registros[5]+ *cont]>>4); //Codigo del registro en el vector
    
    basereg = MV.registros[codreg] >> 16;
    offset= MV.registros[codreg] & 0x00FF;

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
    *op = -1;
}

void EMPTY(mv* MV, int opA, int opB, char operacion){
    printf("Funcion invalida \n");
    STOP(MV, 0, 0, 0); //Error
}

//SYS.
//Terminar de revisar

void setMemoria(mv* MV, int posMem, int valorB){
    for(int i=3; i>=0; i--){
        (*MV).memoria[posMem+i] = (valorB >> (i*8)) & 0xFF;
    }
}

void setReg(mv *MV, char posA, char sectorA, int valorB){
    if(valorB == -1){//Por ahi borrar
        printf("Error en el tipo de ValoropST\n");
    }
    else
        switch(sectorA){
            case 0:
                (*MV).registros[posA] = valorB;
                break;
            case 1:
                (*MV).registros[posA] &= 0xFFF0;
                (*MV).registros[posA] |= valorB & 0x000F;
                break;
            case 2:
                (*MV).registros[posA] &= 0xFF0F;
                (*MV).registros[posA] |= valorB & 0x00F0;
                break;
            case 3:
                (*MV).registros[posA] &= 0xFF00;
                (*MV).registros[posA] |= valorB & 0x00FF;
                break;
            default:
                printf("Error en el sector\n");
                STOP(MV, 0, 0, 0); //Error
                break;
        }
}

int ValoropST(int tipo, int op, mv* MV){ //Valor operando Segun Tipo
    char posReg = op>>4;
    char sector = (op>>2) & 0x3;
    switch(tipo){
        case 0:
            return -1;//linea de ValoropST vacio, return -1???
            break;
        case 1:
            return getReg(*MV, posReg, sector); //(mv, direccion en mv.registro, sector)
            break;
        case 2:
            return op;
            break;
        case 3:
            return (*MV).memoria[op];
            break;
    }
}

int getReg(mv MV, char posReg, char sector){
    int valor=0;

    switch (sector){
        case 0:
            valor = MV.registros[ posReg];
            break;
        case 1:
            valor = MV.registros[posReg] & 0x000f;
            break;
        case 2:
            valor = MV.registros[posReg] & 0x00f0;
            break;
        case 3:
        valor = MV.registros[posReg] & 0x00ff;
            break;
        default:
            printf("Error en el sector\n");
            break;
    }
    return valor;
}

void set(mv* MV, int opA, char tipoA, int valorB){
    if(tipoA == 1)
        setReg(MV, opA>>4, (opA>>2) & 0x03, valorB); //(MV, direccion en mv.registro, sector, valorB)
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
    }

    void SUB(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        set(MV, opA, tipoA, ValoropST(tipoA, opA, MV) - ValoropST(tipoB, opB, MV));
    }

    void MUL(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        set(MV, opA, tipoA, ValoropST(tipoA, opA, MV)*ValoropST(tipoB, opB, MV));
    }

    void DIV(mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int valorB = ValoropST(tipoB, opB, MV);

        if(valorB != 0)
            set(MV, opA, tipoA, ValoropST(tipoA, opA, MV)/valorB);
        else{
            printf("Error de division por 0\n");
            STOP(MV, 0, 0, 0); //Error division por 0
        }
    }

    void JMP (mv* MV, int opA, int opB, char operacion){
        //JMP
        (*MV).registros[5] = opA;
    }

    void JZ (mv* MV, int opA, int opB, char operacion){
        //JZ
        
        if (((*MV).registros[8]) & 0x8000 ==0  && (*MV).registros[8] & 0x4000 ==0){ //CC
            (*MV).registros[5] = opA; //IP
        }
    }

    void JP(mv* MV, int opA, int opB, char operacion){
        if (((*MV).registros[8] & 0x8000) ==0 && ((*MV).registros[8] & 0x4000) ==0) {
            (*MV).registros[5] = opA; // IP
        }
    }

    void JN (mv* MV, int opA, int opB, char operacion){
            
        if (((*MV).registros[8]) & 0x8000==1 && ((*MV).registros[8]) & 0x4000 ==0){ //CC
            (*MV).registros[5] = opA; //IP
        }
    }

    void JNZ (mv* MV, int opA, int opB, char operacion){
        //JN
        
        if ((((*MV).registros[8]) & 0x8000 ==1 && (*MV).registros[8] & 0x4000 ==0) || ((*MV).registros[8]) & 0x8000 ==0 &&  (*MV).registros[8] & 0x4000 ==0){ //CC
            (*MV).registros[5] = opA; //IP
        }
    }

    void JNP (mv* MV, int opA, int opB, char operacion){
        //JN
        
        if ((((*MV).registros[8]) & 0x8000 ==1 &&  (*MV).registros[8] & 0x4000 ==0) || ((*MV).registros[8]) & 0x8000 ==0 &&  (*MV).registros[8] & 0x4000 ==1){ //CC
            (*MV).registros[5] = opA; //IP
        }
    }

    void JNN (mv* MV, int opA, int opB, char operacion){
        //JN

        if ((((*MV).registros[8]) & 0x8000 ==0 &&  (*MV).registros[8] & 0x4000 ==1) || ((*MV).registros[8]) & 0x8000 ==0 && (*MV).registros[8] & 0x4000 ==0){ //CC
            (*MV).registros[5] = opA; //IP
        }
    }

    void LDH (mv* MV, int opA, int opB, char operacion){
        int aux=0x00000000;
        opA=opA & 0xFF00; //quedaria 11111111 11111111 000000....
        aux=aux | (opB & 0x00FF);
        set(MV, opA, (operacion>>4)&0x3 ,aux);
    }

    void LDL (mv* MV, int opA, int opB, char operacion){
        int aux=0x00000000;
        aux=opA<<16;
        aux=aux | (opB & 0x00FF);
        set(MV, opA, (operacion>>4)&0x3 ,aux);
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
        (*MV).registros[5] = sizeof((*MV).memoria + 1);
    }

    void SWAP (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        char tipoB=(operacion>>6) & 0x3;
        int aux;
        if ((tipoA==3 && tipoB==3) || (tipoA==3 && tipoB==2) || (tipoA==2 && tipoB==2) || (tipoA==2 && tipoB==3)){
        aux=opA;
        opA=opB;
        opB=aux;
        }
        else
        printf("Error,operando INMEDIATO o NULO"); //q hago aca?
        
    }

    void NOT (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        set(MV, opA, tipoA, ~ValoropST(tipoA, opA, MV));
        CAMBIACC(MV,opA);
    }

    void SHL (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        int valorB = ValoropST((operacion>>6) & 0x3, opB, MV);
        if (tipoA==0 || tipoA==2)
        printf ("Error,operando INMEDIATO o NULO");
        else
        if (tipoA==1){
            //set(MV, opA, tipoA, ValoropST(tipoA, opA, MV) << ValoropST(operacion>>6, opB, MV)); por las dudas no
            (*MV).registros[opA] = (*MV).registros[opA]<< valorB;
            CAMBIACC(MV,(*MV).registros[opA]);
        }
        else{ //tipoA==3
            (*MV).memoria[opA]= (*MV).memoria[opA]<< valorB;
            CAMBIACC(MV,(*MV).memoria[opA]);
        }  
    }

    void SHR (mv* MV, int opA, int opB, char operacion){
        char tipoA=(operacion >> 4) & 0x3;
        int mascara, valorB = ValoropST((operacion>>6) & 0x3, opB, MV);
        if (tipoA==0 || tipoA==2)
        printf ("Error,operando INMEDIATO o NULO");
        else
        if (tipoA==1){
            (*MV).registros[opA] = (*MV).registros[opA]>> valorB;
            if (opA & 0x8000){
                mascara = ((0xFFFF) << (16 - valorB)) & 0xFFFF;
                (*MV).registros[opA] |= mascara;
            }
            CAMBIACC(MV,(*MV).registros[opA]);
        }
        else{ //tipoA==3
            (*MV).memoria[opA]= (*MV).memoria[opA]>> valorB;
            if (opA & 0x80000000){
                mascara = ((0xFFFFFFFF) << (32 - valorB)) & 0xFFFFFFFF;
                valorB|= mascara;
        }
        CAMBIACC(MV,(*MV).memoria[opA]);
        }
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
void CAMBIACC (mv* MV, int opA){
    if (opA & 0x8000){ //da un valor
        (*MV).registros[8] |= 0x8000; // Si el If da 1, es negativo, Activo N (bit 15)
        (*MV).registros[8] &= ~0x4000; // Apaga bit Z (bit 14)
    }
    else{
        if(opA==0){
            (*MV).registros[8] |= 0x4000; // Si el If da 1, es cero, Activo Z (bit 14)
            (*MV).registros[8] &= ~0x8000;// Apaga bit N (bit 15)
        }
    }
}

void BytesMEMORIA(mv MV, int *cont, int *operando){
    int todo=0;
    todo = MV.memoria[MV.registros[5]+*cont + 2];
    todo = (MV.memoria[MV.registros[5]+*cont + 1] << 8) | todo;
    //0 inmediato inmediato registro
    todo = (MV.memoria[MV.registros[5]+*cont + 3] << 8) | todo;
    //
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
        int operandoA = "", operandoB="";

        //El tamaño y el tipo son iguales
        FuncOperandos[tipoA](*MV, &cont, &operandoA);
        FuncOperandos[tipoB](*MV, &cont, &operandoB);
        (*MV).registros[5] += cont; //Aumento el IP
        cont = 0; //Reinicio el contador

        //Hay un problema con el contador pero lo voy a solucionar en otro momento, parece joda pero se suma valores solo


        /* Si es un registro cont va aumentar 1, y saldra el byte con el registro y el sector
        Si es un inmediato sale directamente con el inmediato
        Si es memoria, cont aumenta 2 y sale el inmediato y el registro en el entero
        */
        printf("[%x] %x %x %x   | %s", (*MV).registros[5], operacion, operandoA, operandoB, nomFun[instruccion & 0x1F]);
        if(tipoA == 2)
            printf(" %x ", operandoA);
            else
            if(tipoA == 1)
                printf(" %x ", EscrReg(*MV, operandoA));
            else
                if(tipoA == 3)
                    printf(" %x ", EscrMem(*MV, operandoA));
                else
                    printf(" ");
        if(tipoB == 2)
            printf(" %x ", operandoB);
            else
            if(tipoB == 1)
                printf (" %x ", EscrReg(*MV, operandoB));
            else
                if(tipoB == 3)
                    printf (" %x ", EscrMem(*MV, operandoB));
                else
                    printf("  ");

       //printf("[%x] %x     |    %s , %s ", (*MV).registros[5],(), instruccion, escritura(A), escritura(B));
        
    }
}

char* EscrReg(mv MV, int operando){
    char* nombresRegistros[16] = {"CS", "DS", "", "", "", "IP", "", "", "CC", "AC", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX"};
    char* palabra;
    char sector = (operando >> 2) & 0x3, registro = operando >> 4;
    switch(sector){
        case 0:
            return nombresRegistros[registro];
            break;
        case 1:
            palabra = nombresRegistros[registro];
            palabra[1] = "";
            palabra[3] = "L";
            break;
        case 2:
            palabra = nombresRegistros[registro];
            palabra[1] = "";
            palabra[3] = "H";
            break;
        case 3:
            palabra = nombresRegistros[registro];
            palabra[1] = "";
            palabra[2] = "X";
            break;
    return palabra;
    }
}

char* EscrMem(mv MV, int operando){
    char* palabra, registro=0;
    short int inmediato=0;

    registro = operando & 0x0000FF;
    inmediato = (operando >> 8);

    palabra = "[%x + %d]", EscrReg(MV, registro), inmediato;
    //palabra = "[%x + %d]", bytesReg, inmeidato;
    return palabra;
}

        
