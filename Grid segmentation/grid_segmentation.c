#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define THRESHOLD 128  // Seuil pour binarisation

// Structure pour une image en niveaux de gris
typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

// Fonction pour créer une nouvelle image
Image create_image(int width, int height) {
    Image img;
    img.width = width;
    img.height = height;
    img.data = (uint8_t *)malloc(width * height);
    if (img.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire pour l'image.\n");
        exit(1);
    }
    return img;
}

// Fonction pour libérer la mémoire d'une image
void free_image(Image img) {
    free(img.data);
}

// Fonction pour lire une image PGM en niveaux de gris
Image load_pgm_image(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    char format[3];
    int width, height, max_val;
    
    // Lire le format
    fscanf(fp, "%2s", format);
    if (format[0] != 'P' || format[1] != '5') {
        fprintf(stderr, "L'image doit être au format PGM binaire (P5).\n");
        fclose(fp);
        exit(1);
    }

    // Lire les dimensions et la valeur maximale
    fscanf(fp, "%d %d %d", &width, &height, &max_val);
    fgetc(fp);  // Consommer le caractère de nouvelle ligne

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

// Fonction pour sauvegarder une image en format PGM
void save_pgm_image(const char *filename, Image img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    // Écrire l'en-tête du format PGM
    fprintf(fp, "P5\n%d %d\n255\n", img.width, img.height);
    fwrite(img.data, 1, img.width * img.height, fp);

    fclose(fp);
}

// Fonction pour binariser l'image
void binarize_image(Image *img) {
    for (int i = 0; i < img->width * img->height; i++) {
        img->data[i] = (img->data[i] < THRESHOLD) ? 0 : 255;
    }
}

// Fonction pour détecter les contours des lettres et extraire les lettres
void detect_and_extract_letters(Image *src) {
    int visited[src->height][src->width];
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            visited[i][j] = 0;
        }
    }
    
    int label = 0;

    // Détection des contours des lettres
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            if (src->data[y * src->width + x] == 0 && !visited[y][x]) {
                // Début d'une nouvelle lettre
                int min_x = x, min_y = y, max_x = x, max_y = y;

                // DFS pour trouver les contours de la lettre
                int stack[src->height * src->width][2];
                int stack_size = 0;
                stack[stack_size][0] = x;
                stack[stack_size][1] = y;
                stack_size++;

                while (stack_size > 0) {
                    stack_size--;
                    int cur_x = stack[stack_size][0];
                    int cur_y = stack[stack_size][1];

                    if (cur_x < 0 || cur_x >= src->width || cur_y < 0 || cur_y >= src->height || visited[cur_y][cur_x]) {
                        continue;
                    }

                    visited[cur_y][cur_x] = 1;

                    if (src->data[cur_y * src->width + cur_x] == 0) {
                        if (cur_x < min_x) min_x = cur_x;
                        if (cur_y < min_y) min_y = cur_y;
                        if (cur_x > max_x) max_x = cur_x;
                        if (cur_y > max_y) max_y = cur_y;

                        stack[stack_size][0] = cur_x + 1;
                        stack[stack_size][1] = cur_y;
                        stack_size++;
                        stack[stack_size][0] = cur_x - 1;
                        stack[stack_size][1] = cur_y;
                        stack_size++;
                        stack[stack_size][0] = cur_x;
                        stack[stack_size][1] = cur_y + 1;
                        stack_size++;
                        stack[stack_size][0] = cur_x;
                        stack[stack_size][1] = cur_y - 1;
                        stack_size++;
                    }
                }

                // Extraire et sauvegarder la lettre
                int letter_width = max_x - min_x + 1;
                int letter_height = max_y - min_y + 1;
                
                if (letter_width > 0 && letter_height > 0) {
                    Image letter = create_image(letter_width, letter_height);
                    for (int ly = 0; ly < letter_height; ly++) {
                        for (int lx = 0; lx < letter_width; lx++) {
                            letter.data[ly * letter_width + lx] = src->data[(min_y + ly) * src->width + (min_x + lx)];
                        }
                    }

                    char filename[50];
                    snprintf(filename, sizeof(filename), "letter_%d.pgm", label);
                    save_pgm_image(filename, letter);
                    free_image(letter);
                    label++;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_image.pgm>\n", argv[0]);
        return 1;
    }

    // Charger l'image source
    Image src = load_pgm_image(argv[1]);
    
    // Binariser l'image
    binarize_image(&src);
    
    // Détecter les lettres et les extraire
    detect_and_extract_letters(&src);
    
    // Libérer la mémoire de l'image source
    free_image(src);
    
    return 0;
}
