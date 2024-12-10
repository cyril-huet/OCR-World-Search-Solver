#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour lire un fichier PGM (format P5) et le charger dans une surface SDL
SDL_Surface *load_pgm_as_surface(const char *input_file) {
    FILE *fp = fopen(input_file, "rb");
    if (!fp) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", input_file);
        return NULL;
    }

    // Lire l'en-tête PGM
    char magic[3];
    int width, height, maxval;
    if (fscanf(fp, "%2s\n%d %d\n%d\n", magic, &width, &height, &maxval) != 4 || strcmp(magic, "P5") != 0) {
        fprintf(stderr, "Erreur : le fichier %s n'est pas un PGM valide\n", input_file);
        fclose(fp);
        return NULL;
    }

    if (maxval != 255) {
        fprintf(stderr, "Erreur : seul maxval=255 est pris en charge pour les fichiers PGM\n");
        fclose(fp);
        return NULL;
    }

    // Allouer un buffer pour les pixels
    Uint8 *pixels = malloc(width * height);
    if (!pixels) {
        fprintf(stderr, "Erreur : mémoire insuffisante pour charger l'image\n");
        fclose(fp);
        return NULL;
    }

    // Lire les pixels
    if (fread(pixels, 1, width * height, fp) != (size_t)(width * height)) {
        fprintf(stderr, "Erreur : problème lors de la lecture des pixels du fichier %s\n", input_file);
        free(pixels);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    // Créer une surface SDL
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels, width, height, 8, width, SDL_PIXELFORMAT_INDEX8);
    if (!surface) {
        fprintf(stderr, "Erreur : impossible de créer une surface SDL (%s)\n", SDL_GetError());
        free(pixels);
        return NULL;
    }

    // Créer une palette de niveaux de gris
    SDL_Color palette[256];
    for (int i = 0; i < 256; i++) {
        palette[i].r = palette[i].g = palette[i].b = i;
        palette[i].a = 255;
    }
    SDL_SetPaletteColors(surface->format->palette, palette, 0, 256);

    return surface;
}

// Fonction pour convertir un PGM en PNG
void pgm_to_png(const char *input_file, const char *output_file) {
    // Charger l'image PGM dans une surface SDL
    SDL_Surface *surface = load_pgm_as_surface(input_file);
    if (!surface) {
        return;
    }

    // Sauvegarder la surface SDL en PNG
    if (IMG_SavePNG(surface, output_file) != 0) {
        fprintf(stderr, "Erreur : impossible de sauvegarder l'image en PNG (%s)\n", IMG_GetError());
    }

    // Libérer les ressources
    SDL_FreeSurface(surface);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <input.pgm> <output.png>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialiser SDL2 et SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur : impossible d'initialiser SDL (%s)\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur : impossible d'initialiser SDL_image pour PNG (%s)\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Convertir le fichier
    pgm_to_png(argv[1], argv[2]);

    // Quitter SDL
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

