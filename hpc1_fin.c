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

	double start_time_proce, end_time_proce, start_time_read, end_time_read,start_time_comm, end_time_comm ;   	// variables for mesurng the times  
	int myid , numprocs , proc ;  									// variables for info about the processor
	
	//about MPI
	MPI_Status status;
	MPI_Request request;
	
	// master process
	int master = 0;

	int tag = 123;
	long long int somma =0;
	long long int somma_parziale=0;
	
	/////////////////////////////////////////////////////////////////////start the parallel part of the program 
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	start_time_proce = MPI_Wtime();

	if ( argc <=1) { 												 // error management about parallel part
  		fprintf (stderr , " Usage : mpi -np n %s number_of_iterations \n", argv[0] ) ;
  		MPI_Finalize() ; 											//in case of error close the parallel part and go out to the progam
  		exit(-1) ;
	}
	
	start_time_read=MPI_Wtime();  									//start to take the tame for read the number in the file

/*	
	long long int ** vett ;// inizialize the variable for read the file
	FILE *fd; //pointers to file
	long long int numero;
  
	vett =(long long int **) malloc(sizeof(long long int));	// allocation 
  
	// open the file 
	fd=fopen("numero.txt", "r"); //open in read mode the file (in this case my file name is "numero") and read the number inside 
  
	// error management for file 
	if( fd==NULL ) {
  		perror("Errore in apertura del file");
  		exit(1);
	}

	// read the the number inside the file and put the numver inside the variavle vett 		
	numero=fscanf(fd, "%i", &vett);  

	fclose(fd);  	// close the file 
	end_time_read=MPI_Wtime(); //stop the time for read the file
	printf ( "\n # walltime on master processor : %10.8f \n", end_time_read - start_time_read ) ; //print the time for read the file
*/

	long long int vett = atoll(argv[1]);
	end_time_read=MPI_Wtime(); 												//stop the time for read the file
																			//check if the number inside the file is divisibile by the number of processors  
	long long int divisibile = vett % numproc; 								//take the rest 
	
	if (divisibile != 0)  													// if the number is not divisibile by number of processors i give the number 
		vett = vett - divisibile;  											// divisibile and i will add the extra numbers to the master processor
	
	long long int array[(vett/numproc)+divisibile];
	long long int i;
	
	for ( i = (vett/numproc)*proc; i<(vett/numproc)*(proc+1); i++){
		array[i] = i;  														// create the array with the number that the processor have to sum
		somma_parziale = somma_parziale + i; 								//make the partial sum for that processor
	}

	if (myid ==0) { 														//if I am the master process gather results from others
	long long int j;
		for (j = vett; j<vett+divisibile; j++){
			somma_parziale = somma_parziale + j;
		}
		somma = somma_parziale;
		start_time_comm=MPI_Wtime(); 
    	for (proc=1; proc<numprocs ; proc++) {
    		MPI_Recv(&somma_parziale,1,MPI_LONG_LONG,proc,tag,MPI_COMM_WORLD,&status ) ; //master receive the partial sum by the processors
      		printf("%lli\n", somma_parziale); 
			somma= somma+somma_parziale ;
		}
		end_time_comm=MPI_Wtime(); 
    	printf("%lli\n", somma);  											//master processor print the total sum
    	end_time_proce=MPI_Wtime();   										// stop the time that rappresent the time for run the program 
																			//in the master (all program) because the master makes the last operation
	}
  
	else {  																// for all the slave processes send results to the master 
		MPI_Ssend(&somma_parziale , 1 ,MPI_LONG_LONG, master , tag ,MPI_COMM_WORLD) ; //the processors send to the master processor their partial sum
    	end_time_proce=MPI_Wtime();											//stop the time that rappresent the time for run the program in the processor
	}
	
   	printf ( "\n # walltime : %10.8f  \t on the processor: %i\n" , end_time_proce - start_time_proce, myid ) ;
   	printf ( "\n # time to read the number : %10.8f \t on the processor: %i\n", end_time_read - start_time_read, myid ) ;
	printf ( "\n # time for communicate to the master the partial sum : %10.8f \t on the processor: %i\n \n", end_time_comm - start_time_comm, myid ) ;
   
   	MPI_Finalize() ;
}
