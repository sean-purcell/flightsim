SOURCES=$(filter-out test.cpp opengltest.cpp quaterniontest.cpp movingtestground.cpp terraintest.cpp planetest.cpp flightsimulate.cpp, $(wildcard *.cpp))
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

.PHONY: all planetest opengltest terraintest

all: planetest opengltest

planetest: bin bin/planetest.o bin/openglutil.o bin/openglshaders.o bin/terrain.o bin/simplexnoise.o bin/aircraft.o bin/util.o bin/biome-processor.o bin/lodepng.o
	g++ -o bin/planetest $(filter-out $(word 1, $^), $^) $(LIB) $(FLAGS) $(LFLAGS) -flto

terraintest: bin bin/terraintest.o $(OBJECTS)
	g++ -o bin/terraintest $(word 2,$^) $(OBJECTS) $(LIB) $(FLAGS) $(LFLAGS) -flto

opengltest: bin bin/opengltest.o bin/openglutil.o bin/openglshaders.o bin/terrain.o bin/simplexnoise.o bin/biome-processor.o bin/lodepng.o
	g++ -o bin/opengltest $(filter-out $(word 1, $^), $^) $(LIB) $(FLAGS) $(LFLAGS) -flto

bin/%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(LIB)

bin:
	@mkdir bin

clean:
	rm -rf bin/

