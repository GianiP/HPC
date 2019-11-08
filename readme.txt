All programs were performed on Ulysse the cluster of Sissa.
I've always used the Openmpi library as described below. Furthermore, in the request for resources to the cluster
I have always preferred to work with a single node to avoid the communication time between nodes.

In particular the module was openmpi/1.9.2/gnu/4.9.2

All the programs run on the cluster have been written in C because is the programming language
that I know best and is the most efficient and fast.
Python was used for the graphics as it is the simplest language.


Some useful commands used:

1. take 1 node and 20 core for 30 min
        qsub -l nodes=1:ppn=20 -I -l walltime=00:30:00

2.load the module
        module load openmpi/1.9.2/gnu/4.9.2

3.compile in parallel mode (only after loading the module)
        mpicc nomeprogramma.c -o nomeuscita.x

4.execute the program with a numbers of core
        mpirun -np {number_of_cores} ./nomeuscita.x


ATTENTION: after having requested the resources, remember to contact them in the correct folder and not in the home.
           Also remember to load the openmpi module also in the nodes.