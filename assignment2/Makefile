quill = quill_obj shared_lib nqueens
all: clean $(quill)
quill_obj: 
	g++ -c -Wall -Werror -fpic quill-runtime.cpp -lpthread
shared_lib: 
	g++ -shared -o libquill.so quill-runtime.o
nqueens:
	g++ -L. -I. -Wall -g -o nqueens nqueens.cpp -lquill -lpthread
clean:
	rm -rf *.o $(quill)
	rm -rf libquill.so