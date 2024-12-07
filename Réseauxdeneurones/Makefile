# Compiler et linker
CC = gcc

# Options pour les includes et les librairies
INCLUDES = -I/opt/homebrew/Cellar/sdl2/2.30.9/include/SDL2 \
           -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/SDL2 \
           -I/opt/homebrew/Cellar/sdl2_gfx/1.0.4/include

LIBS = -L/opt/homebrew/Cellar/sdl2_image/2.8.2_2/lib \
       -L/opt/homebrew/Cellar/sdl2_gfx/1.0.4/lib \
       -lSDL2 -lSDL2_image -lSDL2_gfx

# Options de compilation
CFLAGS = -Wall -Wextra -g $(INCLUDES)
LDFLAGS = $(LIBS)

# Fichiers sources et exécutables
SOURCES = convert.c extraction_de_lettre.c parcours.c resize.c reseaux.c test.c pgm_to_ppm.c colonne.c grid.c copy.c
EXECUTABLES = convert extraction_de_lettre parcours resize reseaux test pgm_to_ppm colonne grid copy

# Règle par défaut
all: $(EXECUTABLES)

# Règle générique pour créer les exécutables
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(EXECUTABLES) *.o
	rm -rf *.dSYM
	rm grid_only.ppm
	rm output.pgm
	rm -rf images
	rm output_with_boxes.ppm
	rm letters.txt
	rm matrice.txt
# Règle pour exécuter tout le pipeline
run: all
	./extraction_de_lettre grid_only.pgm
	./parcours
	./convert
	./resize
	./reseaux weights output.pgm images
	./test
	./pgm_to_ppm
	./colonne
	./grid
	./copy images/

