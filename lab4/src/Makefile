COMPILER = gcc
COMPILER_FLAGS = -std=c99 -Wall -pedantic
COMPILE_FLAG = -c
OBJECT_FLAG = -o
MATH_FLAG = -lm
PIC_FLAG = -fPIC
SHARED_FLAG = -shared
DYNAMIC_LOADING_FLAG = -ldl

all : linking_realization1 linking_realization2 dynamic

linking_realization2 : linking_main realization2
		$(COMPILER) $(COMPILER_FLAGS) linking_main.o realization2.o $(OBJECT_FLAG) linking_realization2 $(MATH_FLAG)

linking_realization1 : linking_main realization1
		$(COMPILER) $(COMPILER_FLAGS) linking_main.o realization1.o $(OBJECT_FLAG) linking_realization1 $(MATH_FLAG)

linking_main: linking_main.c
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) linking_main.c

realization2 : realization2.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) realization2.c

realization1 : realization1.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) realization1.c

dynamic : dynamic_main dynamic_realization1 dymanic_realization2
		$(COMPILER) $(COMPILER_FLAGS) dynamic_main.o $(OBJECT_FLAG) dynamic $(DYNAMIC_LOADING_FLAG)

dynamic_main : dynamic_main.c 
		$(COMPILER) $(COMPILER_FLAGS) $(COMPILE_FLAG) dynamic_main.c

dymanic_realization2 : realization2.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(PIC_FLAG) $(SHARED_FLAG) $(OBJECT_FLAG) librealization2.so realization2.c $(MATH_FLAG)

dynamic_realization1 : realization1.c foos.h
		$(COMPILER) $(COMPILER_FLAGS) $(PIC_FLAG) $(SHARED_FLAG) $(OBJECT_FLAG) librealization1.so realization1.c $(MATH_FLAG)

clean : 
		rm *.o *.so linking_realization1 linking_realization2 dynamic