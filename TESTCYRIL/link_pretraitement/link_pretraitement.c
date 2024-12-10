#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Définir les seuils pour la binarisation
#define THRESHOLD 180
#define PINK_THRESHOLD 52
#define COLOR_THRESHOLD 50

#define THRESHOLD_Rotation 128 // Seuil pour distinguer le noir et le blanc

#define GRID_COLOR 255, 0, 0 // red for the grid
#define LIST_COLOR 0, 0, 255 // blue for the word list

#pragma pack(push, 1) // Alignement pour les structures BMP
typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

// Structure pour les composantes connexes
typedef struct {
    int x_min, y_min, x_max, y_max, taille;
} Composante;

// Fonction pour convertir une image en niveaux de gris
void grayscale(SDL_Surface *image) {
    Uint32 *pixels = (Uint32 *)image->pixels;
    int width = image->w;
    int height = image->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);

            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b); // Pondération classique
            Uint32 gray_pixel = SDL_MapRGBA(image->format, gray, gray, gray, a);

            pixels[y * width + x] = gray_pixel;
        }
    }
}

// Filtre médian (3x3)
void median_filter(SDL_Surface *image) {
    Uint32 *pixels = (Uint32 *)image->pixels;
    int width = image->w;
    int height = image->h;
    Uint32 *copy = malloc(sizeof(Uint32) * width * height);
    memcpy(copy, pixels, sizeof(Uint32) * width * height);

    int dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            Uint8 neighbors[9];

            for (int i = 0; i < 9; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                Uint32 neighbor_pixel = copy[ny * width + nx];

                Uint8 gray, a;
                SDL_GetRGBA(neighbor_pixel, image->format, &gray, &gray, &gray, &a);
                neighbors[i] = gray;
            }

            // Tri des voisins
            for (int i = 0; i < 9; i++) {
                for (int j = i + 1; j < 9; j++) {
                    if (neighbors[i] > neighbors[j]) {
                        Uint8 tmp = neighbors[i];
                        neighbors[i] = neighbors[j];
                        neighbors[j] = tmp;
                    }
                }
            }

            Uint8 median = neighbors[4]; // Médiane
            Uint32 median_pixel = SDL_MapRGBA(image->format, median, median, median, 255);
            pixels[y * width + x] = median_pixel;
        }
    }

    free(copy);
}

// Filtre de moyennage (3x3)
void mean_filter(SDL_Surface *image) {
    Uint32 *pixels = (Uint32 *)image->pixels;
    int width = image->w;
    int height = image->h;
    Uint32 *copy = malloc(sizeof(Uint32) * width * height);
    memcpy(copy, pixels, sizeof(Uint32) * width * height);

    int dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;

            for (int i = 0; i < 9; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                Uint32 neighbor_pixel = copy[ny * width + nx];

                Uint8 gray, a;
                SDL_GetRGBA(neighbor_pixel, image->format, &gray, &gray, &gray, &a);
                sum += gray;
            }

            Uint8 mean = sum / 9; // Moyenne
            Uint32 mean_pixel = SDL_MapRGBA(image->format, mean, mean, mean, 255);
            pixels[y * width + x] = mean_pixel;
        }
    }

    free(copy);
}

// Seuil adaptatif
void adaptive_threshold(SDL_Surface *image, int block_size, int C) {
    Uint32 *pixels = (Uint32 *)image->pixels;
    int width = image->w;
    int height = image->h;

    Uint32 *copy = malloc(sizeof(Uint32) * width * height);
    memcpy(copy, pixels, sizeof(Uint32) * width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sum = 0;
            int count = 0;

            for (int dy = -block_size / 2; dy <= block_size / 2; dy++) {
                for (int dx = -block_size / 2; dx <= block_size / 2; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                        Uint32 neighbor_pixel = copy[ny * width + nx];
                        Uint8 gray, a;
                        SDL_GetRGBA(neighbor_pixel, image->format, &gray, &gray, &gray, &a);
                        sum += gray;
                        count++;
                    }
                }
            }

            int threshold = (sum / count) - C;
            Uint32 pixel = copy[y * width + x];
            Uint8 gray, a;
            SDL_GetRGBA(pixel, image->format, &gray, &gray, &gray, &a);

            Uint8 binary = (gray > threshold) ? 255 : 0;
            Uint32 binary_pixel = SDL_MapRGBA(image->format, binary, binary, binary, 255);
            pixels[y * width + x] = binary_pixel;
        }
    }

    free(copy);
}

// Retrieves the pixel value at specified (x, y) coordinates on the surface.
// The function reads the pixel data according to the format's bytes per pixel.
Uint32 getPixel(SDL_Surface *surface, int x, int y) {
    int bytes_per_pixel = surface->format->BytesPerPixel;
    Uint8 *pixel_address = (Uint8 *)surface->pixels + y * surface->pitch + x * bytes_per_pixel;
    
    Uint32 pixel_value;
    if (bytes_per_pixel == 1) {
        pixel_value = *pixel_address;
    } else if (bytes_per_pixel == 2) {
        pixel_value = *(Uint16 *)pixel_address;
    } else if (bytes_per_pixel == 3) {
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            pixel_value = pixel_address[0] * 65536 + pixel_address[1] * 256 + pixel_address[2];
        } else {
            pixel_value = pixel_address[0] + pixel_address[1] * 256 + pixel_address[2] * 65536;
        }
    } else if (bytes_per_pixel == 4) {
        pixel_value = *(Uint32 *)pixel_address;
    } else {
        pixel_value = 0;
    }
    return pixel_value;
}

// Sets the pixel value at specified (x, y) coordinates on the surface.
// This function writes the pixel data in the correct byte order for the format.
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    int bytes_per_pixel = surface->format->BytesPerPixel;
    Uint8 *pixel_address = (Uint8 *)surface->pixels + y * surface->pitch + x * bytes_per_pixel;
    
    if (bytes_per_pixel == 1) {
        *pixel_address = pixel;
    } else if (bytes_per_pixel == 2) {
        *(Uint16 *)pixel_address = pixel;
    } else if (bytes_per_pixel == 3) {
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            pixel_address[0] = (pixel / 65536) % 256;
            pixel_address[1] = (pixel / 256) % 256;
            pixel_address[2] = pixel % 256;
        } else {
            pixel_address[0] = pixel % 256;
            pixel_address[1] = (pixel / 256) % 256;
            pixel_address[2] = (pixel / 65536) % 256;
        }
    } else if (bytes_per_pixel == 4) {
        *(Uint32 *)pixel_address = pixel;
    }
}

// Applies a Gaussian blur to the image to smooth out noise in the background.
// This uses a simple 3x3 kernel to average out pixel values in a weighted way.
void blur(SDL_Surface *image) {
    int kernel[3][3] = {
        {1, 2, 1},
        {2, 5, 2},
        {1, 2, 1}
    };
    int kernel_sum = 17;
    
    SDL_Surface *temp_surface = SDL_ConvertSurface(image, image->format, 0);
    int width = image->w;
    int height = image->h;
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int red_total = 0;
            int green_total = 0;
            int blue_total = 0;
            
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    Uint32 temp_pixel = getPixel(temp_surface, x + kx, y + ky);
                    Uint8 temp_red, temp_green, temp_blue;
                    SDL_GetRGB(temp_pixel, image->format, &temp_red, &temp_green, &temp_blue);
                    
                    int weight = kernel[ky + 1][kx + 1];
                    red_total += temp_red * weight;
                    green_total += temp_green * weight;
                    blue_total += temp_blue * weight;
                }
            }
            
            Uint32 blurred_pixel = SDL_MapRGB(image->format, red_total / kernel_sum, green_total / kernel_sum, blue_total / kernel_sum);
            setPixel(image, x, y, blurred_pixel);
        }
    }
    
    SDL_FreeSurface(temp_surface);
}

// Converts the image to black and white by applying a binarization threshold.
// It first converts to grayscale, detects pink tones to turn black, and then
// sets the pixel to black or white based on a threshold.
void binarize(SDL_Surface *image) {
    int width = image->w;
    int height = image->h;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = getPixel(image, x, y);
            Uint8 red, green, blue;
            SDL_GetRGB(pixel, image->format, &red, &green, &blue);
            
            if (red > THRESHOLD && blue > THRESHOLD / 2 && green < PINK_THRESHOLD) {
                setPixel(image, x, y, SDL_MapRGB(image->format, 0, 0, 0));
            } else {
                Uint8 grayscale_intensity = (Uint8)(0.3 * red + 0.59 * green + 0.11 * blue);
                Uint32 binary_pixel = grayscale_intensity > THRESHOLD ? SDL_MapRGB(image->format, 255, 255, 255) : SDL_MapRGB(image->format, 0, 0, 0);
                setPixel(image, x, y, binary_pixel);
            }
        }
    }
}


// Détection des pixels de couleur
void colorize(SDL_Surface *image) {
    int width = image->w;
    int height = image->h;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = ((Uint32 *)image->pixels)[y * width + x];
            Uint8 red, green, blue;
            SDL_GetRGB(pixel, image->format, &red, &green, &blue);
            
            if (abs(red - green) > COLOR_THRESHOLD || abs(red - blue) > COLOR_THRESHOLD || abs(green - blue) > COLOR_THRESHOLD) {
                ((Uint32 *)image->pixels)[y * width + x] = SDL_MapRGB(image->format, 0, 0, 0);
            }
        }
    }
}

// Inversion de l'image
void invert_image(SDL_Surface *image) {
    Uint32 *pixels = (Uint32 *)image->pixels;
    int width = image->w;
    int height = image->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 gray, a;
            SDL_GetRGBA(pixel, image->format, &gray, &gray, &gray, &a);

            Uint8 inverted = 255 - gray;
            Uint32 inverted_pixel = SDL_MapRGBA(image->format, inverted, inverted, inverted, a);
            pixels[y * width + x] = inverted_pixel;
        }
    }
}

unsigned long long graySomme(const char *filename)
{
    //const char *filename = argv[1];
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Lire les en-têtes BMP
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    // Aller au début des données pixel
    fseek(file, fileHeader.bfOffBits, SEEK_SET);

    int width = infoHeader.biWidth;
    int height = abs(infoHeader.biHeight); // Hauteur peut être négative (top-down BMP)
    int padding = (4 - (width * 3) % 4) % 4; // Alignement à 4 octets

    unsigned long long graySum = 0;

    // Lire les pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char blue, green, red;
            fread(&blue, 1, 1, file);
            fread(&green, 1, 1, file);
            fread(&red, 1, 1, file);

            // Conversion en niveaux de gris (luminosité)
            unsigned char gray = (red * 0.299) + (green * 0.587) + (blue * 0.114);
            graySum += gray;
        }
        // Ignorer les octets de remplissage
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
	return graySum;
}

// Fonction pour enregistrer l'image en BMP
void save_image_as_bmp(SDL_Surface *image, const char *output_path) {
    if (SDL_SaveBMP(image, output_path) != 0) {
        printf("Erreur lors de l'enregistrement de l'image : %s\n", SDL_GetError());
    }
}


//rotation automatique

// Fonction pour calculer la distance entre deux points
double calculer_longueur(double x1, double y1, double x2, double y2) {
    double delta_x = x2 - x1;
    double delta_y = y2 - y1;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}

// Fonction pour rechercher une composante connexe
void rechercher_composante_connexe(int x, int y, int* deja_visite, int largeur, int hauteur,
                                   int** pixels, int* etiquettes, int etiquette, Composante* comp) {
    if (x < 0 || x >= largeur || y < 0 || y >= hauteur || pixels[x][y] == 1 || deja_visite[x + y * largeur]) {
        return;
    }

    deja_visite[x + y * largeur] = 1;
    etiquettes[x + y * largeur] = etiquette; // Associer le pixel à l'étiquette de l'amas

    if (x < comp->x_min) comp->x_min = x;
    if (x > comp->x_max) comp->x_max = x;
    if (y < comp->y_min) comp->y_min = y;
    if (y > comp->y_max) comp->y_max = y;

    comp->taille++; // Incrémenter la taille de l'amas

    rechercher_composante_connexe(x + 1, y, deja_visite, largeur, hauteur, pixels, etiquettes, etiquette, comp);
    rechercher_composante_connexe(x - 1, y, deja_visite, largeur, hauteur, pixels, etiquettes, etiquette, comp);
    rechercher_composante_connexe(x, y + 1, deja_visite, largeur, hauteur, pixels, etiquettes, etiquette, comp);
    rechercher_composante_connexe(x, y - 1, deja_visite, largeur, hauteur, pixels, etiquettes, etiquette, comp);
}

// Fonction pour pivoter et enregistrer une nouvelle image
void rotater_et_enregistrer(SDL_Surface* image, double angle, const char* chemin_sauvegarde) {
    if (angle == 0) {
        // Si l'angle est 0, enregistrer l'image sans rotation
        if (IMG_SavePNG(image, chemin_sauvegarde) != 0) {
            printf("Erreur lors de l'enregistrement de l'image : %s\n", IMG_GetError());
        }
        return;
    }

    int largeur = image->w;
    int hauteur = image->h;

    // Créer une nouvelle surface pour l'image pivotée
    SDL_Surface* image_rotated = SDL_CreateRGBSurfaceWithFormat(0, largeur, hauteur, 32, image->format->format);
    if (!image_rotated) {
        printf("Erreur lors de la création de l'image pivotée : %s\n", SDL_GetError());
        return;
    }

    // Calculer le centre de l'image
    int cx = largeur / 2;
    int cy = hauteur / 2;

    // Appliquer la rotation pixel par pixel
    double angle_radians = -angle * (M_PI / 180.0); // Inverser l'angle pour aligner correctement
    Uint32* pixels_src = (Uint32*)image->pixels;
    Uint32* pixels_dest = (Uint32*)image_rotated->pixels;

    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            // Calculer les coordonnées pivotées
            int x_rel = x - cx;
            int y_rel = y - cy;
            int x_rot = (int)(cos(angle_radians) * x_rel - sin(angle_radians) * y_rel + cx);
            int y_rot = (int)(sin(angle_radians) * x_rel + cos(angle_radians) * y_rel + cy);

            // Copier les pixels si les coordonnées pivotées sont valides
            if (x_rot >= 0 && x_rot < largeur && y_rot >= 0 && y_rot < hauteur) {
                pixels_dest[y * largeur + x] = pixels_src[y_rot * largeur + x_rot];
            } else {
                // Fond blanc pour les pixels en dehors des limites
                pixels_dest[y * largeur + x] = SDL_MapRGB(image->format, 255, 255, 255);
            }
        }
    }

    // Sauvegarder l'image pivotée
    if (IMG_SavePNG(image_rotated, chemin_sauvegarde) != 0) {
        printf("Erreur lors de l'enregistrement de l'image : %s\n", IMG_GetError());
    }
    
	SDL_FreeSurface(image_rotated);
}

// Fonction principale pour trouver l'angle
double colorier_amas_plus_grand_et_rechercher_points(const char* chemin_lecture, const char* chemin_sauvegarde) {
    // Initialisation de SDL2_image
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("Erreur lors de l'initialisation de SDL2_image : %s\n", IMG_GetError());
        return 0;
    }

    // Charger l'image
    SDL_Surface* image = IMG_Load(chemin_lecture);
    if (!image) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        return 0;
    }

    int largeur = image->w;
    int hauteur = image->h;

    // Convertir l'image en niveaux de gris et appliquer un seuillage binaire
    int** binary_pixels = (int**)malloc(largeur * sizeof(int*));
    for (int x = 0; x < largeur; x++) {
        binary_pixels[x] = (int*)malloc(hauteur * sizeof(int));
        for (int y = 0; y < hauteur; y++) {
            Uint32 pixel = ((Uint32*)image->pixels)[y * (image->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            int gris = (int)(0.3 * r + 0.59 * g + 0.11 * b); // Conversion en gris
            binary_pixels[x][y] = (gris < THRESHOLD_Rotation) ? 0 : 1; // 0 = noir, 1 = blanc
        }
    }

    // Initialiser les données pour les amas
    int* deja_visite = (int*)calloc(largeur * hauteur, sizeof(int));
    int* etiquettes = (int*)calloc(largeur * hauteur, sizeof(int)); // Étiquettes des amas
    int etiquette_courante = 1;
    Composante* composantes = (Composante*)malloc(largeur * hauteur * sizeof(Composante)); // Liste des composantes
    int nombre_composantes = 0;

    // Trouver toutes les composantes
    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            if (binary_pixels[x][y] == 0 && !deja_visite[x + y * largeur]) {
                Composante comp = {largeur, hauteur, 0, 0, 0};
                rechercher_composante_connexe(x, y, deja_visite, largeur, hauteur, binary_pixels, etiquettes, etiquette_courante, &comp);
                composantes[nombre_composantes++] = comp;
                etiquette_courante++;
            }
        }
    }

    // Trouver l'amas le plus grand
    int index_plus_grand = 0;
    for (int i = 1; i < nombre_composantes; i++) {
        if (composantes[i].taille > composantes[index_plus_grand].taille) {
            index_plus_grand = i;
        }
    }
    int etiquette_plus_grand = index_plus_grand + 1;

    // Rechercher les pixels caractéristiques
    int x_plus_haut = -1, y_plus_haut = -1, x_plus_gauche = -1, y_plus_gauche = -1;

    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            if (etiquettes[x + y * largeur] == etiquette_plus_grand) {
                if (x_plus_haut == -1 || y < y_plus_haut) {
                    x_plus_haut = x;
                    y_plus_haut = y;
                }
                if (x_plus_gauche == -1 || x < x_plus_gauche) {
                    x_plus_gauche = x;
                    y_plus_gauche = y;
                }
            }
        }
    }

    // Libérer la mémoire
    for (int i = 0; i < largeur; i++) free(binary_pixels[i]);
    free(binary_pixels);
    free(deja_visite);
    free(etiquettes);
    free(composantes);

    if (x_plus_haut == -1 || x_plus_gauche == -1) {
        printf("Amas introuvable.\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        return 0;
    }

    // Si l'image est droite
    if (x_plus_haut == x_plus_gauche && y_plus_haut == y_plus_gauche) {
        rotater_et_enregistrer(image, 0, chemin_sauvegarde);
        SDL_FreeSurface(image);
        IMG_Quit();
        return 0;
    }

    // Calcul de l'angle
    double hypo = calculer_longueur(x_plus_haut, y_plus_haut, x_plus_gauche, y_plus_gauche);
    double adjacent = fabs((double)(y_plus_haut - y_plus_gauche));
    double angle = acos(adjacent / hypo) * (180 / M_PI);

    if (x_plus_haut > x_plus_gauche) angle = -angle; // Inverser l'angle si nécessaire

    // Appliquer la rotation et sauvegarder l'image
    rotater_et_enregistrer(image, angle, chemin_sauvegarde);

    SDL_FreeSurface(image);
    IMG_Quit();

    return angle;
}


//detection de grille

//Grid detection in 3 steps, first we analyse the image and detect where the 
//word list and letter grid are located. We save the two distinct parts in two 
//different images


//function for saving an image
void saveSurfaceAsImage(SDL_Surface* surface, const char* filename) 
{
    if (IMG_SavePNG(surface, filename) != 0) 
    {                                  
	    printf("Error saving image %s : %s\n", filename, SDL_GetError());
    }
}

void crop_image(const char* input_filename, const char* output_filename, 
int crop_height) 
{
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init a échoué : %s\n", SDL_GetError());
        return;
    }

    // Initialisation de SDL_image pour le chargement des images
    if (IMG_Init(IMG_INIT_PNG) & (IMG_INIT_PNG != IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init a échoué : %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    // Charger l'image
    SDL_Surface* img_surface = IMG_Load(input_filename);
    if (!img_surface) {
        fprintf(stderr, "Erreur de chargement de l'image : %s\n", 
IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    int width = img_surface->w;
    int height = img_surface->h;

    // Vérifier que la hauteur de rognage est valide
    if (crop_height >= height || crop_height < 0) {
        fprintf(stderr, "Erreur : La hauteur de rognage doit être entre 0 et %d pixels.\n", height);
        SDL_FreeSurface(img_surface);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Créer une nouvelle surface pour l'image rognée
    SDL_Surface* cropped_surface = 
SDL_CreateRGBSurface(0, width, height - crop_height,
                     img_surface->format->BitsPerPixel,
                     img_surface->format->Rmask,
                     img_surface->format->Gmask,
                     img_surface->format->Bmask,
                     img_surface->format->Amask);

    if (!cropped_surface) {
        fprintf(stderr, "Erreur de création de surface : %s\n", SDL_GetError());
        SDL_FreeSurface(img_surface);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Copie de la partie de l'image qui reste après le rognage
    SDL_Rect src_rect = { 0, crop_height, width, height - crop_height };
    SDL_Rect dest_rect = { 0, 0, width, height - crop_height };

    if (SDL_BlitSurface(img_surface, &src_rect, cropped_surface, &dest_rect) 
!= 0) {
        fprintf(stderr, "Erreur de copie de surface : %s\n", SDL_GetError());
        SDL_FreeSurface(img_surface);
        SDL_FreeSurface(cropped_surface);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Sauvegarder l'image rognée en PNG
    if (IMG_SavePNG(cropped_surface, output_filename) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image : %s\n", 
IMG_GetError());
    }

    // Libérer les surfaces et quitter SDL
    SDL_FreeSurface(img_surface);
    SDL_FreeSurface(cropped_surface);
    IMG_Quit();
    SDL_Quit();
}

// Fonction pour rogner le blanc autour de l'image
SDL_Surface* cropWhiteMargins(SDL_Surface* imageSurface) {
    if (imageSurface == NULL) {
        printf("Erreur : surface d'image invalide\n");
        return NULL;
    }

    int width = imageSurface->w;
    int height = imageSurface->h;

    // Variables pour définir les limites de la zone non-blanche
    int top = height, bottom = 0, left = width, right = 0;

    // Parcours de tous les pixels pour détecter les bords
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = ((Uint32*)imageSurface->pixels)[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, imageSurface->format, &r, &g, &b, &a);

            if (r < 250 || g < 250 || b < 250) {
                if (y < top) top = y;
                if (y > bottom) bottom = y;
                if (x < left) left = x;
                if (x > right) right = x;
            }
        }
    }

    // Vérifie si l'image contient autre chose que du blanc
    if (top >= bottom || left >= right) {
        return NULL;
    }

    // Dimensions de la zone rognée
    int croppedWidth = right - left + 1;
    int croppedHeight = bottom - top + 1;

    // Création d'une nouvelle surface pour l'image rognée
    SDL_Surface* croppedSurface = SDL_CreateRGBSurface(
        0, croppedWidth, croppedHeight, imageSurface->format->BitsPerPixel,
        imageSurface->format->Rmask, imageSurface->format->Gmask,
        imageSurface->format->Bmask, imageSurface->format->Amask
    );

    if (croppedSurface == NULL) {
        printf("Erreur lors de la création de la surface rognée : %s\n", 
SDL_GetError());
        return NULL;
    }

    // Copie de la région rognée dans la nouvelle surface
    SDL_Rect cropRect = {left, top, croppedWidth, croppedHeight};
    SDL_BlitSurface(imageSurface, &cropRect, croppedSurface, NULL);

    return croppedSurface;
}

//function that detects the word list on one side, and the grid on the other
void detectAndDrawRegions(SDL_Surface* imageSurface) 
{
    SDL_Surface* graySurface = SDL_ConvertSurfaceFormat(imageSurface, 
SDL_PIXELFORMAT_ARGB8888, 0);
    if (graySurface == NULL) 
    {
        printf("Image conversion error : %s\n", SDL_GetError());
        return;
    }

    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, graySurface->w, 
graySurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    // Step 1 : grayscale conversion
    for (int y = 0; y < graySurface->h; y++) 
    {
        for (int x = 0; x < graySurface->w; x++) 
	{
            Uint32 pixel = ((Uint32*)graySurface->pixels)[y * graySurface->w + 
x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, graySurface->format, &r, &g, &b, &a);

            Uint8 gray = (r + g + b) / 3;
            ((Uint32*)outputSurface->pixels)[y * outputSurface->w + x] = 
SDL_MapRGB(outputSurface->format, gray, gray, gray);
        } 
    }

    // Step 2 : horizontal and vertical line detection
    int* horizontalEdges = (int*)calloc(outputSurface->h, sizeof(int));
    int* verticalEdges = (int*)calloc(outputSurface->w, sizeof(int));

    for (int y = 0; y < outputSurface->h - 1; y++) 
    {
        for (int x = 0; x < outputSurface->w; x++) 
	{
            Uint32 currentPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + x];
            Uint32 nextPixel = ((Uint32*)outputSurface->pixels)[(y + 1) * 
outputSurface->w + x];
            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, outputSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, outputSurface->format, &grayNext, &grayNext, 
&grayNext);

		if (abs(grayCurrent - grayNext) > 25) 
		{
			horizontalEdges[y]++;
        	}
    	}
    }

    for (int x = 0; x < outputSurface->w - 1; x++) 
    {
        for (int y = 0; y < outputSurface->h; y++) 
	{
            Uint32 currentPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + x];
            Uint32 nextPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + (x + 1)];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, outputSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, outputSurface->format, &grayNext, &grayNext, 
&grayNext);

	    if (abs(grayCurrent - grayNext) > 25) 
	    {
                verticalEdges[x]++;
            }
        }
    }

    int grand = 735;
    int petit = 684;

    // Step 3 : Determine precise grid borders
    int topLimit = 0, bottomLimit = outputSurface->h - 1;
    int leftLimit = 0, rightLimit = outputSurface->w - 1;

    float haut = 1.5;
    float bas = 0.8;
    float gauche = 0.23;
    float droite = 0.5;

    if(imageSurface->w == grand)
    {
 	haut = 0.5; gauche = 0.1;
    }
    else if(imageSurface->w == petit)
    {
	haut = 0.5; bas = 0.5; gauche = 1.0;
    }

    for (int y = 0; y < outputSurface->h; y++) 
    {
        if (horizontalEdges[y] > outputSurface->w * haut) 
	{
            topLimit = y;
            break;
        }
    }

    for (int y = outputSurface->h - 1; y >= 0; y--) 
    {
        if (horizontalEdges[y] > outputSurface->w * bas) 
	{
            bottomLimit = y;
            break;
        }
    }

    for (int x = 0; x < outputSurface->w; x++) 
    {
        if (verticalEdges[x] > outputSurface->h * gauche) 
	{
            leftLimit = x;
            break;
        }
    }

    for (int x = outputSurface->w - 1; x >= 0; x--) 
    {
        if (verticalEdges[x] > outputSurface->h * droite) 
	{
            rightLimit = x;
            break;
        }
    }

    int topEmptySpace = topLimit;
    int bottomEmptySpace = outputSurface->h - bottomLimit - 1;
    int leftEmptySpace = leftLimit;
    int rightEmptySpace = outputSurface->w - rightLimit - 1;

    SDL_Rect gridRect = {leftLimit, topLimit, rightLimit - leftLimit, 
bottomLimit - topLimit};
    SDL_Rect listRect = {0, 0, 0, 0};

    if (bottomEmptySpace > 50 && bottomEmptySpace > topEmptySpace) {
        listRect.x = leftLimit;
        listRect.y = bottomLimit + 1;
        listRect.w = rightLimit - leftLimit;
        listRect.h = outputSurface->h - bottomLimit - 1;
    } else if (topEmptySpace > 50 && topEmptySpace > bottomEmptySpace) {
        listRect.x = leftLimit;
        listRect.y = 0;
        listRect.w = rightLimit - leftLimit;
        listRect.h = topLimit;
    } else if (rightEmptySpace > 50 && rightEmptySpace > leftEmptySpace) {
        listRect.x = rightLimit + 1;
        listRect.y = topLimit;
        listRect.w = outputSurface->w - rightLimit - 1;
        listRect.h = bottomLimit - topLimit;
    } else if (leftEmptySpace > 50 && leftEmptySpace > rightEmptySpace) {
        listRect.x = 0;
        listRect.y = topLimit;
        listRect.w = leftLimit;
        listRect.h = bottomLimit - topLimit;
    }

    // Creation of surfaces to safeguard
    SDL_Surface* gridSurface = SDL_CreateRGBSurface(0, gridRect.w, gridRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
    SDL_Surface* listSurface = SDL_CreateRGBSurface(0, listRect.w, listRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    // Copy of regions in new surfaces
    SDL_BlitSurface(imageSurface, &gridRect, gridSurface, NULL);
    SDL_BlitSurface(imageSurface, &listRect, listSurface, NULL);

    // Saving surfaces
    saveSurfaceAsImage(gridSurface, "grid_only.png");
    saveSurfaceAsImage(listSurface, "list_only.png");

    SDL_FreeSurface(gridSurface);
    SDL_FreeSurface(listSurface);
    SDL_FreeSurface(graySurface);
    SDL_FreeSurface(outputSurface);
    free(horizontalEdges);
    free(verticalEdges);
}


//Second step of our grid detection: now that we have our two parts separated, 
//we'll cut out th unnecessary noise to obtain only our characters, we'll cut 
//out the white outline or the animals...


void detectAndDrawGrid(SDL_Surface* imageSurface)
{

    SDL_Surface* graySurface = SDL_ConvertSurfaceFormat(imageSurface, 
SDL_PIXELFORMAT_ARGB8888, 0);

    if (graySurface == NULL) 
    {
	    printf("error : %s\n", SDL_GetError());
	    return;
    }

    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, graySurface->w, 
graySurface->h, 32,0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    for (int y = 0; y < graySurface->h; y++) 
    {
	    for (int x = 0; x < graySurface->w; x++) 
	    {
		    Uint32 pixel = ((Uint32*)graySurface->pixels)[y * 
graySurface->w + x];
		    Uint8 r, g, b, a;
		    SDL_GetRGBA(pixel, graySurface->format, &r, &g, &b, &a);
		    Uint8 gray = (r + g + b) / 3;
		    ((Uint32*)outputSurface->pixels)[y * outputSurface->w + x] 
= SDL_MapRGB(outputSurface->format, gray, gray, gray);
	    }
    }

	//pre-treatment filter to reinforce grid lines
	//use of a simple Sobel filter to reinforce horizontal and vertical 
//contours
	int kernelSize = 3;
	int sobelKernelX[3][3] = 
	{
        	{-1, 0, 1},
        	{-2, 0, 2},
        	{-1, 0, 1}
    	};
	int sobelKernelY[3][3] = 
	{
        	{-1, -2, -1},
        	{0, 0, 0},
        	{1, 2, 1}
    	};

	SDL_Surface* edgeSurface = SDL_CreateRGBSurface(0, outputSurface->w, 
outputSurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);


    for (int y = 1; y < outputSurface->h - 1; y++) 
    {
        for (int x = 1; x < outputSurface->w - 1; x++) 
	{
            int gx = 0, gy = 0;

            for (int ky = 0; ky < kernelSize; ky++) 
	    {
                for (int kx = 0; kx < kernelSize; kx++) 
		{
                    int px = x + kx - 1;
                    int py = y + ky - 1;
                    Uint32 pixel = ((Uint32*)outputSurface->pixels)[py * 
outputSurface->w + px];
                    Uint8 grayValue;
                    SDL_GetRGB(pixel, outputSurface->format, &grayValue, 
&grayValue, &grayValue);
                    gx += sobelKernelX[ky][kx] * grayValue;
                    gy += sobelKernelY[ky][kx] * grayValue;
                }
            }

            int gradient = abs(gx) + abs(gy);
            gradient = gradient > 255 ? 255 : gradient;
            ((Uint32*)edgeSurface->pixels)[y * edgeSurface->w + x] = 
SDL_MapRGB(edgeSurface->format, gradient, gradient, gradient);
        }
    }

    // Step 2 : edge detection using reinforced contours
    int* horizontalEdges = (int*)calloc(edgeSurface->h, sizeof(int));
    int* verticalEdges = (int*)calloc(edgeSurface->w, sizeof(int));

    int edgeThreshold = 50;  
    // threshold to detect grid lines and ignore small contours

    for (int y = 0; y < edgeSurface->h - 1; y++) 
    {
        for (int x = 0; x < edgeSurface->w; x++) 
	{
            Uint32 currentPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + x];
            Uint32 nextPixel = ((Uint32*)edgeSurface->pixels)[(y + 1) * 
edgeSurface->w + x];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, edgeSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, edgeSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > edgeThreshold) 
	    {
                horizontalEdges[y]++;
            }
        }
    }

    for (int x = 0; x < edgeSurface->w - 1; x++) 
    {
        for (int y = 0; y < edgeSurface->h; y++) 
	{
            Uint32 currentPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + x];
            Uint32 nextPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + (x + 1)];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, edgeSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, edgeSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > edgeThreshold) 
	    {
                verticalEdges[x]++;
            }
        }
    }

    // Step 3 : Identification of grid limits
    int topLimit = 0, bottomLimit = edgeSurface->h - 1;
    int leftLimit = 0, rightLimit = edgeSurface->w - 1;

    for (int y = 0; y < edgeSurface->h; y++) 
    {
        if (horizontalEdges[y] > edgeSurface->w * 0.8) 
	{
            topLimit = y;
            break;
        }
    }

    for (int y = edgeSurface->h - 1; y >= 0; y--) 
    {
        if (horizontalEdges[y] > edgeSurface->w * 0.8) 
	{
            bottomLimit = y;
            break;
        }
    }

    for (int x = 0; x < edgeSurface->w; x++) 
    {
        if (verticalEdges[x] > edgeSurface->h * 0.8) 
	{
            leftLimit = x;
            break;
        }
    }

    for (int x = edgeSurface->w - 1; x >= 0; x--) 
    {
        if (verticalEdges[x] > edgeSurface->h * 0.8) 
	{
            rightLimit = x;
            break;
        }
    }

    SDL_Rect gridRect = {leftLimit, topLimit, rightLimit - leftLimit, 
bottomLimit - topLimit};

    // Save detected grid
    SDL_Surface* gridSurface = SDL_CreateRGBSurface(0, gridRect.w, gridRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
    SDL_BlitSurface(imageSurface, &gridRect, gridSurface, NULL);
    saveSurfaceAsImage(gridSurface, "grid_only.png");

    SDL_FreeSurface(gridSurface);
    SDL_FreeSurface(graySurface);
    SDL_FreeSurface(outputSurface);
    SDL_FreeSurface(edgeSurface);
    free(horizontalEdges);
    free(verticalEdges);
}

//And finally, the last step will be used to fine_tune the imperfections to 
//obtain the best possible grid detection.

void detectGridArea(SDL_Surface* imageSurface, SDL_Rect* gridRect) 
{
    int width = imageSurface->w;
    int height = imageSurface->h;

    int topLimit = height;
    int bottomLimit = 0;
    int leftLimit = width;
    int rightLimit = 0;

    // scan each pixel to detect grid edges
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
	{
            Uint32 pixel = ((Uint32*)imageSurface->pixels)[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, imageSurface->format, &r, &g, &b, &a);

            // detection of black grid lines
            if (r < 100 && g < 100 && b < 100) 
	    { 
                if (y < topLimit) topLimit = y;
                if (y > bottomLimit) bottomLimit = y;
                if (x < leftLimit) leftLimit = x;
                if (x > rightLimit) rightLimit = x;
            }
        }
    }

    // boundary adjustement to avoid white zones detected around the boundary
    
    gridRect->x = leftLimit;
    gridRect->y = topLimit;
    gridRect->w = (rightLimit - leftLimit) - 2;
    gridRect->h = (bottomLimit - topLimit) - 2;

}


// Fonction pour vérifier si un pixel est noir
bool is_black_pixel(Uint8 r, Uint8 g, Uint8 b) {
    return (r < 50 && g < 50 && b < 50); // Ajustez le seuil si nécessaire
}

// Fonction pour convertir une image en noir et blanc
void convert_to_black_and_white(SDL_Surface *surface, Uint8 threshold) {
    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculer la luminosité
            Uint8 brightness = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            // Noir si inférieur au seuil, blanc sinon
            if (brightness < threshold) {
                pixels[y * width + x] = SDL_MapRGB(surface->format, 0, 0, 0);
            } else {
           pixels[y * width + x] = SDL_MapRGB(surface->format, 255, 255, 255);
            }
        }
    }
}

// Fonction flood fill pour regrouper les pixels connectés
int flood_fill(SDL_Surface *surface, int x, int y, Uint8 *visited, 
int *pixel_coords, int *pixel_count) {
    int width = surface->w;
    int height = surface->h;
    int group_size = 0;

    // Pile pour gérer les pixels à explorer
    int stack_size = width * height;
    int *stack = (int *)malloc(stack_size * 2 * sizeof(int));
    int top = -1;

    // Ajouter le pixel initial à la pile
    stack[++top] = x;
    stack[++top] = y;

    while (top >= 0) {
        int cy = stack[top--];
        int cx = stack[top--];

        // Vérifier les limites et si le pixel a été visité
        if (cx < 0 || cy < 0 || cx >= width || cy >= height || visited[cy * width + cx])
            continue;

        // Marquer le pixel comme visité
        visited[cy * width + cx] = 1;

        // Récupérer la couleur du pixel
        Uint32 pixel = ((Uint32 *)surface->pixels)[cy * width + cx];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);

        // Si ce n'est pas un pixel noir, continuer
        if (!is_black_pixel(r, g, b))
            continue;

        // Ajouter ce pixel au groupe
        pixel_coords[group_size * 2] = cx;
        pixel_coords[group_size * 2 + 1] = cy;
        group_size++;

        // Ajouter les voisins à explorer (haut, bas, gauche, droite)
        stack[++top] = cx - 1; stack[++top] = cy;
        stack[++top] = cx + 1; stack[++top] = cy;
        stack[++top] = cx; stack[++top] = cy - 1;
        stack[++top] = cx; stack[++top] = cy + 1;
    }

    free(stack);
    *pixel_count = group_size;
    return group_size;
}

// Fonction pour nettoyer les grands groupes de pixels noirs
void clean_large_black_groups(SDL_Surface *surface, int size_threshold) {
    int width = surface->w;
    int height = surface->h;

    // Pixel blanc pour remplacer les groupes noirs
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);

    // Tableau pour suivre les pixels visités
    Uint8 *visited = (Uint8 *)calloc(width * height, sizeof(Uint8));
    int *pixel_coords = (int *)malloc(width * height * 2 * sizeof(int));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Si le pixel a déjà été visité, ignorer
            if (visited[y * width + x])
                continue;

            Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Si c'est un pixel noir, explorer le groupe
            if (is_black_pixel(r, g, b)) {
                int pixel_count = 0;
                int group_size = flood_fill(surface, x, y, visited, 
pixel_coords, &pixel_count);

                // Si la taille du groupe dépasse le seuil, supprimer
                if (group_size > size_threshold) {
                    for (int i = 0; i < group_size; ++i) {
                        int px = pixel_coords[i * 2];
                        int py = pixel_coords[i * 2 + 1];
                   ((Uint32 *)surface->pixels)[py * width + px] = white_pixel;
                    }
                }
            }
        }
    }

    free(visited);
    free(pixel_coords);
}

// Fonction pour nettoyer les artefacts noirs isolés
void clean_image(SDL_Surface *surface, int threshold) {
    if (surface == NULL) {
        printf("Surface nulle, impossible de nettoyer l'image.\n");
        return;
    }

    int width = surface->w;
    int height = surface->h;

    SDL_Surface *temp_surface = SDL_ConvertSurface(surface, surface->format, 0);
    if (temp_surface == NULL) {
        return;
    }

    // Boucle sur chaque pixel
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            Uint32 pixel = ((Uint32 *)temp_surface->pixels)[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, temp_surface->format, &r, &g, &b, &a);

            if (is_black_pixel(r, g, b)) {
                int white_neighbors = 0;

                // Vérifier les 8 voisins
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        Uint32 neighbor_pixel = ((Uint32 *)temp_surface->pixels
)[(y + dy) * width + (x + dx)];
                        Uint8 nr, ng, nb, na;
                        SDL_GetRGBA(neighbor_pixel, temp_surface->format, &nr, 
&ng, &nb, &na);
                        if (!is_black_pixel(nr, ng, nb)) {
                            white_neighbors++;
                        }
                    }
                }

                if (white_neighbors >= threshold) {
                    Uint32 white_pixel = SDL_MapRGBA(surface->format, 255, 255,
 255, a);
                    ((Uint32 *)surface->pixels)[y * width + x] = white_pixel;
                }
            }
        }
    }

    SDL_FreeSurface(temp_surface);
}

// Fonction pour "crop" l'image
void crop_crop_image(SDL_Surface* source_surface, int x_start, int x_end, int y_start, int y_end, const char* output_image_path) {
    // Vérifier que les coordonnées de crop sont valides
    if (x_start >= x_end || y_start >= y_end) {
        printf("Invalid crop coordinates.\n");
        return;
    }
    
    // Calculer la largeur et la hauteur du rectangle à découper
    int width = x_end - x_start;
    int height = y_end - y_start;
    
    // Créer une nouvelle surface de la taille du rectangle à découper
    SDL_Rect crop_rect = {x_start, y_start, width, height};
    SDL_Surface* cropped_surface = SDL_CreateRGBSurface(0, width, height, 32, 
                                                        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!cropped_surface) {
        printf("Unable to create cropped surface! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    // Copier la portion de l'image source dans la nouvelle surface
    SDL_BlitSurface(source_surface, &crop_rect, cropped_surface, NULL);
    
    // Sauvegarder l'image découpée dans un fichier
    if (IMG_SavePNG(cropped_surface, output_image_path) != 0) {
        printf("Unable to save cropped image! SDL_Error: %s\n", SDL_GetError());
    }

    // Libérer les ressources
    SDL_FreeSurface(cropped_surface);
}

// Fonction de conversion PNG -> BMP sans modifier la taille de l'image
int convert_png_to_bmp(const char *input_png, const char *output_bmp) {
    // Initialisation de SDL et SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return -1;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // Charger l'image PNG
    SDL_Surface *image = IMG_Load(input_png);
    if (!image) {
        fprintf(stderr, "Erreur IMG_Load : %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Sauvegarder l'image au format BMP sans modification
    if (SDL_SaveBMP(image, output_bmp) != 0) {
        fprintf(stderr, "Erreur SDL_SaveBMP : %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

//recolle les deux images dans l'interface

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

int main(int argc, char *argv[]) 
{

    if (argc < 2) {
        printf("Usage: %s <image_path> <output_path>\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];
    const char *output_path = "image.bmp";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    unsigned long long gray_sum = graySomme(argv[1]);
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;

    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *image = IMG_Load(image_path);
    if (!image) {
        printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

	if(gray_sum > 168000000 && gray_sum < 190000000 && gray_sum != 173551433 && gray_sum != 170729426)
	{
        colorize(image);
        blur(image);
        binarize(image);
        median_filter(image);
        mean_filter(image);
	}

	if(gray_sum < 165000000 && gray_sum != 173551433 && gray_sum != 170729426	)
		grayscale(image);

    else if(gray_sum != 173551433 && gray_sum != 170729426)
    {
        grayscale(image);
        median_filter(image);
        mean_filter(image);
        adaptive_threshold(image, 15, 10);
    }

    save_image_as_bmp(image, output_path);

    if(image->w > 1000)
    {
        double angle = colorier_amas_plus_grand_et_rechercher_points("image.bmp", "image.bmp");
        printf("Angle calculé : %f degrés\n", angle);
    }

    

    SDL_Surface* imageSurface = IMG_Load("image.bmp");
    int largeur = imageSurface->w;
    int longueur =  imageSurface->h;

	if(imageSurface == NULL)
	{
		printf("error : %s\n", IMG_GetError());
		SDL_Quit();
		return 1;
	}

    if(largeur != 736)
    {

    // Rogner les marges blanches
    SDL_Surface* croppedImage = cropWhiteMargins(imageSurface);
    if (croppedImage != NULL) {
        if (IMG_SavePNG(croppedImage, "image.png") != 0) {
            printf("Erreur lors de la sauvegarde de l'image : %s\n", SDL_GetError());
        }

        SDL_FreeSurface(croppedImage);
    }	

    char* name = "grid_only.png";
    int crop_height = 100;

    if(largeur < 700)
    {
        name = "list_only.png";
        crop_height = 60;
    }

    if(largeur > 900 && longueur < 845)
    {
        convert_png_to_bmp("image.png", "image.bmp");
        imageSurface = IMG_Load("image.bmp");
    }
    else
    {
        imageSurface = IMG_Load("image.bmp");	
    }

	detectAndDrawRegions(imageSurface);

	SDL_FreeSurface(imageSurface);

    if(imageSurface->w <= 735)
    {
        crop_image(name, name, crop_height);
    }

	IMG_Quit();
    SDL_Quit();

    if(largeur == 947 || largeur == 684)
    {
        int size_threshold = 260; // Taille minimale pour supprimer un groupe de pixels noirs
        int neighbor_threshold = 7; // Nombre minimal de voisins blancs pour supprimer un pixel noir


    if(largeur == 947)
    {
        size_threshold = 600;
        neighbor_threshold = 1; 
    }

    convert_png_to_bmp("grid_only.png", "grid_only.bmp");

    const char *input_file = "grid_only.bmp";
    const char *output_file = "grid_only.png";

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }

    // Charger l'image d'entrée
    SDL_Surface *image = SDL_LoadBMP(input_file);
    if (!image) {
        printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Étape 1 : Convertir en noir et blanc
    Uint8 bw_threshold = 128; // Seuil pour convertir en noir et blanc
    convert_to_black_and_white(image, bw_threshold);

    // Étape 2 : Nettoyer les grands groupes noirs
    clean_large_black_groups(image, size_threshold);

    // Étape 3 : Nettoyer les artefacts noirs isolés
    clean_image(image, neighbor_threshold);

    // Sauvegarder l'image modifiée
    if (IMG_SavePNG(image, output_file) != 0) {
        printf("Erreur lors de la sauvegarde de l'image : %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(image);
    SDL_Quit();

    convert_png_to_bmp("list_only.png", "list_only.bmp");

    input_file = "list_only.bmp";
    output_file = "list_only.png";

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }

    // Charger l'image d'entrée
    SDL_Surface *imagedeux = SDL_LoadBMP(input_file);
    if (!imagedeux) {
        printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Étape 1 : Convertir en noir et blanc
    convert_to_black_and_white(imagedeux, bw_threshold);
    // Étape 2 : Nettoyer les grands groupes noirs
    clean_large_black_groups(imagedeux, size_threshold);
    // Étape 3 : Nettoyer les artefacts noirs isolés
    clean_image(imagedeux, neighbor_threshold);

    // Sauvegarder l'image modifiée
    if (IMG_SavePNG(imagedeux, output_file) != 0) {
        printf("Erreur lors de la sauvegarde de l'image : %s\n", SDL_GetError());
        SDL_FreeSurface(imagedeux);
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(imagedeux);
    SDL_Quit();
    }

    if(largeur == 947)
    {
        name = "grid_only.png";
        crop_height = 200;
        crop_image(name, name, crop_height);
    }
    }
    else
    {

    const char *input_file1 = "image.bmp";
    const char *output_file1 = "image.png";

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }

    // Charger l'image d'entrée
    SDL_Surface *imagea = SDL_LoadBMP(input_file1);
    if (!imagea) {
        printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Étape 1 : Convertir en noir et blanc
    Uint8 bw_threshold1 = 128; // Seuil pour convertir en noir et blanc
    convert_to_black_and_white(imagea, bw_threshold1);

    // Étape 2 : Nettoyer les grands groupes noirs
    int size_threshold1 = 260; // Taille minimale pour supprimer un groupe de pixels noirs
    clean_large_black_groups(imagea, size_threshold1);

    // Étape 3 : Nettoyer les artefacts noirs isolés
    int neighbor_threshold1 = 7; // Nombre minimal de voisins blancs pour supprimer un pixel noir
    clean_image(imagea, neighbor_threshold1);

    // Sauvegarder l'image modifiée
    if (IMG_SavePNG(imagea, output_file1) != 0) {
        printf("Erreur lors de la sauvegarde de l'image : %s\n", SDL_GetError());
        SDL_FreeSurface(imagea);
        SDL_Quit();
        return -1;
    }

    // Libérer les ressources
    SDL_FreeSurface(imagea);
    SDL_Quit();
    imageSurface = IMG_Load("image.png");

    // Définir les coordonnées du rectangle de découpe (exemple : x_start, x_end, y_start, y_end)
    int x_start = 30;
    int x_end = 700;
    int y_start = 110;
    int y_end = 670;

    // Appeler la fonction de découpe et enregistrer le résultat
    crop_crop_image(imageSurface, x_start, x_end, y_start, y_end, "grid_only.png");

    // Définir les coordonnées du rectangle de découpe (exemple : x_start, x_end, y_start, y_end)
    x_start = 230;
    x_end = 580;
    y_start = 790;
    y_end = 900;

    // Appeler la fonction de découpe et enregistrer le résultat
    crop_crop_image(imageSurface, x_start, x_end, y_start, y_end, "list_only.png");

    // Libérer la surface de l'image chargée
    SDL_FreeSurface(imageSurface);
    
    // Quitter SDL_image et SDL
    IMG_Quit();
    SDL_Quit();
    }

    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    if(largeur == 841 || largeur == 947)
    {
        combine_images_horizontal_bottom_aligned("list_only.png", "grid_only.png", "interface.png");
    }
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
    }

    return 0;
}