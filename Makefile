LD_DEBUG=all
all:
	g++ -std=c++17 -I./include -L./lib -o engine $(wildcard ./src/*.cpp) ./src/glad.c -lopengl32 -lglfw3dll