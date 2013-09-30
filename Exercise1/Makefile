CC = g++
VERSION = LINUX

ifneq ($(MAKECMDGOALS),debug)
CPP_MODE =   -ansi -fpic -finline -Wall -pipe
else
CPP_MODE =  -fno-inline -Wall -pipe -ggdb
endif

CPPFLAGS =  -D$(VERSION) $(CPP_MODE)

SOURCE = PlanetsGraph.c
TARGET = $(SOURCE:.c=)

INCLUDES = -I./

LIBS	=  -L./ -licgip_math -licgdip_simplegraph -lGL -lGLU -lglut

REC_MAKES = math/ simplegraph/

.SILENT: 

.PHONY: clean all rec_makes $(REC_MAKES)

all: rec_makes $(TARGET)

$(TARGET): $(SOURCE) libicgip_math.a libicgdip_simplegraph.a
	$(CC) $< $(CPPFLAGS) -o $@ $(LIBS) $(INCLUDES)

clean: rec_makes
	rm $(TARGET)


rec_makes: $(REC_MAKES)

$(REC_MAKES):
	cd $@; $(MAKE) $(MAKECMDGOALS)

