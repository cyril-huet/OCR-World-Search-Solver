CC = gcc

LIBS  =  -lSDL2 -lSDL2_image -lm

# Options de compilation
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = $(LIBS)

# Fichiers sources et exécutables
SOURCES = convert.c extraction_de_lettre.c parcours.c resize.c reseaux.c test.c pgm_to_ppm.c colonne.c grid.c copy.c matrice.c trie.c
EXECUTABLES = convert extraction_de_lettre parcours resize reseaux test pgm_to_ppm colonne grid copy matrice trie

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
	touch matrice.txt

