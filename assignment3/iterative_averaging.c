#include <omp.h>
#include <mpi.h>
#include<sys/time.h>
#include <stdlib.h>
#include <stdio.h>

long get_usecs () {
  struct timeval t;
  gettimeofday(&t,NULL);
  return t.tv_sec*1000000+t.tv_usec;
}

//global array
double **A, **shadow;

int main(int argc, char **argv) {

    long N = (argc > 1) ? (atol)(argv[1]) : 25165824l;
    int NI = (argc > 2) ? (atoi)(argv[2]) : 64;
    int P;

    int rank, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    MPI_Request *send_req, *recv_req;
    MPI_Status *send_stats, *recv_stats;

    //Allocate request and status array
    send_req = (MPI_Request*) malloc(sizeof(MPI_Request)*2);  
    send_stats = (MPI_Status*) malloc(sizeof(MPI_Status)*2); 

    recv_req = (MPI_Request*) malloc(sizeof(MPI_Request)*2);  
    recv_stats = (MPI_Status*) malloc(sizeof(MPI_Status)*2); 

    P = np;

    long start[np], end[np];
    long batch_size = N/np;
    if (batch_size*np != N) batch_size++;
    start[0] = 0, end[0] = batch_size;
    long left = N - batch_size;

    for (int t = 1; t < np; t++) {
        batch_size = left/(np-t);
        if (batch_size*(np-t) != left) batch_size++;
        start[t] = end[t-1];
        end[t] = start[t] + batch_size;
        left = left - batch_size;
    }

    //Array A is partioned amongst all the process equally depending on their batch size
    //The two extremas A[0] and A[N+1] aren't allocated to maintain uniformity and being
    //only 2 elements they are treated specially in val1 and val2 by all the processes 
    //again to maintain uniformity across all the processes.
    
    //allocating array A
    long SIZE = end[rank]-start[rank];
    A = (double**) malloc(sizeof(double) * SIZE);
    shadow = (double**) malloc(sizeof(double) * SIZE);
    

    for (long i = 0; i < SIZE; i++) {
        A[i] = (double*) malloc(sizeof(double));
        shadow[i] = (double*) malloc(sizeof(double));
    }

    int tag = 123;

    for (long i = 0; i < SIZE; i++) *A[i] = 0;

    if (SIZE > 0) {
        long initial, ending;
        int flag1, flag2, flag3, flag4;
        flag1 = flag2 = flag3 = flag4 = 0;
        if (rank == 0) initial = get_usecs();

        for (int iter = 0; iter < NI; iter++) {
            double val1, val2;
            if (rank > 0) {
                //sending first element of my chunk to (rank-1)th process
                if (end[rank-1]-start[rank-1] > 0) {
                    MPI_Isend(A[0], 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &(send_req[0]));
                    flag1 = 1;
                }
            }

            if (rank < np-1) {
                //sending last element of my chunk to (rank+1)th process
                if (end[rank+1]-start[rank+1] > 0) {
                    MPI_Isend(A[SIZE-1], 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &(send_req[1]));
                    flag2 = 1;
                }
            }

            if (rank > 0) {
                //receiving last element of (rank-1)th process' chunk
                if (end[rank-1]-start[rank-1] > 0) {
                    MPI_Irecv(&val1, 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &(recv_req[0]));
                    flag3 = 1;
                }else val1 = 0.0;
            }else val1 = 0.0;

            if (rank < np-1) {
                //receiving first element of (rank+1)th process' chunk
                if (end[rank+1]-start[rank+1] > 0) {
                    MPI_Irecv(&val2, 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &(recv_req[1]));
                    flag4 = 1;
                }else val2 = N+1l;
            }else val2 = (N+1l);

            #pragma omp parallel for default(none) shared(SIZE, shadow, A)
            for (long i = 1; i < SIZE-1; i++) {
                *shadow[i] = (*A[i-1] + *A[i+1]) / 2.0;
            }

            if (flag1 == 1) {
                MPI_Wait(&(send_req[0]), &(send_stats[0]));
            }
            if (flag2 == 1) {
                MPI_Wait(&(send_req[1]), &(send_stats[1]));
            }
            if (flag3 == 1) {
                MPI_Wait(&(recv_req[0]), &(recv_stats[0]));
            }
            if (flag4 == 1) {
                MPI_Wait(&(recv_req[1]), &(recv_stats[1]));
            }

            //doing first and last computation here
            if (SIZE == 1l) *shadow[SIZE-1] = (val1 + val2) / 2.0;
            else {
                *shadow[0] = (val1 + *A[1]) / 2.0;
                *shadow[SIZE-1] = (*A[SIZE-2] + val2) / 2.0;
            }
            double** temp = shadow;
            shadow = A;
            A = temp;
           
        }
        if (rank == 0) {
            ending = get_usecs();
            double dur = ((double)(ending-initial))/1000000;
            printf("Time = %.3fs\n", dur);
        }
    }

    double my_sum = 0.0, total_sum = 0.0;
    for (long i = 0; i < SIZE; i++) my_sum += *A[i];
    MPI_Reduce(&my_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Array sum = %f\n", total_sum);
    }
    MPI_Finalize();

    return 0;
}