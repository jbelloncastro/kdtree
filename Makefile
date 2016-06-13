CXX=g++
CXXFLAGS=-O3 -std=c++11

all: test benchmark

clean:
	rm -f test benchmark
