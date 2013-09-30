COMPILER = g++
#LINKER = ar -r -cs 
LINKER = ld -r -o 

COMPILER_FLAGS = -ansi -Wall
ifeq ($(COMPILER),gcc)
COMPILER_FLAGS += -std=c99
endif

ifeq ($(MAKECMDGOALS),debug)
COMPILER_FLAGS += -g -ggdb
endif

INCLUDES = -I.. -I./

LIBNAME = libicgdip_simplegraph.a

.PHONY: debug

../$(LIBNAME): simplegraph.o
	$(LINKER) ../$(LIBNAME) $^ 

simplegraph.o: simplegraph.c simplegraph.h
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDES) -c $< -o $@

debug: ../$(LIBNAME)

clean_obj:
	rm -f *.o

clean_bin:
	rm -f ../$(LIBNAME)

clean: clean_obj clean_bin
