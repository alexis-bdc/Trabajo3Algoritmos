/*
        Trabajo 3 Algoritmos Distribuidos
        Alumnos:    Alexis Bolados
                    Florencia Cespedes
        Profesor:   Ruben Carvajal Schiaffino
        Entrega:    4 Diciembre 2022

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
#include <pthread.h>
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
    int rows;
    int cols;
    int **data;
}matrix;



//variables globales






//funciones

void printMatrix(matrix m, int mode, int rank){
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
void SumMatrix(matrix mA, matrix mB, int mode, int rank){
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






//main

void main (int argc, char *argv[]) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //recibe particion de matrices
    int partition = 0;
    if(argv[4] == 'H'){
        partition = HORIZONTAL;
    }else if(argv[4] == 'V'){
        partition = VERTICAL;
    }

    //recibe operacion a realizar
    int operation = 0;
    if(argv[3] == 'S'){
        operation = SUMA;
    }else if(argv[3] == 'M'){
        operation = MULTIPLICACION;
    }

    //recibe modo de ejecucion
    int mode = 0;
    if(argv[2] == 'S'){
        mode = SILENT;
    }else if(argv[2] == 'V'){
        mode = VERBOSE;
    }

    //recibe file from bash arguments
    fp = fopen(argv[1], "r");

    
    if (fp == NULL)
        exit(EXIT_FAILURE);



    //recibe dimensiones de matrices
    int rowsA;
    int colsA;
    int rowsB;
    int colsB;


}