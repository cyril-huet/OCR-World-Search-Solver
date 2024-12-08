#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour convertir une image PNG en PGM
int convert_image_to_pgm(const char* source_path, const char* dest_path) {
    // Initialisation de SDL et SDL_Image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }
    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur d'initialisation de SDL_Image : %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // Charger l'image source
    SDL_Surface* image = IMG_Load(source_path);
    if (!image) {
        fprintf(stderr, "Erreur de chargement de l'image : %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Ouvrir le fichier de destination
    FILE* output_file = fopen(dest_path, "w");
    if (!output_file) {
        fprintf(stderr, "Erreur d'ouverture du fichier de destination\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Écrire l'en-tête du fichier PGM
    fprintf(output_file, "P2\n%d %d\n255\n", image->w, image->h);

    // Convertir chaque pixel en niveau de gris et écrire dans le fichier
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            Uint32 pixel = ((Uint32*)image->pixels)[y * (image->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);

            // Conversion en niveaux de gris
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            // Écriture dans le fichier
            fprintf(output_file, "%d ", gray);
        }
        fprintf(output_file, "\n");
    }

    // Libération des ressources
    fclose(output_file);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// Exemple d'utilisation
int main() {
    const char* source_path = "image_source.png";
    const char* dest_path = "image.pgm";

    if (convert_image_to_pgm(source_path, dest_path) == 0) {
        printf("Conversion réussie !\n");
    } else {
        printf("Erreur lors de la conversion.\n");
    }

    return 0;
}

