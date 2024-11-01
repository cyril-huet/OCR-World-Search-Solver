CC = gcc
CFLAGS = -Wall -Werror -Wextra

separtion: detection_grille.c
	$(CC) $(CFLAGS) -o detection_grille detection_grille.c -lSDL2 -lSDL2_image -lm

.PHONY: clean
clean:
	rm -f detection_grille
