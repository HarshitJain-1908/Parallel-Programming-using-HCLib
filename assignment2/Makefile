quill = quill_obj shared_lib nqueens run
all: clean $(quill)
quill_obj: 
	g++ -std=c++11 -c -Wall -Werror -fpic -g quill-runtime.cpp -lpthread
shared_lib: 
	g++ -std=c++11 -g -shared -lpthread -o libquill.so quill-runtime.o
nqueens:
	g++ -std=c++11 -L. -I. -Wall -g -o nqueens nqueens.cpp -lquill -lpthread
clean:
	rm -rf *.o $(quill)
	rm -rf libquill.so
run:
	QUILL_WORKERS=1 ./nqueens 12
	QUILL_WORKERS=2 ./nqueens 12
	QUILL_WORKERS=3 ./nqueens 12
	QUILL_WORKERS=4 ./nqueens 12