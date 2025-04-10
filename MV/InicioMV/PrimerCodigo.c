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

void SYS(mv*,int, int,char );
void JMP(mv*,int, int,char );
void JZ(mv*,int, int,char );
void JP(mv*,int, int,char );
void JZ(mv*,int, int,char );
void JNZ(mv*,int, int,char );
void JNP(mv*,int, int,char );
void JNN(mv*,int, int,char );
int NOT(mv*,int, int,char );
void EMPTY(mv*,int, int,char );
void STOP(mv*,int, int,char );
void MOV(mv*,int, int,char );
void ADD(mv*,int, int,char );
void SUB(mv*,int, int,char );
void SWAP(mv*,int, int,char );
void MUL(mv*,int, int,char );
void DIV(mv*,int, int,char );
void CMP(mv*,int, int,char );
int SHL(mv*,int, int,char  );
int SHR(mv*,int, int,char  );
int AND(mv*,int, int,char );
int OR(mv*,int, int,char );
int XOR(mv*,int, int,char  );
int LDL(mv*,int, int,char  );
int LDH(mv*,int, int,char );
int RND(mv*,int, int,char );

void LeeCS(mv*);
void CAMBIACC(mv*, int);

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
    void (*Funciones[32])(mv*, int, int, char) = {EMPTY,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY};
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
      
    if(operacion >= 0x0 & operacion<= 0x8)//Solo un operando
        if(opA != 0)
            Funciones[operacion](MV, opA, opB, instruccion); //Llama a la funcion correspondiente
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
}

//SYS.
//Terminar de revisar

void setMemoria(mv* MV, int opA, int valorB){
    for(int i=3; i>=0; i--){
        (*MV).memoria[opA+i] = (valorB >> (i*8)) & 0xFF;
    }
}

void setReg(mv *MV, char opA, char sectorA, int valorB){
    if(valorB == -1){
        printf("Error en el tipo de ValoropST\n");
    }
    else
        switch(sectorA){
            case 0:
                (*MV).registros[opA] = valorB;
                break;
            case 1:
                (*MV).registros[opA] &= 0xFFF0;
                (*MV).registros[opA] |= valorB & 0x000F;
                break;
            case 2:
                (*MV).registros[opA] &= 0xFF0F;
                (*MV).registros[opA] |= valorB & 0x00F0;
                break;
            case 3:
                (*MV).registros[opA] &= 0xFF00;
                (*MV).registros[opA] |= valorB & 0x00FF;
                break;
            default:
                printf("Error en el sector\n");
                break;
        }
}

int ValoropST(int tipo, int op, mv* MV){ //Valor operando Segun Tipo
    switch(tipo){
        case 0:
            return -1;//linea de ValoropST vacio, return -1???
            break;
        case 1:
            return getReg(*MV, op>>4, (op>>2)&0x3); //(mv, direccion en mv.registro, sector)
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
            valor = MV.registros[posReg];
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
    if(tipoA == 1){
        setReg(MV, opA>>4, (opA>>2) & 0x03, valorB);
    }
    else{
        if (tipoA == 3){
            setMemoria(MV, opA, valorB);
        }
    }
}

void MOV (mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3;
    char tipoB=(operacion>>6) & 0x3;
    set(MV, opA, tipoA,ValoropST(tipoB, opB, MV));
}

void ADD(mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3;
    char tipoB=(operacion>>6) & 0x3;
    set(MV, opA, tipoA, ValoropSt(tipoB, opB, MV) + ValoropST(tipoA, opA, MV));
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
    else
        printf("Error de division por 0\n");
        STOP(MV, opA, opB, operacion); //Error division por 0
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

int LDH (mv* MV, int opA, int opB, char operacion){
    int aux=0x00000000;
    opA=opA & 0xFF00; //quedaria 11111111 11111111 000000....
    aux=aux | (opB & 0x00FF);
    return aux;
}

int LDL (mv* MV, int opA, int opB, char operacion){
    int aux=0x00000000;
    aux=opA<<16;
    aux=aux | (opB & 0x00FF);
    return aux;
}

int RND (mv* MV, int opA, int opB, char operacion){
    return rand() % (opB + 1);
}

int AND (mv* MV, int opA, int opB, char operacion){
    opA= opA & opB;
    CambiaCC(MV,opA);
    return opA;
}

int OR (mv* MV, int opA, int opB, char operacion){
    opA= opA | opB;
    CambiaCC(MV,opA);
    return opA;
}

int XOR (mv* MV, int opA, int opB, char operacion){
    opA= opA ^ opB;
    CambiaCC(MV,opA);
    return opA;
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

int NOT (mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3;
    int aux;
    if (tipoA==0)
      printf ("Error,operando NULO");
    else{
      opA= ~opA;
      CambiaCC(MV,opA);
    }
    return opA;
}

void CambiaCC (mv* MV, int opA){
    if (opA & 0x8000){ //da un valor
        (*MV).registros[8] |= 0x8000; // Si el If da 1, es negativo, Activo N (bit 15)
        (*MV).registros[8] &= ~0x4000; // Apaga bit Z (bit 14)
    }
    else{
        if(opA==0)
            (*MV).registros[8] |= 0x4000; // Si el If da 1, es cero, Activo Z (bit 14)
            (*MV).registros[8] &= ~0x8000;// Apaga bit N (bit 15)
    }
}

int SHL (mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3;
    int valorB = ValoropST((operacion>>6) & 0x3, opB, MV);
    if (tipoA==0 || tipoA==2)
      printf ("Error,operando INMEDIATO o NULO");
    else
      if (tipoA==1){
        //set(MV, opA, tipoA, ValoropST(tipoA, opA, MV) << ValoropST(operacion>>6, opB, MV)); por las dudas no
        (*MV).registros[opA] = (*MV).registros[opA]<< valorB;
        CambiaCC(MV,(*MV).registros[opA]);
      }
      else{ //tipoA==3
        (*MV).memoria[opA]= (*MV).memoria[opA]<< valorB;
        CambiaCC(MV,(*MV).memoria[opA]);
      }  
}

int SHR (mv* MV, int opA, int opB, char operacion){
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
        CambiaCC(MV,(*MV).registros[opA]);
      }
      else{ //tipoA==3
        (*MV).memoria[opA]= (*MV).memoria[opA]>> valorB;
        if (opA & 0x80000000){
            mascara = ((0xFFFFFFFF) << (32 - valorB)) & 0xFFFFFFFF;
            valorB|= mascara;
      }
    CambiaCC(MV,(*MV).memoria[opA]);
    }
}

void CMP (mv* MV, int opA, int opB, char operacion){
    char tipoA=(operacion >> 4) & 0x3;
    char tipoB=(operacion>>6) & 0x3;
    int valorA=ValoropST(tipoA, opA, MV);
    int valorB=ValoropST(tipoB, opB, MV);
    CambiaCC(MV, valorA-valorB);
    /*
    (*MV).registros[8] &= 0x0000;
    if(valorA == valorB) //Activa el Z
        (*MV).registros[8] |= 0x4000;   //Las mascaras estan bien?
    else
        if(valorA < valorB) //Activa el N
            (*MV).registros[8] |= 0x8000;
    //Sino no pasa nada por que el CC ya esta en 0 */
}