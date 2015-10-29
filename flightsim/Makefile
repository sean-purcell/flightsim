SOURCES=$(filter-out opengltest.cpp quaterniontest.cpp movingtestground.cpp terraintest.cpp planetest.cpp flightsimulate.cpp, $(wildcard *.cpp))
OBJECTS=$(patsubst %.cpp, bin/%.o, $(SOURCES))

LIB=-lsfml-window -lsfml-system -lsfml-graphics -L/usr/local/lib -Iinc/ $(EXTRALIB)

FLAGS=-g -std=c++11 -Wno-deprecated-declarations

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	FLAGS+= -lGL
	LIB+= -L/usr/X11R6/lib -lGLEW -lGLU -lGL -lglut
endif
ifeq ($(UNAME_S),Darwin)
	FLAGS+= -framework OpenGL -framework GLUT
endif

.PHONY: all planetest opengltest terraintest

all: planetest terraintest opengltest

planetest: bin bin/planetest.o $(OBJECTS)
	g++ -o bin/planetest $(word 2,$^) $(OBJECTS) $(LIB) $(FLAGS) -flto

terraintest: bin bin/terraintest.o $(OBJECTS)
	g++ -o bin/terraintest $(word 2,$^) $(OBJECTS) $(LIB) $(FLAGS) -flto

opengltest: bin bin/opengltest.o
	g++ -o bin/opengltest $(word 2,$^) $(LIB) $(FLAGS) -flto

bin/%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(LIB)

bin:
	@mkdir bin

clean:
	rm -rf bin/

