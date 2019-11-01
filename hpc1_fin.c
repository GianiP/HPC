#include<malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#define USE MPI
#define SEED 35791246

int main ( int argc , char *argv[ ] ){
	// variables for mesurng the times 
	double start_time_proce, end_time_proce, start_time_read, end_time_read,start_time_comm, end_time_comm ;   
	// variables for info about the processor
	int myid , numprocs , proc ;
	//about MPI
	MPI_Status status;
	MPI_Request request;
	// master process
	int master = 0;
	unsigned int somma =0;
	unsigned int somma_parziale=0;
	
	//start the parallel part of the program 
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	start_time_proce = MPI_Wtime();
	if ( argc <=1) {  // error management about parallel part
  		fprintf (stderr , " Usage : mpi -np n %s number_of_iterations \n", argv[0] ) ;
  		MPI_Finalize() ; //in case of error close the parallel part and go out to the progam
  		exit(-1) ;
	}
  
	start_time_read=MPI_Wtime();  //start to take the tame for read the number in the file

	// inizialize the variable for read the file
	unsigned int ** vett ;
	FILE *fd; //pointers to file
	unsigned int numero;
  
	/* allocation */
	vett =(unsigned int **) malloc(sizeof(unsigned int));
  
	/* open the file */
	fd=fopen("numero.txt", "r"); //open in read mode the file (in this case my file name is "numero") and read the number inside 
  
	/* error management for file */
	if( fd==NULL ) {
  		perror("Errore in apertura del file");
  		exit(1);
	}

	/* read the the number inside the file and put the numver inside the variavle vett */		
	numero=fscanf(fd, "%i", &vett);  

	/* close the file */
	fclose(fd);
	end_time_read=MPI_Wtime(); //stop the time for read the file
	printf ( "\n # walltime on master processor : %10.8f \n", end_time_read - start_time_read ) ; //print the time for read the file

	//check if the number inside the file is divisibile by the number of processors  
	unsigned int divisibile = vett % numproc; //take the rest 
	if (divisibile != 0){  // if the number is not divisibile by number of processors i give the number divisibile and i will
						   // add the extra numbers to the master processor
		vett = vett - divisibile;
	}
	unsigned int array[(vett/numproc)+divisibile];
	for (int i = (vett/numproc)*proc; i<(vett/numproc)*(proc+1); i++){
		array[i] = i;   //i create the array with the number that the processor have to sum
		somma_parziale = somma_parziale + array[i]; //make the partial sum for that processor
	}

	if (myid ==0) { //if I am the master process gather results from others
		for (int i = vett; i<vett+divisibile; i++){
			somma_parziale = somma_parziale + array[i];
		}
		somma = somma_parziale;
		start_time_comm=MPI_Wtime(); 
    	for (proc=1; proc<numprocs ; proc++) {
    		MPI_Recv(&somma_parziale,1,MPI_LONG_LONG,proc,tag,MPI_COMM_WORLD,&status ) ; //master receive the partial sum by the processors
      		printf("%lli\n", somma_parziale); 
			somma= somma+somma_parziale ;
		}
		end_time_comm=MPI_Wtime(); 
    	printf("%lli\n", somma);  //master processor print the total sum
    	end_time_proce=MPI_Wtime();   // stop the time that rappresent the time for run the program in the master (all program) because the master makes the last operation
    	printf ( "\n # walltime on master processor : %10.8f \n", end_time_proce - start_time_proce ) ;
	}
  
	else {   // for all the slave processes send results to the master /
		MPI_Ssend(&somma_parziale , 1 ,MPI_LONG_LONG, master , tag ,MPI_COMM_WORLD) ; //the processors send to the master processor their partial sum
    	end_time_proce=MPI_Wtime(); //stop the time that rappresent the time for run the program in the processor
		printf ( "\n # walltime on  processor %i : %10.8f \n",&proc, end_time_proce - start_time_proce ) ;
	}
}
