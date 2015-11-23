SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp, bin/%.o, $(SOURCES))

LIB=-Iinc/ $(EXTRALIB)

FLAGS=-g -O3 -std=c++11 -Wno-deprecated-declarations
LFLAGS= -L/usr/local/lib -lboost_system -lboost_system-mt

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LFLAGS+= -L/usr/X11R6/lib -lGLEW -lGLU -lGL -lglut
	FLAGS+=LFLAGS
endif
ifeq ($(UNAME_S),Darwin)
	LFLAGS+= -framework OpenGL -framework GLUT
endif

.PHONY: all flightsim

flightsim: bin $(OBJECTS)
	g++ -o $@ $(filter-out $(word 1, $^), $^) $(LIB) $(FLAGS) $(LFLAGS) -flto

bin/%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(LIB)

bin:
	@mkdir bin

clean:
	rm -rf bin/

