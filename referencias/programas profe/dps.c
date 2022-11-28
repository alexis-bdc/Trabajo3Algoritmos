/*
 * Program Name: dps.c
 *               cada proceso genera n numeros al azar en el rango 1..max y luego determina la 
 *               frecuencia de x en paralelo
 *
 * AUTHOR: Ruben Carvajal Schiaffino
 *
 * LAST UPDATE: Santiago de Chile, 7/11/2017
 *
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "mpi.h"


#define TAG1 1
#define TAG2 2
#define TAG3 3
#define MASTER 0
#define SILENT 0
#define VERBOSE 1


struct Message {

   int mythid, myprocid, numthreads, xvalue, nvalue, fvalue;
}; 


MPI_Status status;
int *dataset;       
unsigned char modop;


/*
 * 
 */
void Usage(char *arg) {

   printf("\nUsage: %s n max x t [-V | -S]\n",arg);
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
      if (modop == VERBOSE) {
         printf("From %d %d Value = %d\n",me,i,dataset[i]);
         fflush(stdout);
      }
   }     
}


/*
 *
 */
void *Count(void *s) {

   int i, f, x, size;
   struct Message *m;

   m = (struct Message *) s;
   x = m->xvalue;
   size = m->nvalue / m->numthreads;
   f = 0;
   for (i = m->mythid * size; i < size; i = i + 1)
      if (dataset[i] == x)
         f = f + 1;
   if (modop == VERBOSE) {
      printf("From (%d , %d) x = %d appears %d times\n",m->myprocid,m->mythid,x,f);
      fflush(stdout);
   }
   m->fvalue = f;
   pthread_exit((void *) m);
}  

      
/*
 *
 */
int main(int argc, char *argv[]) {

   char processor_name[MPI_MAX_PROCESSOR_NAME];
   unsigned int me, allwe, whoami, n, max, *data, i, x, ok, freq, t;
   float E_cpu;
   clock_t cs, ce;
   long E_wall;
   time_t ts, te;
   pthread_t *thread;
   pthread_attr_t attribute;
   struct Message **m;
   void *exit_status;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&allwe);
   MPI_Comm_rank(MPI_COMM_WORLD,&whoami);
   MPI_Get_processor_name(processor_name,&me);
   printf("Process [%d] Alive on %s\n",whoami,processor_name);
   fflush(stdout);
   data = calloc(4,sizeof(unsigned int));
   MPI_Barrier(MPI_COMM_WORLD);
   if (whoami == MASTER) {
      modop = SILENT;
      if (argc == 6) {
         n = atoi(argv[1]);
         max = atoi(argv[2]);
         x = atoi(argv[3]);
         t = atoi(argv[4]);
         if (strcmp(argv[5],"-V") == 0)
            modop = VERBOSE;
         if (strcmp(argv[5],"-S") == 0)
            modop = SILENT;
         thread = calloc(t,sizeof(pthread_t));
         m = calloc(t,sizeof(struct Message *));
         for (i = 0; i < t; i = i + 1)
            m[i] = calloc(1,sizeof(struct Message));
         data[0] = n;
         data[1] = max;
         data[2] = t;
	 data[3] = modop;
         for (i = 1; i < allwe; i = i + 1)
            MPI_Send(data,4,MPI_UNSIGNED,i,TAG1,MPI_COMM_WORLD);
         GenData(n,max,whoami);
         for (i = 1; i < allwe; i = i + 1) {
            MPI_Recv(&ok,1,MPI_UNSIGNED,MPI_ANY_SOURCE,TAG2,MPI_COMM_WORLD,&status);
	    if (modop == VERBOSE) {
               printf("Master ok = %d From %d\n",ok,status.MPI_SOURCE);
               fflush(stdout);
	    }
         }
         for (i = 1; i < allwe; i = i + 1)
            MPI_Send(&x,1,MPI_UNSIGNED,i,TAG3,MPI_COMM_WORLD);
	 ts = time(NULL);
         cs = clock();   
         pthread_attr_init(&attribute);
         pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
         for (i = 0; i < t; i = i + 1) {
	    if (modop == VERBOSE) {
               printf("Master: creating thread %d\n", i);
               fflush(stdout);
	    }
            m[i]->mythid = i;
            m[i]->myprocid = whoami;
            m[i]->numthreads = t;
            m[i]->nvalue = n;
            m[i]->xvalue = x;
            pthread_create(&thread[i],&attribute,Count,(void *) m[i]);
         }
         pthread_attr_destroy(&attribute); 
         for (i = 0, freq = 0; i < t; i = i + 1) {
            pthread_join(thread[i],&exit_status);
            m[i] = (struct Message *) exit_status;
	    if (modop == VERBOSE) {
               printf("Master %d.- Result Received From Thread %d\n",i+1,m[i]->mythid);
               fflush(stdout);
	    }
            freq = freq + m[i]->fvalue;        
         }
         printf("%d appears %d times on %d\n",x,freq,whoami);
         fflush(stdout);
         for (i = 1; i < allwe; i = i + 1) {
            MPI_Recv(&freq,1,MPI_UNSIGNED,MPI_ANY_SOURCE,TAG3,MPI_COMM_WORLD,&status);
            printf("%d appears %d times on %d\n",x,freq,status.MPI_SOURCE);
            fflush(stdout);     
         }
         ce = clock();
         te = time(NULL);
         E_wall = (long) (te - ts);
         E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC;      
         printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu,E_wall);
	 fflush(stdout);
      }
      else
         Usage(argv[0]);
   }
   else {
      MPI_Recv(data,4,MPI_UNSIGNED,MASTER,TAG1,MPI_COMM_WORLD,&status);
      n = data[0];
      max = data[1];
      t = data[2];
      modop = data[3];
      thread = calloc(t,sizeof(pthread_t));
      m = calloc(t,sizeof(struct Message *));
      for (i = 0; i < t; i = i + 1)
         m[i] = calloc(1,sizeof(struct Message));
      GenData(n,max,whoami);
      ok = 1;  
      MPI_Send(&ok,1,MPI_UNSIGNED,MASTER,TAG2,MPI_COMM_WORLD);
      MPI_Recv(&x,1,MPI_UNSIGNED,MASTER,TAG3,MPI_COMM_WORLD,&status);
      pthread_attr_init(&attribute);
      pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
      for (i = 0; i < t; i = i + 1) {
	 if (modop == VERBOSE) {
            printf("From %d: creating thread %d\n",whoami,i);
            fflush(stdout);
	 }
         m[i]->mythid = i;
         m[i]->myprocid = whoami;
         m[i]->numthreads = t;
         m[i]->nvalue = n;
         m[i]->xvalue = x;
         pthread_create(&thread[i],&attribute,Count,(void *) m[i]);
      }
      pthread_attr_destroy(&attribute); 
      for (i = 0, freq = 0; i < t; i = i + 1) {
         pthread_join(thread[i],&exit_status);
         m[i] = (struct Message *) exit_status;
	 if (modop == VERBOSE) {
            printf("From %d.- %d.- Result Received From Thread %d\n",whoami,i+1,m[i]->mythid);
            fflush(stdout);
	 }
         freq = freq + m[i]->fvalue;        
      }
      MPI_Send(&freq,1,MPI_UNSIGNED,MASTER,TAG3,MPI_COMM_WORLD);
   } 
   MPI_Finalize();
   return 0;            
} 
