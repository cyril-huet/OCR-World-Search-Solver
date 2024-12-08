# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -lSDL2 -lSDL2_image -lm  -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/ -L/opt/homebrew/lib
# Executables
EXECUTABLES = a.out exec extraire fin jade parcours parcoursextraire \
              convert_png_to_pgm pgm_to_png png_to_pgm fichier

# Sources
SRC_EXTRAIRE = extraire.c
SRC_FIN = fin.c
SRC_JADE = jade.c
SRC_PARCOURS = parcours.c
SRC_PARCOURSEXTRAIRE = parcoursextraire.c
SRC_CONVERT_PNG_TO_PGM = convert_png_to_pgm.c
SRC_PGM_TO_PNG = pgm_to_png.c
SRC_PNG_TO_PGM = png_to_pgm.c
SRC_EXEC = exec.c
SRC_FICHIER = fichier.c

# Object files
OBJ_EXTRAIRE = $(SRC_EXTRAIRE:.c=.o)
OBJ_FIN = $(SRC_FIN:.c=.o)
OBJ_JADE = $(SRC_JADE:.c=.o)
OBJ_PARCOURS = $(SRC_PARCOURS:.c=.o)
OBJ_PARCOURSEXTRAIRE = $(SRC_PARCOURSEXTRAIRE:.c=.o)
OBJ_CONVERT_PNG_TO_PGM = $(SRC_CONVERT_PNG_TO_PGM:.c=.o)
OBJ_PGM_TO_PNG = $(SRC_PGM_TO_PNG:.c=.o)
OBJ_PNG_TO_PGM = $(SRC_PNG_TO_PGM:.c=.o)
OBJ_EXEC = $(SRC_EXEC:.c=.o)
OBJ_FICHIER = $(SRC_FICHIER:.c=.o)

# Default target
all: $(EXECUTABLES)

# Individual targets
a.out: $(SRC_EXTRAIRE)
	$(CC) $(CFLAGS) -o $@ $^

exec: $(SRC_EXEC)
	$(CC) $(CFLAGS) -o $@ $^

extraire: $(SRC_EXTRAIRE)
	$(CC) $(CFLAGS) -o $@ $^

fin: $(SRC_FIN)
	$(CC) $(CFLAGS) -o $@ $^

jade: $(SRC_JADE)
	$(CC) $(CFLAGS) -o $@ $^

parcours: $(SRC_PARCOURS)
	$(CC) $(CFLAGS) -o $@ $^

parcoursextraire: $(SRC_PARCOURSEXTRAIRE)
	$(CC) $(CFLAGS) -o $@ $^

convert_png_to_pgm: $(SRC_CONVERT_PNG_TO_PGM)
	$(CC) $(CFLAGS) -o $@ $^

pgm_to_png: $(SRC_PGM_TO_PNG)
	$(CC) $(CFLAGS) -o $@ $^

png_to_pgm: $(SRC_PNG_TO_PGM)
	$(CC) $(CFLAGS) -o $@ $^

fichier: $(SRC_FICHIER)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up object files and executables
clean:
	rm -f *.o $(EXECUTABLES)
	rm -rf mot_png
	rm -rf images
	rm -rf parcours.dSYM
	rm image.pgm
	rm output_with_boxes.png
	rm output_with_boxes.pgm
	rm rectangles.txt
	rm -rf *.dSYM
# PHONY targets
.PHONY: all clean

