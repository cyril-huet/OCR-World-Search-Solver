#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <SDL2/SDL.h>

#define MIN_WIDTH 7
#define MIN_HEIGHT 7

void remove_small_images(const char *directory) {
    DIR *dir = opendir(directory);
    struct dirent *entry;
    char filepath[1024];

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Vérifier que le fichier commence par "letter"
        if (strncmp(entry->d_name, "letter", 6) == 0) {
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

            // Charger l'image avec SDL
            SDL_Surface *image = SDL_LoadBMP(filepath);
            if (image == NULL) {
                fprintf(stderr, "Impossible de charger l'image %s : %s\n", filepath, SDL_GetError());
                continue;
            }

            // Vérifier si la largeur ou la hauteur est inférieure à 7
            if (image->w <= MIN_WIDTH || image->h <= MIN_HEIGHT) {
                printf("Suppression de l'image %s (taille %dx%d)\n", filepath, image->w, image->h);
                if (remove(filepath) != 0) {
                    perror("Erreur lors de la suppression");
                }
            }

            // Libérer la surface SDL
            SDL_FreeSurface(image);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <repertoire>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Parcourir et supprimer les petites images
    remove_small_images(argv[1]);

    // Quitter SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}

