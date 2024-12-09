#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Fonction pour sauvegarder une texture dans un fichier PNG
int sauvegarder_renderer(SDL_Renderer *renderer, int largeur, int hauteur, const char *chemin_sauvegarde) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, largeur, hauteur, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        fprintf(stderr, "Erreur SDL_CreateRGBSurfaceWithFormat: %s\n", SDL_GetError());
        return -1;
    }

    // Lire les pixels du renderer dans la surface
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        fprintf(stderr, "Erreur SDL_RenderReadPixels: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return -1;
    }

    // Sauvegarder la surface dans un fichier PNG
    if (IMG_SavePNG(surface, chemin_sauvegarde) != 0) {
        fprintf(stderr, "Erreur IMG_SavePNG: %s\n", IMG_GetError());
        SDL_FreeSurface(surface);
        return -1;
    }

    SDL_FreeSurface(surface);
    printf("Image sauvegardée dans %s\n", chemin_sauvegarde);
    return 0;
}

// Fonction pour sauvegarder un rectangle spécifique d'une image
void sauvegarder_rectangle_image(const char *chemin_image, int x1, int y1, int x2, int y2, int index) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return;
    }

    SDL_Renderer *rendu = NULL;

    // Création d'un renderer virtuel (sans fenêtre)
    SDL_Window *fenetre = SDL_CreateWindow("Virtuelle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x2 - x1, y2 - y1, SDL_WINDOW_HIDDEN);
    if (!fenetre) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    rendu = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!rendu) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }

    // Chargement de l'image avec SDL_image
    SDL_Surface *surface = IMG_Load(chemin_image);
    if (!surface) {
        fprintf(stderr, "Erreur IMG_Load: %s\n", IMG_GetError());
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }

    // Création de la texture à partir de la surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(rendu, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }

    // Définir le rectangle source (en excluant une éventuelle bordure)
    int padding = 1; // Par exemple, exclure 2 pixels de bordure
    SDL_Rect rect_source = {x1 + padding, y1 + padding, (x2 - x1) - 2 * padding, (y2 - y1) - 2 * padding};

    // Vérifier que les dimensions sont valides
    if (rect_source.w <= 0 || rect_source.h <= 0) {
        fprintf(stderr, "Rectangle invalide (dimensions négatives ou nulles).\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }

    // Définir le rectangle de destination (dimensions finales)
    SDL_Rect rect_dest = {0, 0, rect_source.w, rect_source.h};

    // Créer une texture cible pour le rendu
    SDL_Texture *texture_cible = SDL_CreateTexture(rendu, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, rect_dest.w, rect_dest.h);
    if (!texture_cible) {
        fprintf(stderr, "Erreur SDL_CreateTexture: %s\n", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }

    // Définir la texture cible pour le renderer
    SDL_SetRenderTarget(rendu, texture_cible);

    // Copier la texture source sur la texture cible
    SDL_RenderClear(rendu);
    SDL_RenderCopy(rendu, texture, &rect_source, &rect_dest);

    // Construire le nom du fichier avec l'index dans le dossier mots
    char chemin_sauvegarde[256];
    snprintf(chemin_sauvegarde, sizeof(chemin_sauvegarde), "mot_png/mot%d.png", index);

    // Sauvegarder la texture cible
    if (sauvegarder_renderer(rendu, rect_dest.w, rect_dest.h, chemin_sauvegarde) != 0) {
        fprintf(stderr, "Échec de la sauvegarde de l'image.\n");
    }

    // Nettoyage des ressources
    SDL_DestroyTexture(texture_cible);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
}

int main() {
    // Créer le dossier "mots" s'il n'existe pas
    struct stat st = {0};
    if (stat("mot_png", &st) == -1) {
        if (mkdir("mot_png", 0700) != 0) {
            fprintf(stderr, "Erreur lors de la création du dossier 'mots'.\n");
            return -1;
        }
    }

    // Lecture du fichier des coordonnées
    FILE *fichier = fopen("rectangles.txt", "r");
    if (!fichier) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier rectangles.txt\n");
        return -1;
    }

    int x1, y1, x2, y2, index = 1;
    const char *chemin_image = "output_with_boxes.png";

    // Lire les coordonnées et sauvegarder les images correspondantes
    while (fscanf(fichier, "%d,%d,%d,%d\n", &x1, &y1, &x2, &y2) == 4) {
        // Appel de la fonction pour chaque rectangle trouvé dans le fichier
        sauvegarder_rectangle_image(chemin_image, x1, y1, x2, y2, index++);
    }

    fclose(fichier);
    return 0;
}

