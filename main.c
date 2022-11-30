/*
        Trabajo 3 Algoritmos Distribuidos
        Alumnos:    Alexis Bolados
                    Florencia Cespedes
        Profesor:   Ruben Carvajal Schiaffino
        Entrega:    4 Diciembre 2022
        compilacion
        Ejecucion: mpirun -np K ./t3.exe -p -o -m < data.txt
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
int modo;
int partition;
int operation;
MATRIX matrixA;
MATRIX matrixB;




//funciones

MATRIX * newMatrix(int rows, int cols){       //create new zero matrix
    if (rows <= 0 || cols <= 0) return NULL;

    // allocate a matrix structure
    matrix * m = (matrix *) malloc(sizeof(matrix));

    // set dimensions
    m->nRows = rows;
    m->nCols = cols;

    // allocate a int array of length rows * cols
    m->data = (int *) malloc(rows*cols*sizeof(int));
    // set all data to 0
    int i;
    for (i = 0; i < rows*cols; i++)
        m->data[i] = 0;
    return m;
}



void printMatrix(matrix m, int rank){
    if(mode == VERBOSE || rank == MASTER){
        for(int i = 0; i < m.rows; i++){
            for(int j = 0; j < m.cols; j++){
                printf("%d ", m.data[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}


//to review
void SumMatrixMaster(matrix mA, matrix mB, int rank){
    matrix result;
    if(mode == VERBOSE || rank == MASTER){
        for(int i = 0; i < mA.rows; i++){
            for(int j = 0; j < mA.cols; j++){
                result.data[i][j] = mA.data[i][j] + mB.data[i][j];
            }
        }
        printMatrix(result, mode, rank);
    }
    else if(mode == SILENT){
        for(int i = 0; i < mA.rows; i++){
            for(int j = 0; j < mA.cols; j++){
                result.data[i][j] = mA.data[i][j] + mB.data[i][j];
            }
        }
        return result;
    }
}

// master and slave code
void masterCode (FILE *file){
        
    //create matrix A and get number of rows and cols
    matrixArows = fgetc(input);
    matrixAcolums = fgetc(input);
    matrixA = newMatrix(matrixArows,matrixAcolums);


    //create matrix B and get number of rows and cols
    matrixBrows = fgetc(input);
    matrixBcolums = fgetc(input);
    matrixB = newMatrix(matrixBrows,matrixBcolums);

    //get data from file to matrix A
    for (i = 0; i< matrixArows; i++){
        for (j = 0; j < matrixAcolums; j++){
            matrixA.data[i][j] = fgetc(input);
        }
    }

    //get data from file to matrix B
    for (i = 0; i< matrixBrows; i++){
        for (j = 0; j < matrixBcolums; j++){
            matrixB.data[i][j] = fgetc(input);
        }
    }

    

    //send data to slaves
    //receive data from slaves
    //print data
    


}


void slaveCode(){
    //recibe data from master
    
    
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
    
    FILE *input;
    input = stdin;


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



    // if (whoami == MASTER) {
    //     //master code
    //     masterCode(input);    
    // }

    // else {
    //     //slave code
    //     slaveCode();
    // }
}