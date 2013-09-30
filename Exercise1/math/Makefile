COMPILER = gcc
#LINKER = ar -r -cs 
LINKER = ld -r -o 

COMPILER_FLAGS = -ansi -Wall
ifeq ($(COMPILER),gcc)
COMPILER_FLAGS += -std=c99
endif

ifeq ($(MAKECMDGOALS),debug)
COMPILER_FLAGS += -g -ggdb
endif

INCLUDES = -I..

LIBNAME = libicgip_math.a

.PHONY: debug

../$(LIBNAME): f3.o f3x3.o f4x4.o
	$(LINKER) ../$(LIBNAME) $^ 

f3.o: f3.c f3.h
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDES) -c $< -o $@

f3x3.o: f3x3.c f3x3.h f3.h
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDES) -c $< -o $@

f4x4.o: f4x4.c f4x4.h f3x3.h f3.h
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDES) -c $< -o $@

debug: ../$(LIBNAME)

clean_obj:
	rm -f *.o

clean_bin:
	rm -f ../$(LIBNAME)

clean: clean_obj clean_bin
