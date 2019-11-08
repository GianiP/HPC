/* 	Author: Giovanni Pinna
	Date: 01 november 2019
	work: this program take a number N form a file that has name numero.txt
			after that make the sum of N number with a collective operation. 
			All of this happend in parallel on p processors.
	how to compile with openmpi/1.8.3/gnu/4.9.2:
												module load openmpi/1.8.3/gnu/4.9.2
												mpicc SumNumber2_colletive_operation.c -o SumNumber2_colletive_operation.x
												mpirun -np {number of node} ./SumNumber2_colletive_operation.x
*/
#include<malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#define USE MPI

int main ( int argc , char *argv[ ] ){
	
	long long int somma, local_somma;
	double start_time_proce, end_time_proce, start_time_read, end_time_read,start_time_comm, end_time_comm ;   	// variables for mesurng the times  
	int myid , numprocs , proc ;  									// variables for info about the processor
	
	//about MPI
	MPI_Status status;
	MPI_Request request;
	
	// master process
	int master = 0;
	int tag = 123;
	
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
	
	start_time_read=MPI_Wtime();  												//start to take the tame for read the number in the file

	FILE *fd; //pointers to file
	long long int vett;
  
	// open the file 
	fd=fopen("numero.txt", "r"); 											//open in read mode the file (in this case my file name is "numero") 
  																			//and read the number inside 
	// error management for file 
	if( fd==NULL ) {
  		perror("Errore in apertura del file");
  		exit(1);
	}

	fscanf(fd, "%lld", &vett);												// read the the number inside the file and put the numver inside the variavle vett 		
	  
	fclose(fd);  															// close the file 
	end_time_read=MPI_Wtime();												//stop the time for read the file

	int resto = atoi(vett)%numprocs
	long long int N = (vett)-resto)/numprocs;
																			//check if the number inside the file is divisibile by the number of processors  
	local_somma=0;
	long long int i;
	for(i=N*myid;i<N*(myid+1); i++){
		local_somma = local_somma +i;
	}	

	MPI_Reduce(&local_somma, &somma, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD); //with this istruction all processor send to master their partial sum
   																					//the istruction make the sum of the partial sum and put the result 
																					//into "somma" in the end the master processor have the sum of the all 
    																				//partial sum into "somma"
    																			
	end_time_proce=MPI_Wtime();											//stop the time that rappresent the time for run the program in the processor
	end_time_comm=MPI_Wtime(); 
    end_time_proce=MPI_Wtime();   										// stop the time that rappresent the time for run the program 

   	printf ( "\n # walltime : %10.8f  \t on the processor: %d\n" , end_time_proce - start_time_proce, myid ) ;
   	printf ( "\n # time to read the number : %10.8f \t on the processor: %d\n", end_time_read - start_time_read, myid ) ;
	printf ( "\n # time for communicate to the master the partial sum : %10.8f \t on the processor: %d\n \n", end_time_comm - start_time_comm, myid ) ;
   
   	MPI_Finalize() ;
   	return 0;
}
