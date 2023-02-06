CC=g++
CFLAGS=-Wall -fexceptions -O2 -std=c++11 -c
LDFLAGS=-s
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/GotoNewXYZ

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@-mkdir bin
	$(CC) obj/$(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	@-mkdir obj
	$(CC) $(CFLAGS) $< -o obj/$@
