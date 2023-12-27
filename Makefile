#!/bin/bash
CC = g++
DEBUG = -g3
PROJNAME = Perceptron

#HEADERPATH = ./.
SOURCEPATH = ./.

#LOCALDEPSINCLUDES = ./deps
#AUXINCLUDES = 
#AUXLIBS = 

#INCLUDES = -I$(LOCALDEPSINCLUDES) -I$(AUXINCLUDES)  
#LIBS = -L$(AUXLIBS) 
#LIBS += -L/usr/local/lib/
#rlunaro: removed optimization for tests: -O3
CFLAGS = -std=gnu++11 -std=c++11 -Wall -fmessage-length=0 -fPIC $(INCLUDES)
CFLAGS += $(DEBUG)

HDRS  = $(shell find $(HEADERPATH) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) -name '*.h')
#HDRS += $(shell find $(HEADERPATH) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) -name '*.h++')
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
#SRCS += $(shell find $(SOURCEPATH) -name '*.c')
OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)

all : PerceptronTest $(PROJNAME).a $(PROJNAME).so

$(PROJNAME).a : $(SOURCEPATH)/Perceptron.o
	@echo Creating static lib $@
	ar rcs $@ $(SOURCEPATH)/Perceptron.o

$(PROJNAME).so : $(SOURCEPATH)/Perceptron.o
	@echo Creating dynamic lib $@
	$(CC) -o $@ $(SOURCEPATH)/MLP.o $(LDFLAGS) $(LFLAGS) 

#PerceptronTest: $(SOURCEPATH)/PerceptronTest.o  $(SOURCEPATH)/Perceptron.o
PerceptronTest: $(SOURCEPATH)/Perceptron.cpp
	@echo Compiling program $@
	#$(CC)  $^ $(CFLAGS) $(LFLAGS) -o $@
	$(CC)  $(CFLAGS) -o Perceptron.cpp main.cpp
	
clean:
	@echo Clean
	rm -f *~ $(SOURCEPATH)/*.o *~
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ $(SOURCEPATH)/*.o *~ $(PROJNAME).a $(PROJNAME).so PerceptronTest 
	@echo Success
