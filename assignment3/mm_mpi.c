#include <omp.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define N 128

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

    int **A = (int**) malloc(sizeof(int*) * N * N);
    int **B = (int**) malloc(sizeof(int*) * N * N);
    int **C = (int**) malloc(sizeof(int*) * N * N);

    //Allocating A, B and C
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i*N + j] = (int*) malloc(sizeof(int));
            B[i*N + j] = (int*) malloc(sizeof(int));
            C[i*N + j] = (int*) malloc(sizeof(int));
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
                *A[i*N + j] = 1;
                *B[i*N + j] = 1;
            }
        }

        int start = 0, end = chunk_size;
        int left = N - chunk_size;

        for (int t = 1; t < np; t++) {
            dest = t;
            int start = t*chunk_size;
            int end = start + chunk_size;
            left -= chunk_size;
            // printf("%d %d\n", chunk_size, left);

            //sending A
            for (int i = start; i < end; i++) {
                for (int j = 0; j < N; j++) {
                    MPI_Isend(A[i*N + j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[dest-1])); 
                }
            }
            //sending B
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    MPI_Isend(B[i*N + j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[dest-1]));
                }
            }
        }

        MPI_Waitall(np-1, req_array, stats_array);

        // // for (int i = 0; i < N; i++) *C[i] = *A[i] + *B[i];
        for (int i = start; i < end; i++) {
            for (int j = 0; j < N; j++) {
                *C[i*N + j] = 0;
                for (int k = 0; k < N; k++) {
                    *C[i*N + j] += (*A[i*N + k]) * (*B[k*N + j]);
                }
            }
        }

        int src;

        for (int t = 1; t < np; t++) {
            src = t;
            int start = t*chunk_size;
            int end = start + chunk_size;
            for (int i = start; i < end; i++) {
                for (int j = 0; j < N; j++) {
                    MPI_Irecv(C[i*N + j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[src-1]));
                }
            }
            // MPI_Wait(&(req_array[src-1]), &stats);

        }

        MPI_Waitall(np-1, req_array, stats_array);
        // for (int i = 0; i < N; i++) printf("%d ", *C[i]);
        // printf("\n");

    }else {

        int src = 0;
        int start = rank*chunk_size;
        int end = start + chunk_size;

        //receiving A
        for (int i = start; i < end; i++) {
            for (int j = 0; j < N; j++) {
                MPI_Irecv(A[i*N + j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
            }
        }
        //receiving B
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                MPI_Irecv(B[i*N + j], 1, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
            }
        }

        MPI_Wait(&(req_array[rank-1]), &stats);
        printf("recv A and B\n");

        for (int i = start; i < end; i++) {
            for (int j = 0; j < N; j++) {
                *C[i*N + j] = 0;
                for (int k = 0; k < N; k++) {
                    // if (rank == 1) printf("mul: %d %d ", *A[i*N + k] , *B[k*N + j]);
                    *C[i*N + j] += (*A[i*N + k]) * (*B[k*N + j]);
                }
                // if (rank == 1) printf("%d ", *C[i*N + j]);
            }
            // printf("\n");
        }
        // printf("-----------%d----------\n", rank);
        // for (int i = start; i < end; i++) {
        //     for (int j = 0; j < N; j++) {
        //         printf("%d ", *A[i*N + j]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
        // for (int i = start; i < end; i++) {
        //     for (int j = 0; j < N; j++) {
        //         printf("%d ", *B[i*N + j]);
        //     }
        //     printf("\n");
        // }
        // printf("rank%d start: %d end: %d\n", rank, start, end);
        // for (int i = start; i < end; i++) {
        //     for (int j = 0; j < N; j++) {
        //         printf("%d ", *C[i*N + j]);
        //     }
        //     printf("\n");
        // }
        int dest = 0;
        for (int i = start; i < end; i++) {
            for (int j = 0; j < N; j++) {
                MPI_Isend(C[i*N + j], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
            }
        }
        MPI_Wait(&(req_array[rank-1]), &stats);
        // printf("\n");
        // for (int i = start; i < end; i++) printf("%d ", *B[i]);
    }

    // printf("Hello from rank%d in world of size %d\n", rank, np);

    if (rank == 0) {
        //Verify the result at root
        int expected = 1; 
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // printf("%d ", *C[i*N + j]);
                if (*C[i*N + j] != N) {
                    printf("0: Test FAILED\n");
                    expected = 0;
                    break;
                }
            }
            // printf("\n");
            if (expected == 0) break;
        }
        if(expected == 1) printf("0: Test SUCCESS\n"); 
    }

    free(A);
    free(B);
    free(C);
    MPI_Finalize();

    return 0;
}