INC = -I./include\
      -I./vendor

MAIN_SRC = $(wildcard ./src/*.cpp)

GLAD_SRC = ./vendor/glad/glad.c

IMGUI_SRC = $(wildcard ./vendor/ImGui/*.cpp)\
		    $(wildcard ./vendor/ImGui/backends/*.cpp)\

LIBRARIES = -lopengl32\
			-lglfw3dll\
			-limm32\
			-lassimp

all:
	g++ -std=c++17 $(INC) -L./lib -o engine $(MAIN_SRC) $(GLAD_SRC) $(IMGUI_SRC) $(LIBRARIES)

no_gui:
	g++ -std=c++17 $(INC) -L./lib -o engine $(MAIN_SRC) $(GLAD_SRC) $(LIBRARIES)

debug:
	g++ -g -Wall -std=c++17 $(INC) -L./lib -o engine $(MAIN_SRC) $(GLAD_SRC) $(LIBRARIES)