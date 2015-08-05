CC = g++
CFLAGS = -I.

all: 
	$(CC) saliency.cpp intensity.cpp common.hpp intensity.hpp color.hpp color.cpp orientation.cpp orientation.hpp saliency_common.hpp saliency_common.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_gpu -D_REENTRANT -lpthread -O3 -o saliency.o $(CFLAGS)


clean: 
	rm *.o
