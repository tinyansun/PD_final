CFLAGS = -std=c++11 -g  

SOURCES= $(wildcard src/*.cpp)
INCLUDES= $(wildcard src/*.h)
OBJECTS=$(SOURCES:.c=.o)

all : $(OBJECTS)
	g++ $(CFLAGS) $(OBJECTS) -lm -o ./bin/router
%.o:  %.c  ${INCLUDES}
	g++ $(CFLAGS) $< -o $@
clean:
	rm src/*.o bin/router	