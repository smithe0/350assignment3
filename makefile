# makefile for assignment
#
#  make        refreshes executable 
#  make clean  removes all object files and str
#  make dep    creates list of dependencies that can 
#              be pasted at the end
PROG := simul

# uncomment for debug mode
CCOPTS := -Wall -Wextra  -g -std=c++11


# compiler and linker
CC := g++ 
#CC := clang++-3.6 

# how to create .o files from .c files
%.o : %.C makefile
	$(CC) $(CCOPTS) -c -o $@ $<

# how to create executable from object files
$(PROG) : simul.o World.o World2.o Marine.o Tank.o
	$(CC) -o $@ $^ -lglut -lGL

# print dependencies
dep:
	g++ -MM *.C

# remove object files
clean:  
	rm -f *.o $(PROG)

# other file dependencies
# (output of make dep)
Marine.o: Marine.C Marine.H Unit.H World.H
simul.o: simul.C World.H Unit.H Marine.H Tank.H
Tank.o: Tank.C Tank.H Unit.H World.H
World2.o: World2.C World.H
World.o: World.C World.H Unit.H
