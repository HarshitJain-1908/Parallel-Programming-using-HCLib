#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define N 1024

int main(int argc, char **argv) {

    int rank, np;
    MPI_Request *req_array;
    MPI_Status *stats_array;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    //Allocate request and status array
    req_array = (MPI_Request*) malloc(sizeof(MPI_Request) * (np-1));  
    stats_array = (MPI_Status*) malloc(sizeof(MPI_Status) * (np-1)); 

    // int **A = (int**) malloc(sizeof(int*) * N * N);
    // int **B = (int**) malloc(sizeof(int*) * N * N);
    // int **C = (int**) malloc(sizeof(int*) * N * N);

    int *A[N][N], *B[N][N], *C[N][N];


    // Allocating A, B and C
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // A[i][j] = (int*) malloc(sizeof(int));
            int *p = (int*) malloc(sizeof(int));
            A[i][j] = p;
            // B[i][j] = (int*) malloc(sizeof(int));
            // C[i][j] = (int*) malloc(sizeof(int));
            int *p1 = (int*) malloc(sizeof(int));
            B[i][j] = p1;
            int *p2 = (int*) malloc(sizeof(int));
            C[i][j] = p2;
        }
    }

    int chunk_size = N/np;
    int tag = 123; // any value
    MPI_Status stats;

    if (rank == 0) {
        int dest;
        // printf("%d %d\n", chunk_size, left);

        //initilaizing A and B
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                *A[i][j] = 1;
                *B[i][j] = 1;
            }
        }

        int left = N;

        // for (int t = 1; t < np; t++) {
        //     dest = t;
        //     left -= chunk_size;
        //     int start = (t-1)*chunk_size;
        //     int end = start + chunk_size;
        //     // printf("%d %d\n", chunk_size, left);

        //     //sending A
        //     for (int i = start; i < end; i++) {
        //         for (int j = 0; j < N; j++) {
        //             MPI_Isend(A[i][j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[dest-1])); 
        //             // MPI_Send(A[i], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        //         } 
        //     }
        //     //sending B
        //     for (int i = start; i < end; i++) {
        //         for (int j = 0; j < N; j++) {
        //             MPI_Isend(B[i][j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[dest-1])); 
        //             // MPI_Send(A[i], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        //         } 
        //     }
        //     // MPI_Wait(&(req_array[dest-1]), &stats);
        // }

        // MPI_Waitall(np-1, req_array, stats_array);

        // int start = (np-1)*chunk_size;
        // int end = start + left;
        // // printf("hello %d %d\n", start, end);
        // // for (int i = 0; i < N; i++) *C[i] = *A[i] + *B[i];
        // // for (int i = start; i < end; i++) *C[i] = *A[i] + *B[i];
        // for (int i = start; i < end; i++) {
        //     for (int j = 0; j < N; j++) {
        //         *C[i][j] = 0;
        //         for (int k = 0; k < N; k++) {
        //             *C[i][j] += (*A[i][k]) * (*B[k][j]);
        //         }
        //     }
        // }

        // int src;

        // for (int t = 1; t < np; t++) {
        //     src = t;
        //     int start = (t-1)*chunk_size;
        //     int end = start + chunk_size;
        //     for (int i = start; i < end; i++) {
        //         for (int j = 0; j < N; j++) {
        //             MPI_Irecv(C[i][j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[src-1]));
        //         }
        //     }
        //     // MPI_Wait(&(req_array[src-1]), &stats);

        // }

        // MPI_Waitall(np-1, req_array, stats_array);
        // for (int i = 0; i < N; i++) printf("%d ", *C[i]);
        // printf("\n");

    }else {

        // int src = 0;
        // int start = rank*chunk_size;
        // int end = start + chunk_size;
        // printf("%d\n", chunk_size);
        //receiving A
    //     for (int i = start; i < end; i++) {
    //         for (int j = 0; j < N; j++) {
    //             MPI_Irecv(A[i][j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
    //         }
    //     }
    //     //receiving B
    //     for (int i = start; i < end; i++) {
    //         for (int j = 0; j < N; j++) {
    //             MPI_Irecv(B[i][j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
    //         }
    //     }

    //     MPI_Wait(&(req_array[rank-1]), &stats);
    
    //     // for (int i = start; i < end; i++) printf("Rank:%d %d ", rank, *A[i]);

    //     for (int i = start; i < end; i++) {
    //         for (int j = 0; j < N; j++) {
    //             *C[i][j] = 0;
    //             for (int k = 0; k < N; k++) {
    //                 *C[i][j] += (*A[i][k]) * (*B[k][j]);
    //             }
    //         }
    //     }
    //     int dest = 0;
    //     for (int i = start; i < end; i++) {
    //         for (int j = 0; j < N; j++) {
    //             MPI_Isend(C[i][j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
    //         }
    //     }
    //     MPI_Wait(&(req_array[rank-1]), &stats);
    //     // printf("\n");
    //     // for (int i = start; i < end; i++) printf("%d ", *B[i]);
    }

    // printf("Hello from rank%d in world of size %d\n", rank, np);

    // if (rank == 0) {
    //     //Verify the result at root
    //     int expected = 1; 
    //     // for(i=0; i<np * TOTAL_AT_RANK; i++) expected += i;
    //     for (int i = 0; i < N; i++) {
    //         for (int j = 0; j < N; j++) {
    //             if (*C[i][j] != 2) {
    //                 printf("0: Test FAILED\n");
    //                 expected = 0;
    //                 break;
    //             }
    //         }
    //         if(expected == 0) break;
    //     }
    //     if(expected == 1) printf("0: Test SUCCESS\n"); 
    // }

    // free(A);
    // free(B);
    // free(C);
    MPI_Finalize();

    return 0;
}