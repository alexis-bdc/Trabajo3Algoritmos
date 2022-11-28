/*
 * Program Name: neighbour.c
 *
 * AUTHOR: Ruben Carvajal Schiaffino
 *
 * LAST UPDATE: Santiago de Chile, 1/11/2005
 *
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "time.h"

#define MPI_data1  MPI_UNSIGNED_CHAR                        /* unsigned char */ 
#define MPI_data2  MPI_UNSIGNED                             /* unsigned int */

#define M1_TAG 1


#define MASTER_NODE   0

typedef unsigned char data1;

MPI_Status status;
       

/*
 *
 */
void ToRightNeighbour(int me, int n)
{
  int dest;
  
  dest = (me + 1) % n;
  MPI_Send(&me,1,MPI_INT,dest,M1_TAG,MPI_COMM_WORLD);  
}


/*
 *
 */
void FromLeftNeighbour(int me) 
{
  int message;
  
  MPI_Recv(&message,1,MPI_INT,MPI_ANY_SOURCE,M1_TAG,MPI_COMM_WORLD,&status);
  printf("[%d] message from %d = %d\n",me,status.MPI_SOURCE,message);
}  

      
/*
 *
 */
void main(int argc, char *argv[])
{
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int me, allwe, whoami;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&allwe);
  MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
  MPI_Get_processor_name(processor_name,&me);
  printf("Process [%d] Alive on %s\n",whoami,processor_name);
  fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);
  ToRightNeighbour(whoami,allwe);
  FromLeftNeighbour(whoami);
  MPI_Finalize();            
} 
