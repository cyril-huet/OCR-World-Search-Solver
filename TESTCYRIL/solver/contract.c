#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


int combine_images(const char *image1_path, const char *image2_path, const char *output_path) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image1 = IMG_Load(image1_path);
    if (!image1) {
        printf("Erreur lors du chargement de l'image 1: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image2 = IMG_Load(image2_path);
    if (!image2) {
        printf("Erreur lors du chargement de l'image 2: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Dimensions pour la nouvelle image
    int width = image1->w + image2->w;
    int height = (image1->h > image2->h) ? image1->h : image2->h;

    SDL_Surface *combined_image = SDL_CreateRGBSurface(0, width, height, 32,
                                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!combined_image) {
        printf("Erreur lors de la création de la surface combinée: %s\n", SDL_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Copier image 2 (à gauche)
    SDL_Rect dest_rect2 = {0, 0, image2->w, image2->h};
    SDL_BlitSurface(image2, NULL, combined_image, &dest_rect2);

    // Copier image 1 (à droite)
    SDL_Rect dest_rect1 = {image2->w, 0, image1->w, image1->h};
    SDL_BlitSurface(image1, NULL, combined_image, &dest_rect1);

    // Sauvegarder l'image combinée
    if (IMG_SavePNG(combined_image, output_path) != 0) {
        printf("Erreur lors de la sauvegarde de l'image combinée: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        SDL_FreeSurface(combined_image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);
    SDL_FreeSurface(combined_image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

int combine_images_vertical(const char *image1_path, const char *image2_path, const char *output_path) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image1 = IMG_Load(image1_path);
    if (!image1) {
        printf("Erreur lors du chargement de l'image 1: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image2 = IMG_Load(image2_path);
    if (!image2) {
        printf("Erreur lors du chargement de l'image 2: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Dimensions pour la nouvelle image
    int width = (image1->w > image2->w) ? image1->w : image2->w;
    int height = image1->h + image2->h;

    SDL_Surface *combined_image = SDL_CreateRGBSurface(0, width, height, 32,
                                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!combined_image) {
        printf("Erreur lors de la création de la surface combinée: %s\n", SDL_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Copier image 1 (en haut)
    SDL_Rect dest_rect1 = {0, 0, image1->w, image1->h};
    SDL_BlitSurface(image1, NULL, combined_image, &dest_rect1);

    // Copier image 2 (en bas)
    SDL_Rect dest_rect2 = {0, image1->h, image2->w, image2->h};
    SDL_BlitSurface(image2, NULL, combined_image, &dest_rect2);

    // Sauvegarder l'image combinée
    if (IMG_SavePNG(combined_image, output_path) != 0) {
        printf("Erreur lors de la sauvegarde de l'image combinée: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        SDL_FreeSurface(combined_image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);
    SDL_FreeSurface(combined_image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

int combine_images_vertical_centered(const char *image1_path, const char *image2_path, const char *output_path) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image1 = IMG_Load(image1_path);
    if (!image1) {
        printf("Erreur lors du chargement de l'image 1: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image2 = IMG_Load(image2_path);
    if (!image2) {
        printf("Erreur lors du chargement de l'image 2: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Dimensions pour la nouvelle image
    int width = (image1->w > image2->w) ? image1->w : image2->w;
    int height = image1->h + image2->h;

    SDL_Surface *combined_image = SDL_CreateRGBSurface(0, width, height, 32,
                                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!combined_image) {
        printf("Erreur lors de la création de la surface combinée: %s\n", SDL_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Remplir la surface combinée avec du blanc
    SDL_FillRect(combined_image, NULL, SDL_MapRGB(combined_image->format, 255, 255, 255));

    // Copier image 1 (en haut)
    SDL_Rect dest_rect1 = {0, 0, image1->w, image1->h};
    SDL_BlitSurface(image1, NULL, combined_image, &dest_rect1);

    // Calcul pour centrer l'image 2 (en bas)
    int centered_x = (width - image2->w) / 2;
    SDL_Rect dest_rect2 = {centered_x, image1->h, image2->w, image2->h};
    SDL_BlitSurface(image2, NULL, combined_image, &dest_rect2);

    // Sauvegarder l'image combinée
    if (IMG_SavePNG(combined_image, output_path) != 0) {
        printf("Erreur lors de la sauvegarde de l'image combinée: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        SDL_FreeSurface(combined_image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);
    SDL_FreeSurface(combined_image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}


int combine_images_horizontal_bottom_aligned(const char *image1_path, const char *image2_path, const char *output_path) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image1 = IMG_Load(image1_path);
    if (!image1) {
        printf("Erreur lors du chargement de l'image 1: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *image2 = IMG_Load(image2_path);
    if (!image2) {
        printf("Erreur lors du chargement de l'image 2: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Dimensions pour la nouvelle image
    int width = image1->w + image2->w;  // Largeur combinée
    int height = (image1->h > image2->h) ? image1->h : image2->h;  // Hauteur maximale des deux

    SDL_Surface *combined_image = SDL_CreateRGBSurface(0, width, height, 32,
                                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!combined_image) {
        printf("Erreur lors de la création de la surface combinée: %s\n", SDL_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Remplir la surface combinée avec du blanc
    SDL_FillRect(combined_image, NULL, SDL_MapRGB(combined_image->format, 255, 255, 255));

    // Copier image 1 (à gauche, alignée par défaut en haut)
    int offset_y_image1 = height - image1->h; // Alignement bas pour image 1
    SDL_Rect dest_rect1 = {0, offset_y_image1, image1->w, image1->h};
    SDL_BlitSurface(image1, NULL, combined_image, &dest_rect1);

    // Copier image 2 (à droite, alignée vers le bas)
    int offset_y_image2 = height - image2->h; // Alignement bas pour image 2
    SDL_Rect dest_rect2 = {image1->w, offset_y_image2, image2->w, image2->h};
    SDL_BlitSurface(image2, NULL, combined_image, &dest_rect2);

    // Sauvegarder l'image combinée
    if (IMG_SavePNG(combined_image, output_path) != 0) {
        printf("Erreur lors de la sauvegarde de l'image combinée: %s\n", IMG_GetError());
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        SDL_FreeSurface(combined_image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);
    SDL_FreeSurface(combined_image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}


int main()
{
    //if(largeur == 841 || largeur == 947)
    //{
        combine_images_horizontal_bottom_aligned("list_solv.png", "grid_solv.png", "solver_final.png");
    /*}
    else if(largeur == 770 || largeur == 1052)
    {
        combine_images("list_only.png", "grid_only.png", "interface.png");
    }
    else if(largeur == 684)
    {
        combine_images_vertical("list_only.png","grid_only.png","interface.png");
    }
    else
    {
        combine_images_vertical_centered("grid_only.png", "list_only.png", "interface.png");
    }*/

    return 0;
}
