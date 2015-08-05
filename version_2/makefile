CC = g++
CFLAGS = -I.

all: 
	$(CC) `pkg-config --cflags opencv` `pkg-config --libs opencv` saliency.cpp intensity.cpp common.hpp intensity.hpp color.hpp color.cpp orientation.cpp orientation.hpp saliency_common.hpp saliency_common.cpp -D_REENTRANT -lpthread -O3 -o saliency.o $(CFLAGS)


clean: 
	rm *.o
