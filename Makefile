INC = -I./include\
      -I./vendor

MAIN_SRC = $(wildcard ./src/*.cpp)
VEND_SRC = ./vendor/glad/glad.c\
		   $(wildcard ./vendor/ImGui/*.cpp)\
		   $(wildcard ./vendor/ImGui/backends/*.cpp)\

all:
	g++ -std=c++17 $(INC) -L./lib -o engine $(MAIN_SRC) $(VEND_SRC) -lopengl32 -lglfw3dll -limm32