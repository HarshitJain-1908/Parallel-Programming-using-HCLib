#include <omp.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

//global array
double **A, **shadow;

int main(int argc, char **argv) {

    long N = (argc > 1) ? (atol)(argv[1]) : 4;
    int NI = (argc > 2) ? (atoi)(argv[2]) : 4;
    int P;
    // P = (argc > 3) ? (atoi)(argv[3]) : 4;

    int rank, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    MPI_Request req;
    MPI_Status stats;

    //Allocate request and status array
    // req = (MPI_Request*) malloc(sizeof(MPI_Request));  
    // stats = (MPI_Status*) malloc(sizeof(MPI_Status)); 

    P = np;

    int start[np], end[np];
    int batch_size = N/np;
    start[0] = 0, end[0] = batch_size;
    int left = N - batch_size;

    for (int t = 1; t < np; t++) {
        batch_size = left/(np-t);
        start[t] = end[t-1];
        end[t] = start[t] + batch_size;
        left = left - batch_size;
    }

    //allocating array A
    int SIZE = end[rank]-start[rank];
    A = (double**) malloc(sizeof(double) * SIZE);
    shadow = (double**) malloc(sizeof(double) * SIZE);
    

    for (int i = 0; i < SIZE; i++) {
        A[i] = (double*) malloc(sizeof(double));
        shadow[i] = (double*) malloc(sizeof(double));
    }

    int tag = 123;
    // MPI_Status stats;

    for (int i = 0; i < SIZE; i++) *A[i] = 0;

    // for (int i = 0; i < SIZE; i++) {
    //     printf("%f ", A[i]);
    // }
    // printf("\n");
    for (int iter = 0; iter < NI; iter++) {
        double val1, val2;
        if (rank > 0) {
            //sending first element of my chunk to (rank-1)th process
            // MPI_Isend(A[0], 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &req);
            MPI_Send(A[0], 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD);
        }

        if (rank < np-1) {
            //sending last element of my chunk to (rank+1)th process
            // MPI_Isend(A[SIZE-1], 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &req);
            MPI_Send(A[SIZE-1], 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
        }

        if (rank > 0) {
            //receiving last element of (rank-1)th process' chunk
            // MPI_Irecv(&val1, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &req);
            MPI_Recv(&val1, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &stats);
        }else val1 = 0.0;

        if (rank < np-1) {
            //receiving first element of (rank+1)th process' chunk
            // MPI_Irecv(&val2, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &req);
            MPI_Recv(&val2, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &stats);
        }else val2 = (N+1);

        for (int i = 1; i < SIZE-1; i++) {
            *shadow[i] = (*A[i-1] + *A[i+1]) / 2.0;
        }
        printf("hi\n");
        // MPI_Wait(&req, &stats);
        printf("hello\n");

        //doing first and last computation here
        *shadow[0] = (val1 + *A[1]) / 2.0;
        *shadow[SIZE-1] = (*A[SIZE-1] + val2) / 2.0;
        double** temp = shadow;
        shadow = A;
        A = temp;
        printf("hello\n");

        for (int i = 0; i < SIZE; i++) printf("rank%d %f ", rank, *A[i]);
        printf("\n");
        // if (rank == 0) {
            
        //     for (int t = 1; t < np; t++) {
        //         // double B[end[t]-start[t]];
        //         double **B = (double**) malloc(sizeof(double) * (end[t]-start[t]));
        //         // shadow = (double**) malloc(sizeof(double) * SIZE);
            
        //         for (int i = 0; i < end[t]-start[t]; i++) {
        //             B[i] = (double*) malloc(sizeof(double));
        //             // shadow[i] = (double*) malloc(sizeof(double));
        //         }
        //         for (int i = 0; i < (end[t]-start[t]); i++)
        //             MPI_Irecv(B[i], 1, MPI_INT, t, tag, MPI_COMM_WORLD, &req);
        //         MPI_Wait(&req, &stats);
        //         for (int i = 0; i < (end[t]-start[t]); i++) printf("%f ", *B[i]);
        //         printf("\n");
        //     }
        // }else {
        //     for (int i = 0; i < (end[rank]-start[rank]); i++)
        //         MPI_Isend(A[i], 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &req);
        //     MPI_Wait(&req, &stats);
        // }
    }
    //     // for (int i = 1; i <= N; i++) printf("%f ", *A[i]);
    //     // printf("\n");
    // // printf("Hello from rank%d in world of size %d\n", rank, np);

    double my_sum = 0.0, total_sum = 0.0;
    for (int i = 0; i < SIZE; i++) my_sum += *A[i];
    printf("rank%d mysum:%f\n", rank, my_sum);
    MPI_Reduce(&my_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Array sum = %f\n", total_sum);
    }
    MPI_Finalize();

    return 0;
}