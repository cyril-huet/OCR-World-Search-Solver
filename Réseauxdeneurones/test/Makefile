# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Libraries and include paths
LIBS = -lSDL2 -lSDL2_image -lm -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/ -L/opt/homebrew/lib

# List of source files
SOURCES = script.c pngtopgmcharline.c extraction_letter_tric.c charline4.1.c  resize.c charline.c colonne.c convert.c convert_png_to_pgm.c copy.c executable.c extraction_de_lettre.c facile.c fichier.c fin.c grid.c parcours.c pgm_to_png.c pgm_to_ppm.c trait.c

# Generate a list of executables (one per source file)
EXECUTABLES = $(SOURCES:.c=)

# Default target: build all executables
all: $(EXECUTABLES)

# Rule to build each executable
%: %.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

# Clean build files and .dSYM folders
clean:
	rm -f $(EXECUTABLES)
	rm -rf *.dSYM
	rm -f *.o
	rm -f grid_only.pgm
	rm -f image_source.pgm
	rm -f image_source.png
	rm -f input.pgm
	rm -f matrice.txt
	rm -f output.pgm
	rm -f output_with_boxes.pgm
	rm -f output_with_boxes.png
	rm -f rectangles.txt
	rm -rf images/
	rm -rf mot_png/

# Phony targets
.PHONY: all clean

