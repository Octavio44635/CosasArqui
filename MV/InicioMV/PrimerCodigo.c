#include <stdio.h>
#include <stdlib.h>


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

void SYS(mv*,int, int );
void JMP(mv*,int, int );
void JZ(mv*,int, int );
void JP(mv*,int, int );
void JZ(mv*,int, int );
void JNZ(mv*,int, int );
void JNP(mv*,int, int );
void JNN(mv*,int, int );
void NOT(mv*,int, int );
void EMPTY(mv*,int, int );
void STOP(mv*,int, int );
void MOV(mv*,int, int );
void ADD(mv*,int, int );
void SUB(mv*,int, int );
void SWAP(mv*,int, int );
void MUL(mv*,int, int );
void DIV(mv*,int, int );
void CMP(mv*,int, int );
void SHL(mv*,int, int  );
void SHR(mv*,int, int  );
void AND(mv*,int, int );
void OR(mv*,int, int );
void XOR(mv*,int, int  );
void LDL(mv*,int, int  );
void LDH(mv*,int, int );
void RND(mv*,int, int );

void LeeCS(mv*);

void SACAREGISTRO(mv, int *, int *);
void INMEDIATO(mv, int *, int *);
void MEMORIA(mv, int *, int * );
void NULO(mv, int *, int *);

int main(){
     //Las funciones del vector deben recibir los mismos parametros

    mv MV; //Variable maquina virtual
    lectura (&MV); //Llamada a la funcion lectura
    LeeCS(&MV);


    return 0;
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
        printf("%d",tamCS);
        //Vector de registros
        CargaRegistros(MV, tamCS);

        fread((*MV).memoria,sizeof(char),tamCS,arch);
        printf("Lectura: %0x %0x \n", (*MV).memoria[40], (*MV).memoria[0]);
        fclose(arch);
        //Memoria ya esta lleno, sale del procedimiento 

        
    }
    else
        printf("Error al abrir el archivo\n");
    
}


void LeeCS (mv *MV){
    void (*Funciones[32])(mv*, int, int, char ) = {EMPTY,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,SACAREGISTRO,INMEDIATO,MEMORIA};


    char instruccion;
    int opA,opB,operacion,IPaux, cont=0;


    IPaux=(*MV).registros[5];
    while ((*MV).registros[5]<=4){  //MV.REGISTROS 5 = IP (*MV).TSeg[0].Tamanio
      instruccion = (*MV).memoria[IPaux]; //levanto el dato del CS apuntado por IP
      opB=(instruccion&0xC0)>>6;    //4 valores, 2 bits
      opA=(instruccion&0x30)>>4;
      operacion=(instruccion &0x1F);
      //En instruccion la receta de la orden
      //xx x_x_xxxx
      FuncOperandos[opB](*MV, &cont, &opB);
      
      FuncOperandos[opA](*MV, &cont, &opA);
      printf("opA: %d, opB: %d\n", opA, opB);
      //Funciones[operacion](MV, opA, opB); //Llama a la funcion correspondiente
      
      IPaux= IPaux + cont;
      (*MV).registros[5]=IPaux; //Graba en el IP, la ultima instruccion leida
    
    }

}

void SACAREGISTRO (mv MV,int *cont, int *op){
    char PosRegistro, Sector;
    *op = MV.memoria[MV.registros[5] + *cont + 1]; //Posicion del CS ####################
    //Sacamemoria = byte a leer
    *cont += 1;
} // opA = 1100 01 00

void INMEDIATO (mv MV,int *cont, int *op){
  short int inmediato;
  inmediato = MV.memoria[MV.registros[5]+*cont + 2];
  inmediato = (MV.memoria[MV.registros[5]+*cont + 1] << 8) | inmediato;
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
    
    codreg= (MV.memoria[MV.registros[5]+ *cont + 1]>>4); //Codigo del registro en el vector
    
    basereg = MV.registros[codreg] >> 16;
    offset= MV.registros[codreg] & 0x00FF;

    //PosMem = MV.TSeg[basereg].Base + offset + inmediato;

    //*op = MV.memoria[PosMem];
    *op = MV.TSeg[basereg].Base + offset + inmediato;
}

void NULO (mv MV, int *cont, int *op){
    *op = -1;
}

void EMPTY(mv* MV, int opA, int opB, char operacion){
    printf("Funcion invalida \n");
}

//SYS,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY

//Terminar de revisar
void MOV (mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3, tipoB=(operacion>>6) & 0x3;
    if (tipoA == 1){ //Registro
        char sectorA = (opA>>2)&0x3;
        int valorA = valorReg(opA>>4, sectorA);
        if(tipoB == 2 || tipoB == 3){
            int valorB = operando(tipoB, opB, *MV);
            switch(sectorA){
                case 0:
                    for (int i=3, i>=0; i--){
                        (*MV).registros[opA+i] = valorB & 0x000f;
                        valorB = valorB >> 8;
                    }
                    break;
                case 1:
                    (*MV).registros[opA + 3] = valorB & 0x000f;
                    break;
                case 2:
                    (*MV).registros[opA + 2] = (valorB>>8) & 0x00f;
                    break;
                case 3:
                    for (int i=3, i>=2; i--){
                        (*MV).registros[opA+i] = valorB & 0x000f;
                        valorB = valorB >> 8;
                    }
                    break;
                default:
                    printf("Error en el sector\n");
                    break;
            }
        }
        else{
            if(tipoB == 1){ //Registro
                char sectorB = (opB>>2)&0x3;
                int valorB = valorReg(opB>>4, sectorB);
                int valorAux = 0;
                valorAux = (*MV).registros[opA];
                switch(sectorA){
                    case 0:
                        valorAux = valorAux & 0x0000;
                        break;
                    case 1:
                        valorAux = valorAux & 0xFFF0;
                        break;
                    case 2:
                        valorAux = valorAux & 0xFF0F;
                        break;
                    case 3:
                        valorAux = valorAux & 0xFF00;
                        break;
                    default:
                        printf("Error en el sector\n");
                        break;
                valorAux = valorAux + valorB;
                (*MV).registros[opA] = valorAux;
                }
            }
        }
    }
    else{
        if(tipoA == 3){ //memoria
            //Todos los posibles tipos de B
            if (tipoB == 0){
                printf("Error en el tipo de operando\n");
            }
            else{
                if(tipoB == 2 || tipoB == 3){//B no es registro
                    int valorB = operando(tipoB, opB, *MV);
                    for(int i=3, i>=0; i--){
                        (*MV).memoria[opA+i] = (valorB >> (i*8)) & 0xFF;
                    }
                    //Aca guarda byte a byte el valor inmediato o del registro
                }
                else{ //Aca B es un registro
                    //Aca guarda el valor del registro en memoria
                    char sector= (opB>>2)&0x3;
                    int ValorMem = 0, valorB = valorReg(opB>>4, sector);  //B = xxxx , 00xx , 000x o 00x0
                    for(int i=0, i<=3; i++){
                        ValorMem = (*MV).memoria[opA+i];
                        ValorMem = ValorMem << 8;
                    }
                    switch(sector){
                    case 0:
                        //Se asigna todo B
                        for(int i=3, i>=0; i--){
                            (*MV).memoria[opA+i] = ValorMem & 0x000F;
                            ValorMem = ValorMem >> 8;
                        }
                    case 1:
                        (*MV).memoria[opA + 3] = ValorMem & 0x000F;
                    case 2:
                        (*MV).memoria[opA + 2] = (ValorMem>>8) & 0x00f;
                    case 3:
                    for(int i=3, i>=2; i--){
                        (*MV).memoria[opA+i] = ValorMem & 0x000F;
                        ValorMem = ValorMem >> 8;
                    }
                    default:
                        printf("Error en el sector\n");
                        break;
                    }
                    //Dependiendo el sector guarda los respectivos bytes en memoria
                }
            }
        } ///////////////////////////
        else{
            if(tipoA == 0 || tipoA == 2){ //Inmediato o nulo
                printf("Error en el tipo de operando\n");
        }
    }

    }
}
int operando(int tipo, int op, mv MV){
    switch(tipo){
        case 0:
            //linea de operando vacio
            break;
        case 1:
            valorReg(op>>4, (op>>2)&0x3);
            break;
        case 2:
            valor = op;
            break;
        case 3:
            valor = MV.memoria[op];
            break;
    }
}
int valorReg(char posReg, char sector){
    int valor=0;
    switch (sector){
        case 0:
            valor = (*MV).registros[posReg];
            break;
        case 1:
            valor = (*MV).registros[posReg] & 0x000f;
            break;
        case 2:
            valor = (*MV).registros[posReg] & 0x00f0;
            break;
        case 3:
        valor = (*MV).registros[posReg] & 0x00ff;
            break;
        default:
            printf("Error en el sector\n");
            break;
    }
    return valor;
}

void ADD (mv* MV, int opA, int opB, char operacion){
    (*MV).memoria[opA]+=opB;
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


int LDH (mv* MV, int opA, int opB){
    int aux=0x00000000;
    opA=OpA & 0xFF00 //quedaria 11111111 11111111 000000....
    aux=aux | (opB & 0x00FF);     
    return aux;
}

int LDL (mv* MV, int opA, int opB){
    int aux=0x00000000;
    aux=opA<<16;
    aux=aux | (opB & 0x00FF);    
    return aux;
}

int RND (mv* MV, int opA, int opB){
    return= rand() % (opB + 1);
}

int AND (mv* MV, int opA, int opB){
    opA= opA & opB;
    if (opA & 0x01) 

}