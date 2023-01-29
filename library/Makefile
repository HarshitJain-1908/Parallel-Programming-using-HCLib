stamp_lib = stamp_obj shared_lib
all: $(stamp_lib)
stamp_obj: 
	g++ -c -Wall -Werror -fpic stamp.cpp -lpthread
shared_lib: 
	g++ -shared -o libstamp.so stamp.o