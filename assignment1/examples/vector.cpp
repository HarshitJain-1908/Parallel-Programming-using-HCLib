#include "stamp.h"			// stamp APIs are declared here
#include <iostream>
#include <assert.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv) {
  // intialize problem size
  // std::cout<<argc<< " ";
  int numThread = argc>1 ? atoi(argv[1]) : 1;
  int size = argc>2 ? atoi(argv[2]) : 48000000;  
  // allocate vectors
  int* A = new int[size];
  int* B = new int[size];
  int* C = new int[size];
  // initialize the vectors
  std::fill(A, A+size, 1);
  std::fill(B, B+size, 1);
  std::fill(C, C+size, 0);
  // start the timer
  clock_t start, end;
  double time_taken;
  start = clock();
        // stamp API to ex
  // start the parallel addition of two vectors
  // std::cout<<"nt " << numThread;
  stamp::parallel_for(0, size, 1, [&](int i) {
    C[i] = A[i] + B[i];
  }, numThread);
  // end the timer
  end = clock();
  time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "Time taken for execution is : " << time_taken << " sec " <<endl;

  // verify the result vector
  for(int i=0; i<size; i++) {
    // if (i % 2 == 0) 
    assert(C[i] == 2);
    // else assert(C[i] == 0);
  }
  printf("Test Success\n");
  // cleanup memory
  delete[] A;
  delete[] B;
  delete[] C;
  return 0;
}