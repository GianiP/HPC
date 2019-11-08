/* 	Author: Giovanni Pinna
	Date: 01 november 2019
	work: this program take a number N form a command line
			after that make the sum of N numbers. 
			All of this happend in parallel on p processors.
	how to compile with openmpi/1.8.3/gnu/4.9.2:
												module load openmpi/1.8.3/gnu/4.9.2
												mpicc SumNumber.c -o SumNumber.x
												mpirun -np {number of node} ./SumNumber.x {number of iteration}
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
	
	start_time_read=MPI_Wtime();  									//start to take the tame for read the number in the file
	int resto = atoi(argv[1])%numprocs
	long long int N = (atoll(argv[1])-resto)/numprocs;
	end_time_read=MPI_Wtime(); 												//stop the time for read the file
																			//check if the number inside the file is divisibile by the number of processors  
	local_somma=0;
	long long int i;
	for(i=N*myid;i<N*(myid+1); i++){
		local_somma = local_somma +i;
	}	
	
	if (myid ==0) { 														//if I am the master process gather results from others
		start_time_comm = MPI_Wtime();
		somma = local_somma + resto;
    	for (proc=1; proc<numprocs ; proc++) {
    		MPI_Recv(&local_somma,1,MPI_LONG_LONG,proc,tag,MPI_COMM_WORLD,&status ) ; //master receive the partial sum by the processors
      		somma += local_somma
		}
		end_time_comm=MPI_Wtime(); 
    	end_time_proce=MPI_Wtime();   										// stop the time that rappresent the time for run the program 
																			//in the master (all program) because the master makes the last operation
	}
  
	else {  
																		// for all the slave processes send results to the master 
		MPI_Ssend(&somma_parziale , 1 ,MPI_LONG_LONG, master , tag ,MPI_COMM_WORLD) ; //the processors send to the master processor their partial sum
    	end_time_proce=MPI_Wtime();											//stop the time that rappresent the time for run the program in the processor
		printf ("\n the partial sum is: %lld, on the processor %d", somma_pariale, myid);
	}
	
   	printf ( "\n # walltime : %10.8f  \t on the processor: %d\n" , end_time_proce - start_time_proce, myid ) ;
   	printf ( "\n # time to read the number : %10.8f \t on the processor: %d\n", end_time_read - start_time_read, myid ) ;
	printf ( "\n # time for communicate to the master the partial sum : %10.8f \t on the processor: %d\n \n", end_time_comm - start_time_comm, myid ) ;
   
   	MPI_Finalize() ;
   	return 0;
}
