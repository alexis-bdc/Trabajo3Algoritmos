/******************************************************************************
 * 
 * FILE: dquadsolution.c
 * 
 * DESCRIPTION: Distributed solution of a cuadratic equation.
 *
 * AUTHOR: Ruben Carvajal Schiaffino
 *
 * LAST REVISED: Santiago de Chile, 21/12/2015
 *
 *****************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"


#define TAG1   1
#define TAG2   2
#define MASTER 0
#define X1 1
#define X2 2


int whoami, hm_are_we; 
MPI_Status status;


/*
 * 
 * Shows the usage of the program
 * 
 */
void Usage(char *message) {
  
   printf("\nUsage : mpirun -np 3 %s a b c\n",message);
   fflush(stdout);
}


/*
 *
 */
void main( int argc, char *argv[]) {
  
   char processor_name[MPI_MAX_PROCESSOR_NAME];
   int me;
   float coef[3], delta, x, x1, x2;

   
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&hm_are_we);
   MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
   MPI_Get_processor_name(processor_name,&me);
   printf("\n\nProcess [%d] Alive on %s\n",whoami,processor_name);
   fflush(stdout);
   if (whoami == MASTER)  
      if (argc == 4) {
         coef[0] = atof(argv[1]);
	 coef[1] = atof(argv[2]);
	 coef[2] = atof(argv[3]);
         MPI_Send(&coef,3,MPI_FLOAT,X1,TAG1,MPI_COMM_WORLD);
	 MPI_Send(&coef,3,MPI_FLOAT,X2,TAG1,MPI_COMM_WORLD);
	 MPI_Recv(&x1,1,MPI_FLOAT,X1,TAG2,MPI_COMM_WORLD,&status);
	 MPI_Recv(&x2,1,MPI_FLOAT,X2,TAG2,MPI_COMM_WORLD,&status);
	 printf("\n\nReal Solutions of %fx^2 + %fx + %f = 0 are x1 = %f x2 = %f\n\n",coef[0],coef[1],coef[2],x1,x2);
	 fflush(stdout);
      }    
      else
         Usage(argv[0]);
   else {
      MPI_Recv(&coef,3,MPI_FLOAT,MASTER,TAG1,MPI_COMM_WORLD,&status);
      delta = sqrt(coef[1] * coef[1] - 4.0 * coef[0] * coef[2]);
      if (whoami == X1)
	 x = (-coef[1] + delta) / (2.0 * coef[0]);
      else
         x = (-coef[1] - delta) / (2.0 * coef[0]);
      MPI_Send(&x,1,MPI_FLOAT,MASTER,TAG2,MPI_COMM_WORLD);
   }
   MPI_Finalize();      
}
