#!/bin/bash
CC = g++
DEBUG = -g3
PROJNAME = Perceptron
SOURCEPATH = ./.

#CFLAGS = -std=gnu++11 -std=c++11 -Wall -fmessage-length=0 -fPIC $(INCLUDES)
CFLAGS = -std=gnu++11 -std=c++11 
#CFLAGS += $(DEBUG)
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
#CPPFLAGS = -DTASK=$(TASK)

all : PerceptronTest Problem1 Problem2 Problem3 Problem4

PerceptronTest:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp Perceptron.cpp ALU.cpp 	
	./PerceptronTest.o
	
Problem1:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp -DPROBLEM1 Perceptron.cpp ALU.cpp 	
	./Problem1.o

Problem2:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp -DPROBLEM2 Perceptron.cpp ALU.cpp 	
	./Problem2.o

Problem3:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp -DPROBLEM3 Perceptron.cpp ALU.cpp 	
	./Problem3.o

Problem4:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp -DPROBLEM4 Perceptron.cpp ALU.cpp 	
	./Problem4.o

clean:
	@echo Clean
	rm -f *~ $(SOURCEPATH)/*.o *~
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ $(SOURCEPATH)/*.o *~ $(PROJNAME).a $(PROJNAME).so PerceptronTest 
	@echo Success
