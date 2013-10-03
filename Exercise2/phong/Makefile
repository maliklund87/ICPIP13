CC = g++
VERSION = LINUX

ifneq ($(MAKECMDGOALS),debug)
CPP_MODE =   -ansi -fpic -finline -Wall -pipe
else
CPP_MODE =  -fno-inline -Wall -pipe -ggdb
endif

CPPFLAGS =  -D$(VERSION) $(CPP_MODE)

SOURCE = phong.c
TARGET = $(SOURCE:.c=)

INCLUDES = 

LIBS	=  -lGLEW -lGL -lGLU -lglut -lpthread -lXext -lm

.SILENT: 

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $< $(CPPFLAGS) -o $@ $(LIBS) $(INCLUDES)
clean:
	rm $(TARGET)


