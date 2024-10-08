# CSF Assignment 3 Makefile
# You should not need to make any changes

.PHONY: clean csim solution.zip

CC = g++
CFLAGS = -g -Wall -Wextra -pedantic

CSIM_SRCS = assignment3.cpp
CSIM_OBJS = $(CSIM_SRCS:.cpp=.o)

EXES = csim

%.o : %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

all : $(EXES)

csim : $(CSIM_OBJS)
	$(CC) $(CFLAGS) -o $@ $+

# Clean target to remove all object files and executables
clean :
	rm -f *.o $(EXES)

# Use this target to prepare a zip file to upload, including source files, headers, and Makefile
solution.zip :
	rm -f $@
	zip -9r $@ *.cpp Makefile README.txt

depend :
	$(CC) $(CFLAGS) -M $(CSIM_SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
