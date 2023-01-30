Following are the steps to run given examples (fib.cpp, vector.cpp & matrix.cpp) using StaMp APIs 
implemented in stamp.cpp:-

1. Go in the library folder and run 'make' command to create on object file(.o) for stamp.cpp and a 
shared library(.so) file.
2. Then, move to examples folder and run 'make' command to create the executables for given examples
file.
3. After this, we have to export this shared library to the folder in which examples file are present
by providing a path to the shared lirbary. Run these following commands back to back:-
1. echo $LD_LIBRARY_PATH
2. LD_LIBRARY_PATH=/home/harshit/fpp/assignment1/library/:$LD_LIBRARY_PATH
3. export LD_LIBRARY_PATH=/home/harshit/fpp/assignment1/library/:$LD_LIBRARY_PATH
4. ./fib or ./vector or ./matrix (with valid arguments)