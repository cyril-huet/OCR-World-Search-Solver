#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define FINAL_SIZE 28 // Taille de l'image de sortie (28x28)

SDL_Surface* resize_to_28x28(SDL_Surface* input_surface) {
    if (!input_surface) {
        fprintf(stderr, "Erreur : surface d'entrée invalide.\n");
        return NULL;
    }

    // Obtenir les dimensions de l'image originale
    int original_width = input_surface->w;
    int original_height = input_surface->h;

    // Créer une surface de sortie carrée 28x28
    SDL_Surface* final_surface = SDL_CreateRGBSurfaceWithFormat(
        0, FINAL_SIZE, FINAL_SIZE, 32, SDL_PIXELFORMAT_RGBA32);

    if (!final_surface) {
        fprintf(stderr, "Erreur lors de la création de la surface finale : %s\n", SDL_GetError());
        return NULL;
    }

    // Vérifier et convertir la surface source au format destination
    SDL_Surface* converted_surface = SDL_ConvertSurface(input_surface, final_surface->format, 0);
    if (!converted_surface) {
        fprintf(stderr, "Erreur lors de la conversion de la surface : %s\n", SDL_GetError());
        SDL_FreeSurface(final_surface);
        return NULL;
    }

    // Calculer l'échelle pour maintenir le ratio d'aspect
    float scale = fmin((float)FINAL_SIZE / original_width, (float)FINAL_SIZE / original_height);

    // Calculer les dimensions redimensionnées tout en maintenant le ratio
    SDL_Rect dest_rect;
    dest_rect.w = (int)(original_width * scale);
    dest_rect.h = (int)(original_height * scale);
    dest_rect.x = (FINAL_SIZE - dest_rect.w) / 2;
    dest_rect.y = (FINAL_SIZE - dest_rect.h) / 2;

    // Remplir la surface de sortie avec un fond blanc
    SDL_FillRect(final_surface, NULL, SDL_MapRGBA(final_surface->format, 255, 255, 255, 255));

    // Redimensionner et copier l'image d'entrée dans la surface finale
    if (SDL_BlitScaled(converted_surface, NULL, final_surface, &dest_rect) != 0) {
        fprintf(stderr, "Erreur lors du redimensionnement de l'image : %s\n", SDL_GetError());
        SDL_FreeSurface(final_surface);
        SDL_FreeSurface(converted_surface);
        return NULL;
    }

    SDL_FreeSurface(converted_surface);
    return final_surface;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_image> <output_image>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_path = argv[1];
    const char* output_path = argv[2];

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Charger l'image d'entrée
    SDL_Surface* input_surface = IMG_Load(input_path);
    if (!input_surface) {
        fprintf(stderr, "Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Redimensionner l'image
    SDL_Surface* resized_surface = resize_to_28x28(input_surface);
    if (!resized_surface) {
        SDL_FreeSurface(input_surface);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Sauvegarder l'image redimensionnée
    if (IMG_SavePNG(resized_surface, output_path) != 0) {
        fprintf(stderr, "Erreur lors de l'enregistrement de l'image : %s\n", IMG_GetError());
        SDL_FreeSurface(input_surface);
        SDL_FreeSurface(resized_surface);
        SDL_Quit();
        return EXIT_FAILURE;
    }


    // Libérer les ressources
    SDL_FreeSurface(input_surface);
    SDL_FreeSurface(resized_surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}
