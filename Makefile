CXX=g++
CXXFLAGS=-std=c++11

all: test

test: test.cc

clean:
	rm -f test
