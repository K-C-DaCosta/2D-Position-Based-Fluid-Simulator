CC=g++
CFLAGS= -lm -std=c++11 -Wall -g3 -O3
LIBS= -lglfw  -lGL -lGLU -lGLEW -lm -lOpenCL

BIN_NAME=pbd.exe
SRC_DIR = ./src

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(SRC:.cpp=.o)
OBJ := $(subst src,obj, $(OBJ))

INCLUDE = $(wildcard ./include/*.h)
INCLUDE := $(INCLUDE) $(wildcard ./include/*.hpp);

all: build

run:
	./bin/$(BIN_NAME)


build: $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o ./bin/$(BIN_NAME)

obj/%.o: src/%.cpp $(INCLUDE)
	$(CC) -c $(CFLAGS) $< -o $@

dumpAsm: 
	g++ $(CFLAGS) -S ./src/*.cpp  && mv *.s ./asm
