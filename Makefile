LD_DEBUG=all

INC = -I./include\
      -I./vendor

MAIN_SRC = $(wildcard ./src/*.cpp)
VEND_SRC = ./vendor/glad/glad.c


all:
	g++ -std=c++17 $(INC) -L./lib -o engine $(MAIN_SRC) $(VEND_SRC) -lopengl32 -lglfw3dll