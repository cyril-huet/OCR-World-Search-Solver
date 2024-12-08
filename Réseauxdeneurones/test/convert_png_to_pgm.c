#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Crée un dossier s'il n'existe pas
void create_output_directory(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0700) != 0) {
            fprintf(stderr, "Erreur lors de la création du dossier %s.\n", dir_name);
            exit(1);
        }
    }
}

// Convertit une image PNG en PGM et la sauvegarde
void convert_png_to_pgm(const char *input_path, const char *output_path) {
    SDL_Surface *surface = IMG_Load(input_path);
    if (!surface) {
        fprintf(stderr, "Erreur lors de la lecture du fichier %s : %s\n", input_path, IMG_GetError());
        return;
    }

    // Si l'image n'est pas en niveaux de gris (8 bits), la convertir
    SDL_Surface *grayscale_surface = SDL_CreateRGBSurface(0, surface->w, surface->h, 8, 0, 0, 0, 0);
    if (!grayscale_surface) {
        fprintf(stderr, "Erreur lors de la création de l'image en niveaux de gris : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    // Créer une palette en niveaux de gris
    SDL_Color palette[256];
    for (int i = 0; i < 256; i++) {
        palette[i].r = i;
        palette[i].g = i;
        palette[i].b = i;
        palette[i].a = 255;
    }
    SDL_SetPaletteColors(grayscale_surface->format->palette, palette, 0, 256);

    // Copier les pixels en niveaux de gris
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Convertir en gris : formule de luminance (pondération pour la perception humaine)
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            ((Uint8 *)grayscale_surface->pixels)[y * grayscale_surface->w + x] = gray;
        }
    }

    FILE *fp = fopen(output_path, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur lors de la création du fichier %s.\n", output_path);
        SDL_FreeSurface(surface);
        SDL_FreeSurface(grayscale_surface);
        return;
    }

    // Écriture de l'en-tête PGM
    fprintf(fp, "P5\n%d %d\n255\n", grayscale_surface->w, grayscale_surface->h);
    fwrite(grayscale_surface->pixels, 1, grayscale_surface->w * grayscale_surface->h, fp);

    fclose(fp);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(grayscale_surface);

    printf("Converti : %s -> %s\n", input_path, output_path);
}

// Parcourt un dossier pour convertir tous les fichiers PNG en PGM
void process_directory(const char *input_dir, const char *output_dir) {
    create_output_directory(output_dir);

    DIR *dir = opendir(input_dir);
    if (!dir) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le dossier %s.\n", input_dir);
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".png")) {
            char input_path[1024];
            char output_path[1024];

            snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);
            snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, entry->d_name);
            // Remplace l'extension .png par .pgm
            strcpy(strrchr(output_path, '.'), ".pgm");

            convert_png_to_pgm(input_path, output_path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Utilisation : %s <dossier_entree> <dossier_sortie>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL_image : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    process_directory(argv[1], argv[2]);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
