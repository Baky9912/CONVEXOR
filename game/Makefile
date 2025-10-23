OUT = ./obj/main.exe
GLFW_DIR = C:/Program Files (x86)/GLFW

CC = gcc

SRC_DIR = src
GEOMETRY_DIR = $(SRC_DIR)/geometry
GAME_DIR = $(SRC_DIR)/game

IN = main.c \
     $(SRC_DIR)/main_state.c \
     $(SRC_DIR)/glad/glad.c \
     $(GEOMETRY_DIR)/myrandom.c \
     $(GEOMETRY_DIR)/segment.c \
     $(GEOMETRY_DIR)/geometry.c \
     $(GEOMETRY_DIR)/convexhull.c \
     $(GEOMETRY_DIR)/concavehull.c \
     $(GEOMETRY_DIR)/polygon.c \
     $(GAME_DIR)/camera.c \
     $(GAME_DIR)/globals.c \
     $(GAME_DIR)/food_shards.c \
     $(GAME_DIR)/hostile_mob.c \
     $(GAME_DIR)/protagonist.c

GLFW_INC = $(GLFW_DIR)/include
GLFW_LIB = $(GLFW_DIR)/lib

IFLAGS = -I. -I./include -I./src/include
CFLAGS = -Wall -DGLFW_INCLUDE_NONE $(IFLAGS) -I"$(GLFW_INC)"
LFLAGS = -L"$(GLFW_LIB)" -lglfw3 -lopengl32 -lgdi32 -lm

.SILENT: all

all: clean build run

clean:
	@if exist .\obj\main.exe del .\obj\main.out

build: $(IN)
	$(CC) -v $(IN) -o $(OUT) $(CFLAGS) $(IFLAGS) $(LFLAGS)

run: $(OUT)
	./$(OUT)