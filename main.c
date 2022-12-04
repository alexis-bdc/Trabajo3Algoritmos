/*
        Trabajo 3 Algoritmos Distribuidos
        Alumnos:        Alexis Bolados
                        Florencia Cespedes
        Profesor:       Ruben Carvajal Schiaffino
        Entrega:        4 Diciembre 2022
        compilacion:    mpicc main.c -o t3.exe
        Ejecucion:      mpirun -np K ./t3.exe -p -o -m < data.txt
                        K: numero de nodos | numero de procesos
                        p: particion de matrices | como se reparten desde el archivo {H,V}
                        o: operacion a realizar {S,M}
                        m: modo de ejecucion | silent or verbose {S,V}

        Requerimientos:
                        algoritmos distribuido en memora privada, para operar matrices mapeadas en nodos
                        operaciones: [SUMA][], [MULTIPLICACION][]
*/

//algoritmos distribuido que opera matrices


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

#define SUMA 0
#define MULTIPLICACION 1

#define HORIZONTAL 0
#define VERTICAL 1

#define SILENT 0
#define VERBOSE 1

//structs

typedef struct {
    int nRows;
    int nCols;
    int** data;
}MATRIX;




//global vars
int partition;
int operation;
int mode;
FILE *input;
MATRIX * matrixA;
MATRIX * matrixB;
MATRIX * matrixAux;


//funciones

// Initiate new zero MATRIX 
MATRIX* newMatrix(int nRows, int nCols) {
    MATRIX* matrix = (MATRIX*) malloc(sizeof(MATRIX));
    matrix->nRows = nRows;
    matrix->nCols = nCols;
    matrix->data = (int**) malloc(nRows * sizeof(int*));
    for (int i = 0; i < nRows; i++) {
        matrix->data[i] = (int*) calloc(nCols, sizeof(int));
    }
    return matrix;
}

void printMatrix(MATRIX * m, int rank){
    printf("matrix rows: %d, matrix cols: %d\n", m->nRows, m->nCols);
    if(mode == VERBOSE || rank == MASTER){
        for(int i = 0; i < m->nRows; i++){
            for(int j = 0; j < m->nCols; j++){
                printf("%d ", m->data[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}


//to review
// void SumMatrixMaster(MATRIX mA, MATRIX mB, int rank){
//     MATRIX result;
//     if(mode == VERBOSE || rank == MASTER){
//         for(int i = 0; i < mA.rows; i++){
//             for(int j = 0; j < mA.cols; j++){
//                 result.data[i][j] = mA.data[i][j] + mB.data[i][j];
//             }
//         }
//         printMatrix(result, mode, rank);
//     }
//     else if(mode == SILENT){
//         for(int i = 0; i < mA.rows; i++){
//             for(int j = 0; j < mA.cols; j++){
//                 result.data[i][j] = mA.data[i][j] + mB.data[i][j];
//             }
//         }
//         return result;
//     }
// }



// master and slave code
void masterCode (int me, int allwe, int whoami){
        
    //create MATRIX A and get number of rows and cols
    int matrixArows = fgetc(input);
    int matrixAcolums = fgetc(input);
    matrixA = newMatrix(matrixArows,matrixAcolums);


    //create MATRIX B and get number of rows and cols
    int matrixBrows = fgetc(input);
    int matrixBcolums = fgetc(input);
    matrixB = newMatrix(matrixBrows,matrixBcolums);

    //get data from file to MATRIX A
    for (int i = 0; i < matrixArows; i++){
        for (int j = 0; j < matrixAcolums; j++){
            matrixA->data[i][j] = fgetc(input);
        }
    }

    //get data from file to MATRIX B
    for (int i = 0; i < matrixBrows; i++){
        for (int j = 0; j < matrixBcolums; j++){
            matrixB->data[i][j] = fgetc(input);
        }
    }

    if (mode == VERBOSE){
        //print MATRIX A
        printf("Matrix A initialize as:\n");
        printMatrix(matrixA, MASTER);
        //print MATRIX B
        printf("Matrix B initialize as:\n");
        printMatrix(matrixB, MASTER);
    }


    //send data to slaves
    if (operation == SUMA){}

    else if (operation == MULTIPLICACION){}

    else{
        printf("operation not valid\n");
    }

    // MPI_Send();


    //receive data from slaves



    //print final matrix
  
    if (mode == VERBOSE){
        printf("Matrix result:\n");
        printMatrix(matrixA, MASTER);
    }
    else if (mode == SILENT){
        printMatrix(matrixA, MASTER);
    }
    else{
        printf("mode not valid\n");
    }

}


void slaveCode(int me, int allwe, int whoami){
    //recibe data from master
    // MPI_Recv();
    
    //do operation
    switch (operation)
    {
        case SUMA:
            
        break;
        case MULTIPLICACION:

        break;
    }


    //send data to master
}



//main
void main (int argc, char *argv[]) {
    
    input = stdin;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int me, allwe, whoami;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&allwe);
    MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
    MPI_Get_processor_name(processor_name,&me);
    printf("Process [%d] Alive on %s\n",whoami,processor_name);


    //recibe tipo particion de matrices
    if(argv[4] == 'H'){
        partition = HORIZONTAL;
    }else if(argv[4] == 'V'){
        partition = VERTICAL;
    }

    //recibe operacion a realizar
    if(argv[3] == 'S'){
        operation = SUMA;
    }else if(argv[3] == 'M'){
        operation = MULTIPLICACION;
    }

    //recibe modo de ejecucion
    if(argv[2] == 'S'){
        mode = SILENT;
    }else if(argv[2] == 'V'){
        mode = VERBOSE;
    }

    if (whoami == MASTER) {
        masterCode(me, allwe, whoami);    
    }

    else {
        slaveCode(me, allwe, whoami);
    }

    MPI_Finalize();
}