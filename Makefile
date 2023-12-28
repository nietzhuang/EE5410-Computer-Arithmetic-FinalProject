#!/bin/bash
CC = g++
DEBUG = -g3
PROJNAME = Perceptron
TASK = problem1

SOURCEPATH = ./.


#CFLAGS = -std=gnu++11 -std=c++11 -Wall -fmessage-length=0 -fPIC $(INCLUDES)
CFLAGS = -std=gnu++11 -std=c++11 
#CFLAGS += $(DEBUG)
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
CPPFLAGS = -DTASK=$(TASK)

all : PerceptronTest

PerceptronTest:
	@echo Compiling program $@
	$(CC)  $(CFLAGS) -o $@.o main.cpp $(CPPFLAGS) Perceptron.cpp ALU.cpp 	
	./PerceptronTest.o
	
clean:
	@echo Clean
	rm -f *~ $(SOURCEPATH)/*.o *~
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ $(SOURCEPATH)/*.o *~ $(PROJNAME).a $(PROJNAME).so PerceptronTest 
	@echo Success
