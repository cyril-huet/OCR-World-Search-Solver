# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Libraries and include paths
LIBS = -lSDL2 -lSDL2_image -lm -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/ -L/opt/homebrew/lib

# List of source files
SOURCES = reseaux.c matrice.c trie.c script.c pngtopgmcharline.c extraction_letter_tric.c charline4.1.c resize.c charline.c colonne.c convert.c convert_png_to_pgm.c copy.c executable.c extraction_de_lettre.c facile.c fichier.c fin.c grid.c parcours.c pgm_to_png.c pgm_to_ppm.c trait.c retirepetit.c

# Generate a list of executables (one per source file)
EXECUTABLES = $(SOURCES:.c=)

# Default target: build all executables
all: $(EXECUTABLES)

# Rule to build each executable
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

# Clean build files and temporary files
clean:
	rm -rf $(EXECUTABLES) *.o *.pgm *.ppm *.png grid_only.pgm image_source.pgm image_source.png \
			finale.txt Jade letters.txt input.pgm matrice.txt output.pgm output_with_boxes.pgm output_with_boxes.png rectangles.txt
	rm -rf *.dSYM images/ mot_png/

# Phony targets
.PHONY: all clean

