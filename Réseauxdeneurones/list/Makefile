# Compiler and flags
CC = gcc
CFLAGS = -Werror -Wall -Wextra -g -lSDL2 -lSDL2_image -lm \
         -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/ \
         -L/opt/homebrew/lib

# Executables
EXECUTABLES = a.out exec extraire fin jade parcours parcoursextraire \
              convert_png_to_pgm pgm_to_png png_to_pgm fichier \
              charline charline4.1 process_files

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
SRC_CHARLINE = charline.c
SRC_CHARLINE4_1 = charline4.1.c
SRC_PROCESS_FILES = process_files.c

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

charline: $(SRC_CHARLINE)
	$(CC) $(CFLAGS) -o $@ $^

charline4.1: $(SRC_CHARLINE4_1)
	$(CC) $(CFLAGS) -o $@ $^

process_files: $(SRC_PROCESS_FILES)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up object files and executables
clean:
	rm -f *.o $(EXECUTABLES) 2>/dev/null || true
	rm -rf mot_png 2>/dev/null || true
	rm -rf images 2>/dev/null || true
	rm -rf parcours.dSYM 2>/dev/null || true
	rm -f image.pgm 2>/dev/null || true
	rm -f output_with_boxes.png 2>/dev/null || true
	rm -f output_with_boxes.pgm 2>/dev/null || true
	rm -f rectangles.txt 2>/dev/null || true
	rm -rf *.dSYM 2>/dev/null || true
	rm mot*
# PHONY targets
.PHONY: all clean

