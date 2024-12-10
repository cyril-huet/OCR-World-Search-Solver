# Variables
CC = gcc
SRC = link_pretraitement.c
OUTPUT = link_pretraitement

# Cibles principales
.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) -o $@ $^ -lSDL2 -lSDL2_image -lm

clean:
	rm -f $(OUTPUT)

