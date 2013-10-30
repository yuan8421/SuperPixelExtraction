OFILES = pixel.o image.o main.o

AFILES = $(OFILES) 
LIB = 

ROOTDIR = . 

LINKS =  -lm -lOpenImageIO -lgomp

INCLUDES = -I ./

CXX = g++ -g -W -Wall -Wextra -O2 -D_THREAD_SAFE -fopenmp 

.cpp.o: configure.h
	$(CXX) -c $(INCLUDES) $<

all: $(AFILES)
	$(CXX) -o color $(OFILES) $(INCLUDES) $(LINKS)

clean:
	rm *.o
	rm color

