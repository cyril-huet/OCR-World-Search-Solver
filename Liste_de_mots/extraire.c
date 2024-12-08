#include <sys/stat.h>
#include <sys/types.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define THRESHOLD 128 // Seuil pour la binarisation

typedef struct {
    int min_x, min_y, max_x, max_y;
} BoundingBox;

typedef struct {
    BoundingBox *boxes;
    int count;
    int capacity;
} BoundingBoxList;

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

// Création et gestion d'images
Image create_image(int width, int height) {
    Image img;
    img.width = width;
    img.height = height;
    img.data = (uint8_t *)malloc(width * height);
    if (img.data == NULL) {
        fprintf(stderr, "Échec d'allocation mémoire pour l'image.\n");
        exit(1);
    }
    return img;
}

void free_image(Image img) {
    free(img.data);
}

Image load_pgm_image(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    char format[3];
    int width, height, max_val;

    fscanf(fp, "%2s", format);
    if (strcmp(format, "P5") != 0) {
        fprintf(stderr, "L'image doit être au format PGM binaire (P5).\n");
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d %d %d", &width, &height, &max_val);
    fgetc(fp); // Consomme le caractère de fin de ligne

    if (max_val != 255) {
        fprintf(stderr, "La valeur maximale doit être 255.\n");
        fclose(fp);
        exit(1);
    }

    Image img = create_image(width, height);
    fread(img.data, 1, width * height, fp);

    fclose(fp);
    return img;
}

void create_output_directory(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0700) != 0) {
            fprintf(stderr, "Erreur lors de la création du dossier %s.\n", dir_name);
            exit(1);
        }
    }
}

void save_png_image(const char *filename, Image img) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
        img.data, img.width, img.height, 8, img.width, SDL_PIXELFORMAT_INDEX8);

    if (!surface) {
        fprintf(stderr, "SDL_CreateRGBSurfaceWithFormatFrom a échoué: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Color palette[256];
    for (int i = 0; i < 256; i++) {
        palette[i].r = i;
        palette[i].g = i;
        palette[i].b = i;
        palette[i].a = 255;
    }
    SDL_SetPaletteColors(surface->format->palette, palette, 0, 256);

    if (IMG_SavePNG(surface, filename) != 0) {
        fprintf(stderr, "Échec de la sauvegarde PNG: %s\n", IMG_GetError());
        SDL_FreeSurface(surface);
        exit(1);
    }

    SDL_FreeSurface(surface);
}

void binarize_image(Image *img) {
    for (int i = 0; i < img->width * img->height; i++) {
        img->data[i] = (img->data[i] < THRESHOLD) ? 0 : 255;
    }
}

// Détection des lettres dans l'image
BoundingBoxList detect_letters(Image *src, int min_size, int max_size) {
    int *visited = (int *)calloc(src->width * src->height, sizeof(int));
    if (!visited) {
        fprintf(stderr, "Échec d'allocation mémoire pour le tableau visited.\n");
        exit(1);
    }

    BoundingBoxList list = {NULL, 0, 10};
    list.boxes = (BoundingBox *)malloc(list.capacity * sizeof(BoundingBox));

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            if (src->data[y * src->width + x] == 0 && !visited[y * src->width + x]) {
                BoundingBox box = {x, y, x, y};

                // Flood fill pour trouver les limites de la lettre
                int stack[src->width * src->height][2];
                int stack_size = 0;
                stack[stack_size][0] = x;
                stack[stack_size][1] = y;
                stack_size++;

                while (stack_size > 0) {
                    stack_size--;
                    int cur_x = stack[stack_size][0];
                    int cur_y = stack[stack_size][1];

                    if (cur_x < 0 || cur_x >= src->width || cur_y < 0 || cur_y >= src->height) continue;
                    if (visited[cur_y * src->width + cur_x] || src->data[cur_y * src->width + cur_x] != 0) continue;

                    visited[cur_y * src->width + cur_x] = 1;

                    if (cur_x < box.min_x) box.min_x = cur_x;
                    if (cur_y < box.min_y) box.min_y = cur_y;
                    if (cur_x > box.max_x) box.max_x = cur_x;
                    if (cur_y > box.max_y) box.max_y = cur_y;

                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            if (dx != 0 || dy != 0) {
                                stack[stack_size][0] = cur_x + dx;
                                stack[stack_size][1] = cur_y + dy;
                                stack_size++;
                            }
                        }
                    }
                }

                int width = box.max_x - box.min_x + 1;
                int height = box.max_y - box.min_y + 1;

                if (width >= min_size && height >= min_size && width <= max_size && height <= max_size) {
                    if (list.count == list.capacity) {
                        list.capacity *= 2;
                        list.boxes = (BoundingBox *)realloc(list.boxes, list.capacity * sizeof(BoundingBox));
                    }
                    list.boxes[list.count++] = box;
                }
            }
        }
    }

    free(visited);
    return list;
}

// Extraire le numéro du mot à partir du nom du fichier
int extract_word_number(const char *input_filename) {
    const char *last_slash = strrchr(input_filename, '/');
    const char *basename = last_slash ? last_slash + 1 : input_filename;

    const char *mot_position = strstr(basename, "mot");
    if (mot_position) {
        return atoi(mot_position + 3); // Convertir la partie numérique après "mot"
    }
    return 0; // Retourne 0 si aucun numéro n'est trouvé
}

// Groupement et sauvegarde des lettres
void group_and_save_letters(Image *src, BoundingBoxList *list, const char *input_filename) {
    int word_number = extract_word_number(input_filename);

    const char *output_dir = "images";
    create_output_directory(output_dir);

    int letter_counter = 1;

    for (int i = 0; i < list->count; i++) {
        int width = list->boxes[i].max_x - list->boxes[i].min_x + 1;
        int height = list->boxes[i].max_y - list->boxes[i].min_y + 1;

        Image letter = create_image(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                letter.data[y * width + x] =
                    src->data[(list->boxes[i].min_y + y) * src->width + (list->boxes[i].min_x + x)];
            }
        }

        char filename[200];
        snprintf(filename, sizeof(filename), "%s/lettre_%d_%d.png", output_dir, word_number, letter_counter++);
        save_png_image(filename, letter);
        free_image(letter);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilisation: %s <image_entree.pgm>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Échec de SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        fprintf(stderr, "Échec de IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    Image src = load_pgm_image(argv[1]);
    binarize_image(&src);

    BoundingBoxList list = detect_letters(&src, 1, 200);
    group_and_save_letters(&src, &list, argv[1]);

    free(list.boxes);
    free_image(src);

    IMG_Quit();
    SDL_Quit();

    return 0;
}

