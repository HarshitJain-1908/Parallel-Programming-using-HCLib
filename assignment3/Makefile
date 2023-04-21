EXE=iterative_averaging mm_mpi
all: $(EXE)

iterative_averaging: iterative_averaging.c
	mpicc -o $@ $^ -fopenmp
mm_mpi: mm_mpi.c
	mpicc -o $@ $^ -fopenmp
clean:
	rm -rf *.o $(EXE)