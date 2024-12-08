#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

// Fonction pour convertir une image PNG en PGM
void convert_png_to_pgm(const char *png_path, const char *pgm_path) {
    SDL_Surface *image = IMG_Load(png_path);
    if (!image) {
        fprintf(stderr, "Erreur de chargement de l'image %s: %s\n", png_path, IMG_GetError());
        return;
    }

    FILE *pgm_file = fopen(pgm_path, "wb");
    if (!pgm_file) {
        fprintf(stderr, "Erreur lors de la création du fichier %s.\n", pgm_path);
        SDL_FreeSurface(image);
        return;
    }

    // Écrire l'en-tête PGM
    fprintf(pgm_file, "P5\n%d %d\n255\n", image->w, image->h);

    // Vérifier si l'image est déjà en format 8 bits (grayscale)
    if (image->format->BytesPerPixel == 1) {
        fwrite(image->pixels, 1, image->w * image->h, pgm_file);
    } else {
        // Si l'image n'est pas en 8 bits, convertir les pixels en niveaux de gris
        uint8_t *grayscale_data = (uint8_t *)malloc(image->w * image->h);
        if (!grayscale_data) {
            fprintf(stderr, "Erreur d'allocation mémoire pour les données grayscale.\n");
            fclose(pgm_file);
            SDL_FreeSurface(image);
            return;
        }

        uint32_t *pixels = (uint32_t *)image->pixels;
        for (int y = 0; y < image->h; y++) {
            for (int x = 0; x < image->w; x++) {
                uint32_t pixel = pixels[y * image->w + x];
                uint8_t r = (pixel & image->format->Rmask) >> image->format->Rshift;
                uint8_t g = (pixel & image->format->Gmask) >> image->format->Gshift;
                uint8_t b = (pixel & image->format->Bmask) >> image->format->Bshift;
                grayscale_data[y * image->w + x] = (uint8_t)((r + g + b) / 3);
            }
        }

        fwrite(grayscale_data, 1, image->w * image->h, pgm_file);
        free(grayscale_data);
    }

    fclose(pgm_file);
    SDL_FreeSurface(image);

}

// Fonction pour parcourir le dossier "images" et convertir PNG -> PGM
void convert_images_in_directory(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le dossier %s.\n", directory);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Vérifier si le fichier est un PNG
        const char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".png") == 0) {
            // Construire les chemins complets pour le fichier PNG et le fichier PGM
            char png_path[512];
            char pgm_path[512];
            snprintf(png_path, sizeof(png_path), "%s/%s", directory, entry->d_name);

            snprintf(pgm_path, sizeof(pgm_path), "%s/%s", directory, entry->d_name);
            pgm_path[strlen(pgm_path) - 4] = '\0'; // Supprimer ".png"
            strcat(pgm_path, ".pgm");              // Ajouter ".pgm"

            // Convertir le fichier PNG en PGM
            convert_png_to_pgm(png_path, pgm_path);

            // Supprimer le fichier PNG après conversion
            if (remove(png_path) != 0) {
                fprintf(stderr, "Erreur lors de la suppression du fichier %s.\n", png_path);
            } 
        }
    }

    closedir(dp);
}

int main() {
    const char *directory = "images";

    // Vérifier si SDL est initialisé correctement
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Vérifier si SDL_image est initialisé correctement
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Convertir toutes les images dans le dossier
    convert_images_in_directory(directory);

    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

