#include <omp.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {

    long N = (argc > 1) ? (atol)(argv[1]) : 1024;
    int NI = (argc > 2) ? (atoi)(argv[2]) : 64;
    int P = (argc > 3) ? (atoi)(argv[3]) : 4;

    int rank, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) printf("N=%ld NI=%d P=%d\n", N, NI, P);
    printf("Hello from rank%d in world of size %d\n", rank, np);

    MPI_Finalize();

    return 0;
}