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
#include <string.h>
#include <mpi.h>

#define MASTER 0
#define TAG 1

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
MPI_Status status;

int matrixArows, matrixAcolums, matrixBrows, matrixBcolums;

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

void sumaVert (int col, int rank){
    int i;
    int *buffer = (int*) malloc(matrixA->nRows * sizeof(int));
    for (i = 0; i < matrixA->nRows; i++){
        buffer[i] = matrixA->data[i][col] + matrixB->data[i][col];
    }
    MPI_Send(&buffer, matrixA->nRows, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
}

void sumaHoriz (int row, int rank){
    int i;
    int *buffer = (int*) malloc(matrixA->nCols * sizeof(int));
    for (i = 0; i < matrixA->nCols; i++){
        buffer[i] = matrixA->data[row][i] + matrixB->data[row][i];
    }
    MPI_Send(&buffer, matrixA->nCols, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
}

void multVert (int col, int rank){
    

}

void multHoriz (int row, int rank){
   

}





// master and slave code
void masterCode (int me, int allwe, int whoami){
    int slaves = allwe - 1;

    //initiate MATRIX auxiliar to return result
    if (operation == SUMA){
        matrixAux = newMatrix(matrixArows,matrixAcolums);
    }
    else if (operation == MULTIPLICACION){
        matrixAux = newMatrix(matrixArows,matrixBcolums);
    }


    if (mode == VERBOSE){
        //print MATRIX A
        printf("Matrix A initialize as:\n");
        printMatrix(matrixA, MASTER);
        //print MATRIX B
        printf("Matrix B initialize as:\n");
        printMatrix(matrixB, MASTER);
    }


    //receive data from slaves
    if (operation == SUMA && matrixA->nCols == matrixB->nCols && matrixA->nRows == matrixB->nRows){
        int *buffer = (int*) malloc(matrixA->nRows * sizeof(int));
        int indexs[allwe-1];
        printf("initiate SUMA\n");
        
        // how many rows do every slave have to work with
        int i = 0;
        for (i; i < matrixAcolums%slaves ; i++){ indexs[i] = matrixA->nCols/slaves+1; }
        for (i; i <= slaves-1 ; i++) {indexs[i] = matrixA->nCols/slaves; }

        // print indexs
        // if (mode == VERBOSE){
        //     printf("indexs: ");
        //     for (int i = 0; i < slaves; i++){
        //         printf("%d ", indexs[i]);
        //     }
        //     printf("\n");
        // }

        //send data to slaves

        int index_pos = 0;
        for (int i = 1; i <= slaves; i++){
            //printf("call slave %d\n", i);
            for (int j = 0; j < indexs[i]; j++){
                MPI_Send(&index_pos, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
                index_pos++;
            }
        }

        printf("muere\n");

        //receive data from slaves
        for (int i = 1; i < allwe; i++){
            if (partition == HORIZONTAL){
                MPI_Recv(&buffer, matrixA->nCols, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
                int index = i - 1;
                for (int j = 0; j < matrixA->nCols; j++){
                    matrixAux->data[index][j] = buffer[j];
                }
            }
            else if (partition == VERTICAL){
                MPI_Recv(&buffer, matrixA->nRows, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
                int index = i - 1;
                for (int j = 0; j < matrixA->nRows; j++){
                    matrixAux->data[j][index] = buffer[j];
                }
            }
        }



        
        free(buffer);        
    } 
    else { printf("Matrices no compatibles para la suma\n"); }



    if (operation == MULTIPLICACION && matrixAcolums == matrixBrows){
        for (int i = 1; i < allwe; i++ ){
            
        }

    }

    else { printf("Matrices no compatibles para la multiplicacion\n"); }



    //print final matrix
  
    if (mode == VERBOSE){
        printf("Matrix result:\n");
        printMatrix(matrixAux, whoami);
    }
    else if (mode == SILENT){
        printMatrix(matrixAux, whoami);
    }

}


void slaveCode(int me, int allwe, int whoami){
    // recibe data from master
    // data column/row to operate
    int index;
    MPI_Recv(&index, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);

    printf("slave %d recibe index: %d\n", me, index);
    //do operation
    
    if (operation == SUMA){
        if (partition == VERTICAL){
            printf("slave %d recibe column index: %d\n", whoami, index);
            sumaVert(index, whoami);
        }
        else if (partition == HORIZONTAL){
            sumaHoriz(index, whoami);
        }
    }

    else if (operation == MULTIPLICACION){
        if (partition == VERTICAL){
            
        }
        else if (partition == HORIZONTAL){
            
        }
    }



}



//main
void main (int argc, char *argv[]) {
    
    //input = stdin;
    
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int me, allwe, whoami;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&allwe);
    MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
    MPI_Get_processor_name(processor_name,&me);
    printf("Process [%d] Alive on %s\n",whoami,processor_name);

    //argv[] = {partition, operation, mode}
    if (argc == 4){
        if (strcmp(argv[1], "V") == 0){
            partition = VERTICAL;
        }
        else if (strcmp(argv[1], "H") == 0){
            partition = HORIZONTAL;
        }
        else{
            printf("partition not valid\n");
        }
        if (strcmp(argv[2], "S") == 0){
            operation = SUMA;
        }
        else if (strcmp(argv[2], "M") == 0){
            operation = MULTIPLICACION;
        }
        else{
            printf("operation not valid\n");
        }
        if (strcmp(argv[3], "V") == 0){
            mode = VERBOSE;
        }
        else if (strcmp(argv[3], "S") == 0){
            mode = SILENT;
        }
        else{
            printf("mode not valid\n");
        }
    }
    else{
        printf("Error: not enough arguments\n");
        exit(1);
    }
    fflush(stdout);
    //print execution partition, operation, mode
    //printf("partition: %d, operation: %d, mode: %d\n", partition, operation, mode);


    //create MATRIX A and get number of rows and cols
    scanf("%i", &matrixArows);
    scanf("%i", &matrixAcolums);
    matrixA = newMatrix(matrixArows,matrixAcolums);
    fflush(stdout);

    //create MATRIX B and get number of rows and cols
    scanf("%i", &matrixBrows);
    scanf("%i", &matrixBcolums);
    matrixB = newMatrix(matrixBrows,matrixBcolums);
    fflush(stdout);

    //get data from file to MATRIX A
    for (int i = 0; i < matrixArows; i++){
        for (int j = 0; j < matrixAcolums; j++){
            scanf("%d",&matrixA->data[i][j]);
            fflush(stdout);
        }
    }

    //get data from file to MATRIX B
    for (int i = 0; i < matrixBrows; i++){
        for (int j = 0; j < matrixBcolums; j++){
            scanf("%d",&matrixB->data[i][j] );
            fflush(stdout);
        }
    }

    if (whoami == MASTER) {
        printf("MASTER\n");
        masterCode(me, allwe, whoami);    
    }
    
    else {
        printf("SLAVE %d\n", whoami);
        slaveCode(me, allwe, whoami);
    }

    MPI_Finalize();
}