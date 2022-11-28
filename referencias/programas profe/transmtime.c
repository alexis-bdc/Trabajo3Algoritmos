/*******************************************************************************
 * 
 * transmtime.c
 *
 * Programmer: Ruben Carvajal Schiaffino
 *
 * Santiago de Chile, 11/11/2022
 *
 ******************************************************************************/ 
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"


#define TAG     1
#define MASTER  0
#define MAX    10


MPI_Status status;
       

/*
 *
 */
void Process(int me, int n) {

   int i, *buf;
   long E_wall;
   time_t  ts, te; 
   
   if (me == MASTER) {
      MPI_Send(&n,1,MPI_INT,1,TAG,MPI_COMM_WORLD);
      buf = calloc(n,sizeof(int));
      ts = time(NULL);
      for (i = 0; i < n; i = i + 1)
         buf[i] = 4500;
      te = time(NULL);
      E_wall = (long) (te - ts);
         printf("Filling Elapsed Wall Time: %ld [Secs]\n\n",E_wall);
         fflush(stdout);
      for (i = 1; i <= MAX; i = i + 1) {
         ts = time(NULL);
         MPI_Send(buf,n,MPI_INT,1,TAG,MPI_COMM_WORLD);
         te = time(NULL);
         E_wall = (long) (te - ts);
         printf("Elapsed Wall Time: %ld [Secs]\n\n",E_wall);
         fflush(stdout);
      }
   }
   else {
      MPI_Recv(&n,1,MPI_INT,0,TAG,MPI_COMM_WORLD,&status);
      buf = calloc(n,sizeof(int));
      for (i = 1; i <= MAX; i = i + 1)
         MPI_Recv(buf,n,MPI_INT,MASTER,TAG,MPI_COMM_WORLD,&status);
   }
   free(buf);
}           

      
/*
 *
 */
int main(int argc, char *argv[]) {

   char processor_name[MPI_MAX_PROCESSOR_NAME];
   int me, allwe, whoami, size;

   

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&allwe);
   MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
   MPI_Get_processor_name(processor_name,&me);
   printf("Process [%d] Alive on %s\n",whoami,processor_name);
   fflush(stdout);
   MPI_Barrier(MPI_COMM_WORLD);
   if (whoami == MASTER)
      size = atoi(argv[1]);
   Process(whoami,size);
   MPI_Finalize();            
} 
