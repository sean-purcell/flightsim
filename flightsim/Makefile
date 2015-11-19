SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp, bin/%.o, $(SOURCES))

LIB=-Iinc/ $(EXTRALIB)

FLAGS=-O3 -std=c++11 -Wno-deprecated-declarations
LFLAGS= -lsfml-window -lsfml-system -lsfml-graphics -L/usr/local/lib

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LFLAGS+= -L/usr/X11R6/lib -lGLEW -lGLU -lGL -lglut
	FLAGS+=LFLAGS
endif
ifeq ($(UNAME_S),Darwin)
	LFLAGS+= -framework OpenGL -framework GLUT
endif

.PHONY: all bin/planetest

all: bin/planetest

bin/planetest: bin $(OBJECTS)
	g++ -o bin/planetest $(filter-out $(word 1, $^), $^) $(LIB) $(FLAGS) $(LFLAGS) -flto

bin/%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(LIB)

bin:
	@mkdir bin

clean:
	rm -rf bin/

