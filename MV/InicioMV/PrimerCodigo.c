#include <stdio.h>
#include <stdlib.h>


typedef struct{
   short int Base;
   short int Tamanio;
}TablaS;
 

typedef struct{
    char memoria[16384];
    int registros[16];
    TablaS TSeg[2];} mv;

void lectura(mv*);
void CargaRegistros(mv*, int);

void SYS(mv*);
void JMP(mv*);
void JZ(mv* );
void JP(mv* );
void JZ(mv* );
void JNZ(mv*);
void JNP(mv* );
void JNN(mv* );
void NOT(mv* );
void EMPTY(mv*);
void STOP(mv* );
void MOV(mv* );
void ADD(mv* );
void SUB(mv* );
void SWAP(mv*);
void MUL(mv* );
void DIV(mv* );
void CMP(mv* );
void SHL(mv* );
void SHR(mv* );
void AND(mv* );
void OR(mv* );
void XOR(mv* );
void LDL(mv* );
void LDH(mv*,int, int );
void RND(mv*,int, int );

void LeeCS(mv*, void**); //Chequear esto del doble puntero //////////////////////////////////

//void REGISTRO(int * );
void INMEDIATO(int * );
void MEMORIA(int * );
void NULO(int * );

int main(){
    void (*Funciones[32])(mv*, int, int) = {SYS,JMP,JZ,JP,JZ,JNZ,JNP,JNN,NOT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,STOP,MOV,ADD,SUB,SWAP,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,LDL,LDH,RND,EMPTY}; //Las funciones del vector deben recibir los mismos parametros

    mv MV; //Variable maquina virtual
    int tamCS;
    lectura (&MV); //Llamada a la funcion lectura
    LeeCS(&MV, Funciones);


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


void LeeCS (mv *MV, void (Funciones[])(mv*, int, int)){
    void (*FuncOperandos[4])(mv, int *, int *) = {NULO,REGISTRO,INMEDIATO,MEMORIA};
    char instruccion;
    int opA,opB,operacion,IPaux, cont=0;
    IPaux=(*MV).registros[5];
    while ((*MV).registros[5]<=(*MV).TSeg[0].Tamanio){  //MV.REGISTROS 5 = IP
      instruccion = (*MV).memoria[IPaux]; //levanto el dato del CS apuntado por IP
      opB=(instruccion&0xC0)>>6;    //4 valores, 2 bits
      opA=(instruccion&0x30)>>4;
      operacion=(instruccion &0x1F);

      FuncOperandos[opB](&cont);
      //Suma al contador 1
      //Saca los bytes de la memoria de la posicion [Mv.registro[5] + cont]
      
      IPaux= IPaux + cont;
      FuncOperandos[opA](&cont);
      IPaux= IPaux + cont;
      Funciones[operacion](MV, opA, opB); //Llama a la funcion correspondiente
      
      (*MV).registros[5]=IPaux; //Graba en el IP, la ultima instruccion leida
    
    }

}

void REGISTRO (mv MV,int *cont, int *op){
    char Sacamemoria;
    *cont+=1;
    Sacamemoria= MV.registros[5] + cont;
       
    
}

void NULO (int *op){
    *op = -1;
}

void EMPTY(mv* MV){
    //No hace nada
}



