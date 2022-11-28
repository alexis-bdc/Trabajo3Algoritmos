/*
 * Program Name: distsearch.c
 *               cada proceso genera n numeros al azar en el rango 1..max y luego determina la 
 *               frecuencia de x
 *
 * AUTHOR: Ruben Carvajal Schiaffino
 *
 * LAST UPDATE: Santiago de Chile, 26/11/2013
 *
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define TAG1 1
#define TAG2 2
#define TAG3 3
#define MASTER 0

MPI_Status status;
int *dataset;       


/*
 * 
 */
void Usage(char *arg) {

   printf("\nUsage: %s n max x\n",arg);
   fflush(stdout);
}


/*
 *
 */
void GenData(int n, int max, int me) {

   int i;

   dataset = calloc(n,sizeof(int));
   srand(time(NULL) + me);
   for (i = 0; i < n; i = i + 1) { 
      dataset[i] = (rand() % max) + 1;
      printf("From %d %d Value = %d\n",me,i,dataset[i]);
      fflush(stdout);
   }     
}


/*
 *
 */
int Count(int x, int n) {

   int i, f;

   f = 0;
   for (i = 0; i < n; i = i + 1)
      if (dataset[i] == x)
         f = f + 1;
   return f;
}  

      
/*
 *
 */
void main(int argc, char *argv[]) {

   char processor_name[MPI_MAX_PROCESSOR_NAME];
   int me, allwe, whoami, n, max, *data, i, x, ok, freq;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&allwe);
   MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
   MPI_Get_processor_name(processor_name,&me);
   printf("Process [%d] Alive on %s\n",whoami,processor_name);
   fflush(stdout);
   data = calloc(2,sizeof(int));
   MPI_Barrier(MPI_COMM_WORLD);
   if (whoami == MASTER) {
      if (argc == 4) {
         n = atoi(argv[1]);
         max = atoi(argv[2]);
         x = atoi(argv[3]);
         data[0] = n;
         data[1] = max;
         for (i = 1; i < allwe; i = i + 1)
            MPI_Send(data,2,MPI_INT,i,TAG1,MPI_COMM_WORLD);
         GenData(n,max,whoami);
         for (i = 1; i < allwe; i = i + 1) {
            MPI_Recv(&ok,1,MPI_INT,MPI_ANY_SOURCE,TAG2,MPI_COMM_WORLD,&status);
            printf("MASTER ok = %d From %d\n",ok,status.MPI_SOURCE);
            fflush(stdout);      
         }
         for (i = 1; i < allwe; i = i + 1)
            MPI_Send(&x,1,MPI_INT,i,TAG3,MPI_COMM_WORLD);
         freq = Count(x,n);
         printf("%d appears %d times on %d\n",x,freq,whoami);
         fflush(stdout);
         for (i = 1; i < allwe; i = i + 1) {
            MPI_Recv(&freq,1,MPI_INT,MPI_ANY_SOURCE,TAG3,MPI_COMM_WORLD,&status);
            printf("%d appears %d times on %d\n",x,freq,status.MPI_SOURCE);
            fflush(stdout);     
         }
      }
      else
         Usage(argv[0]);
   }
   else {
      MPI_Recv(data,2,MPI_INT,MASTER,TAG1,MPI_COMM_WORLD,&status);
      n = data[0];
      max = data[1];
      GenData(n,max,whoami);
      ok = 1;  
      MPI_Send(&ok,1,MPI_INT,MASTER,TAG2,MPI_COMM_WORLD);
      MPI_Recv(&x,1,MPI_INT,MASTER,TAG3,MPI_COMM_WORLD,&status);
      freq = Count(x,n);
      MPI_Send(&freq,1,MPI_INT,MASTER,TAG3,MPI_COMM_WORLD);
   } 
   MPI_Finalize();            
} 
