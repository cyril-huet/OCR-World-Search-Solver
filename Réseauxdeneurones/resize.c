#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>

void augmenter_pixel(const char *image_path, int new_width, int new_height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Charger l'image
    SDL_Surface *image = IMG_Load(image_path);
    if (!image) {
        fprintf(stderr, "Erreur de chargement de l'image: %s\n", IMG_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Afficher les dimensions de l'image originale

    int original_width = image->w;
    int original_height = image->h;

    // Vérification des dimensions
    if (new_width < original_width || new_height < original_height) {
        fprintf(stderr, "Erreur : les nouvelles dimensions doivent être supérieures ou égales à celles de l'image originale.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Créer la nouvelle surface avec un fond 32 bits
    SDL_Surface *new_surface = SDL_CreateRGBSurface(0, new_width, new_height, 32, 
                                                     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!new_surface) {
        fprintf(stderr, "Erreur de création de la nouvelle surface: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Remplir la surface avec un fond blanc
    SDL_FillRect(new_surface, NULL, SDL_MapRGB(new_surface->format, 255, 255, 255));

    // Calculer l'offset pour centrer l'image originale
    int offset_x = (new_width - original_width) / 2;
    int offset_y = (new_height - original_height) / 2;

    SDL_Rect dest_rect = { offset_x, offset_y, original_width, original_height };

    // Copier l'image originale sur la nouvelle surface
    if (SDL_BlitSurface(image, NULL, new_surface, &dest_rect) != 0) {
        fprintf(stderr, "Erreur lors de la copie de l'image: %s\n", SDL_GetError());
    } else {
        printf("Image copiée avec succès.\n");
    }

    // Supprimer l'ancien fichier
    if (remove(image_path) != 0) {
        fprintf(stderr, "Erreur lors de la suppression du fichier original %s.\n", image_path);
        SDL_FreeSurface(image);
        SDL_FreeSurface(new_surface);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Sauvegarder la nouvelle image sous le même nom
    if (IMG_SavePNG(new_surface, image_path) != 0) {
        fprintf(stderr, "Erreur lors de l'enregistrement de l'image: %s\n", IMG_GetError());
    } 

    // Libérer les ressources
    SDL_FreeSurface(image);
    SDL_FreeSurface(new_surface);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <image_path> <new_width> <new_height>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    int new_width = atoi(argv[2]);
    int new_height = atoi(argv[3]);

    augmenter_pixel(image_path, new_width, new_height);

    return EXIT_SUCCESS;
}

