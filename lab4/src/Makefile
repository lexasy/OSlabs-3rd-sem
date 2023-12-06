COMPILER = gcc
COMPILER_FLAGS = -std=c99 -Wall -pedantic -lm
COMPILE_FLAG = -c
OBJECT_FLAG = -o


# all : linking_realization1 linking_realization2

linking_realization2 : linking_main realization2
		$(COMPILER) $(COMPILER_FLAGS) linking_main.o realization2.o $(OBJECT_FLAG) linking_realization2

linking_realization1 : linking_main realization1
		$(COMPILER) $(COMPILER_FLAGS) linking_main.o realization1.o $(OBJECT_FLAG) linking_realization1

linking_main: linking_main.c
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) linking_main.c

realization2 : realization2.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) realization2.c

realization1 : realization1.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) realization1.c

clean : 
		rm -rf *.o