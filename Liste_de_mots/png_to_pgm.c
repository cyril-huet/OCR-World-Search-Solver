#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour convertir une image PNG en PGM
void png_to_pgm(const char *input_file, const char *output_file) {
    // Initialiser SDL et SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur : impossible d'initialiser SDL (%s)\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur : impossible d'initialiser SDL_image pour PNG (%s)\n", IMG_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Charger l'image PNG
    SDL_Surface *image = IMG_Load(input_file);
    if (!image) {
        fprintf(stderr, "Erreur : impossible de charger l'image %s (%s)\n", input_file, IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Vérifier que l'image est en niveau de gris ou convertir
    if (image->format->BytesPerPixel != 1) {
        fprintf(stderr, "Erreur : l'image doit être en niveau de gris (1 canal par pixel).\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier de sortie
    FILE *fp = fopen(output_file, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s pour écriture.\n", output_file);
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Écrire l'en-tête PGM
    fprintf(fp, "P5\n%d %d\n255\n", image->w, image->h);

    // Écrire les pixels dans le fichier PGM
    fwrite(image->pixels, 1, image->w * image->h, fp);

    // Fermer le fichier et libérer les ressources
    fclose(fp);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    printf("Conversion réussie : %s -> %s\n", input_file, output_file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <input.png> <output.pgm>\n", argv[0]);
        return EXIT_FAILURE;
    }

    png_to_pgm(argv[1], argv[2]);

    return EXIT_SUCCESS;
}

