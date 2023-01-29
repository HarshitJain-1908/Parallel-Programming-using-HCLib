#include "/home/harshit/fpp/assignment1/library/stamp.h"			// stamp APIs are declared here
#include <iostream>
#include <assert.h>

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
  // start the parallel addition of two vectors
  // std::cout<<"nt " << numThread;
  parallel_for(0, size, 2, [&](int i) {
    C[i] = A[i] + B[i];
  }, numThread);
  // end the timer
  // verify the result vector
  for(int i=0; i<size; i++) {
    if (i % 2 == 0) assert(C[i] == 2);
    else assert(C[i] == 0);
  }
  printf("Test Success\n");
  // cleanup memory
  delete[] A;
  delete[] B;
  delete[] C;
  return 0;
}