SOURCES=$(filter-out quaterniontest.cpp movingtestground.cpp terraintest.cpp planetest.cpp flightsimulate.cpp, $(wildcard *.cpp))
OBJECTS=$(patsubst %.cpp, bin/%.o, $(SOURCES))

LIB=-lsfml-window -lsfml-system -lsfml-graphics ${EXTRALIB}

FLAGS=-O3

planetest: bin bin/planetest.o $(OBJECTS)
	g++ -o bin/planetest $(word 2,$^) $(OBJECTS) $(LIB) $(FLAGS) -flto

terraintest: bin bin/terraintest.o $(OBJECTS)
	g++ -o bin/terraintest $(word 2,$^) $(OBJECTS) $(LIB) $(FLAGS) -flto

bin/%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS)

bin:
	@mkdir bin

clean:
	rm -rf bin/

