#include<malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#define USE MPI
#define SEED 35791246

	
unsigned int * leggi() {
  unsigned int ** vett ;
  FILE *fd;
  unsigned int numero;
  
  /* allocazione */
  vett =(unsigned int **) malloc(sizeof(unsigned int));
  
  /* apre il file */
  fd=fopen("numero.txt", "r"); 
  
  /* verifica errori in apertura */
  if( fd==NULL ) {
    perror("Errore in apertura del file");
    exit(1);
  }
  
  /* legge l'array */		
  numero=fscanf(fd, "%i", &vett);  
  		
  
  /* chiude il file */
  fclose(fd);
  printf ("%i", vett);
  return 0;
  }

unsigned int crea(unsigned int numero){
	  array =(unsigned int *) malloc(numero*sizeof(unsigned int));
	for (int i= 0; i<numero;i++){
		array[i]= i;
	}
}

unsigned int somma(unsigned int arr){
	unsigned int somma;
	if (numero % processori == 0){
		for (int i = (ris*processore); i<(ris*(processore+1)); i++ ){
			somma = arr[i]+somma;
		}
	}
	else{
		//master only
		for (int i = (ris*processore); i<(ris*(processore+1)); i++ ){
			somma = arr[i]+somma;
		}
		for (int i = ris; i< ris+resto; i++)
			somma = arr[i]+ somma;
	}//????
}

int main ( int argc , char *argv[ ] )
{
  // times 
  double start_time, end_time;   
  int myid , numprocs , proc ;
  MPI_Status status;
  MPI_Request request;
  // master process
  int master = 0;
 // int tag = 123;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);

  if ( argc <=1) {
    fprintf (stderr , " Usage : mpi -np n %s number_of_iterations \n", argv[0] ) ;
    MPI_Finalize() ;
    exit(-1) ;
  }

  long long int N = atoll(argv[1])/numprocs;
// take time of processors after initial I/O operation
  start_time = MPI_Wtime();

  // initialize random numbers 
  if (myid ==0) { //if I am the master process gather results from others
    M = local_M ;
    for (proc=1; proc<numprocs ; proc++) {
      MPI_Recv(&local_M,1,MPI_LONG_LONG,proc,tag,MPI_COMM_WORLD,&status ) ;

      M += local_M ;
      printf("%lli", M);
    }
    end_time=MPI_Wtime();
    printf ( "\n # walltime on master processor : %10.8f \n", end_time - start_time ) ;
  }
  
  else {   // for all the slave processes send results to the master /
    MPI_Ssend(&local_M , 1 ,MPI_LONG_LONG, master , tag ,MPI_COMM_WORLD) ;
    end_time=MPI_Wtime();
    printf ( "\n # walltime on processor %i : %10.8f \n",myid, end_time - start_time ) ;
  }

  MPI_Finalize() ; // let MPI finish up /

}
