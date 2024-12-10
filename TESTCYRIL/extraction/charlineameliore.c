#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

bool is_black(Uint32 pixel, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    return r < 50 && g < 50 && b < 50; // Seuil pour considérer un pixel comme "noir"
}

bool has_black_neighbor(Uint32 *pixels, int x, int y, int width, int height, SDL_PixelFormat *format, int pitch) {
    // Vérifie si un des voisins est noir (noyau 3x3 autour du pixel actuel)
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;

            // Vérifier si le voisin est dans les limites
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                Uint32 neighbor_pixel = pixels[ny * pitch + nx];
                if (is_black(neighbor_pixel, format)) {
                    return true; // Un voisin noir a été trouvé
                }
            }
        }
    }
    return false;
}

SDL_Surface* thicken_text(SDL_Surface *surface) {
    if (!surface) {
        printf("Surface invalide !\n");
        return NULL;
    }

    // Créer une copie pour stocker les modifications
    SDL_Surface *result_surface = SDL_ConvertSurface(surface, surface->format, 0);
    if (!result_surface) {
        printf("Erreur lors de la création de la surface : %s\n", SDL_GetError());
        return NULL;
    }

    // Accéder aux pixels
    Uint32 *original_pixels = (Uint32 *)surface->pixels;
    Uint32 *result_pixels = (Uint32 *)result_surface->pixels;
    int width = surface->w;
    int height = surface->h;
    int pitch = surface->pitch / 4; // Pas entre deux lignes (en pixels)

    // Parcourir chaque pixel de l'image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 current_pixel = original_pixels[y * pitch + x];

            // Si le pixel actuel est blanc et a un voisin noir, on le colorie en noir
            if (!is_black(current_pixel, surface->format)) {
                if (has_black_neighbor(original_pixels, x, y, width, height, surface->format, pitch)) {
                    result_pixels[y * pitch + x] = SDL_MapRGB(surface->format, 0, 0, 0); // Noir
                } else {
                    result_pixels[y * pitch + x] = current_pixel; // Sinon, garder la couleur d'origine
                }
            } else {
                result_pixels[y * pitch + x] = current_pixel; // Garder les pixels noirs d'origine
            }
        }
    }

    return result_surface;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_image.png> <output_image.png> <iterations>\n", argv[0]);
        return -1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // Charger l'image d'entrée
    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    int iterations = atoi(argv[3]);
    if (iterations <= 0) {
        printf("Le nombre d'itérations doit être supérieur à 0.\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *processed_image = image;

    // Appliquer plusieurs passes de l'algorithme
    for (int i = 0; i < iterations; i++) {
        SDL_Surface *new_image = thicken_text(processed_image);
        if (i > 0) {
            SDL_FreeSurface(processed_image); // Libérer l'image précédente après la première passe
        }
        processed_image = new_image;
        printf("Passage %d terminé.\n", i + 1);
    }

    // Sauvegarder l'image finale
    if (IMG_SavePNG(processed_image, argv[2]) != 0) {
        printf("Erreur lors de la sauvegarde de l'image: %s\n", IMG_GetError());
        SDL_FreeSurface(image);
        SDL_FreeSurface(processed_image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    printf("L'image renforcée a été sauvegardée avec succès sous '%s'\n", argv[2]);

    // Libérer les ressources
    SDL_FreeSurface(image);
    SDL_FreeSurface(processed_image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

