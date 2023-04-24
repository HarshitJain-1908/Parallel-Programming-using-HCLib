/*Executing the same code with -O3 flag gives segmentation fault because of Waitall API
so, please execute without this flag*/

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include<sys/time.h>

#define N 1024

long get_usecs () {
  struct timeval t;
  gettimeofday(&t,NULL);
  return t.tv_sec*1000000+t.tv_usec;
}

int main(int argc, char **argv) {

    int rank, np;
    int start[np], end[np];

    MPI_Request *req_array;
    MPI_Status *stats_array;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    //Allocate request and status array
    req_array = (MPI_Request*) malloc(sizeof(MPI_Request) * (np-1));  
    stats_array = (MPI_Status*) malloc(sizeof(MPI_Status) * (np-1)); 

    int** A = (int**) malloc(sizeof(int*) * N);
    int** B = (int**) malloc(sizeof(int*) * N);
    int** C = (int**) malloc(sizeof(int*) * N);

    //Allocating A, B and C
    for (int i = 0; i < N; i++) {
        A[i] = (int*) malloc(sizeof(int*) * N);
        B[i] = (int*) malloc(sizeof(int*) * N);
        C[i] = (int*) malloc(sizeof(int*) * N);
    }
    
    int batch_size = N/np;

    int tag = 123;
    MPI_Status stats;

    if (rank == 0) {
        int dest;

        //initializing A and B by the master only
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = 1;
                B[i][j] = 1;
            }
        }
    }

    start[0] = 0, end[0] = batch_size;
    int left = N - batch_size;
    int dest;

    for (int t = 1; t < np; t++) {
        dest = t;
        batch_size = left/(np-t);
        start[t] = end[t-1];
        end[t] = start[t] + batch_size;
        left = left - batch_size;
    }

    //Broadcasting B
    for (int i = 0; i < N; i++) {
        MPI_Ibcast(B[i], N, MPI_INT, 0, MPI_COMM_WORLD, &(req_array[0]));
    }
    MPI_Wait(&(req_array[0]), &stats);

    //scattering A
    // for (int i = 0; i < N; i++) {
        // MPI_Scatter(A, (N/np)*N, MPI_INT, A, (N/np)*N, MPI_INT, 0, MPI_COMM_WORLD);
    // }

    if (rank == 0) {

        for (int t = 1; t < np; t++) {
            dest = t;

            //Logically, A should be scattered here i.e. collective communication is used but since there can be uneven 
            //distribution of data amongst the processes so normal scatter API won't help rather scatterv should be used
            //but since it's not covered in the lectures only pt2pt send, recv APIs are used. 

            //sending A
            for (int i = start[t]; i < end[t]; i++) {
                //sending a row at a time to one of the process
                MPI_Isend(A[i], N, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[dest-1]));
            }
        }

        MPI_Waitall(np-1, req_array, stats_array);

        //After sending all the data, root node starts its computation. 
        long initial = get_usecs();

        for (int i = start[0]; i < end[0]; i++) {
            for (int j = 0; j < N; j++) {
                C[i][j] = 0;
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k]*B[k][j];
                }
            }
        }

        int src;

        for (int t = 1; t < np; t++) {
            src = t;
            for (int i = start[t]; i < end[t]; i++) {
                MPI_Irecv(C[i], N, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[src-1]));
            }
        }

        MPI_Waitall(np-1, req_array, stats_array);

        long ending = get_usecs();
        double dur = ((double)(ending-initial))/1000000;
        //this is the elapsed time after sending data to all nodes and receiving data from all other nodes.
        printf("Time = %.3fs\n", dur);
        
    }else {

        int src = 0;
        
        //receiving A
        for (int i = start[rank]; i < end[rank]; i++) {
            MPI_Irecv(A[i], N, MPI_INT, src, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
        }

        MPI_Wait(&(req_array[rank-1]), &stats);

        for (int i = start[rank]; i < end[rank]; i++) {
            for (int j = 0; j < N; j++) {
                C[i][j] = 0;
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k]*B[k][j];
                }
            }
        }
        
        int dest = 0;
        for (int i = start[rank]; i < end[rank]; i++) {
            MPI_Isend(C[i], N, MPI_INT, dest, tag, MPI_COMM_WORLD, &(req_array[rank-1]));
        }

        MPI_Wait(&(req_array[rank-1]), &stats);
    }

    if (rank == 0) {
        //Verify the result at root
        int expected = 1; 
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (C[i][j] != N) {
                    printf("0: Test FAILED\n");
                    expected = 0;
                    break;
                }
            }
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