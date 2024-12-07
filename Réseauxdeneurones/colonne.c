#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t *data; // RGB data for PPM image
    int width;
    int height;
} Image;

// Charge une image PPM (format P6)
Image load_ppm_image(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    char format[3];
    int width, height, max_val;

    fscanf(fp, "%2s", format);
    if (strcmp(format, "P6") != 0) {
        fprintf(stderr, "L'image doit être au format PPM binaire (P6).\n");
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d %d %d", &width, &height, &max_val);
    fgetc(fp); // Consomme le caractère de nouvelle ligne

    if (max_val != 255) {
        fprintf(stderr, "La valeur maximale doit être 255.\n");
        fclose(fp);
        exit(1);
    }

    Image img;
    img.width = width;
    img.height = height;
    img.data = (uint8_t *)malloc(3 * width * height);
    if (img.data == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour l'image.\n");
        fclose(fp);
        exit(1);
    }

    fread(img.data, 3, width * height, fp);
    fclose(fp);

    return img;
}

// Libère la mémoire associée à une image
void free_image(Image img) {
    free(img.data);
}

// Fonction pour détecter et regrouper les lignes/colonnes rouges
int count_lines(const int *positions, int size, int gap_threshold) {
    if (size == 0) return 0;

    int count = 1; // On commence avec une ligne/colonne détectée
    for (int i = 1; i < size; i++) {
        if (positions[i] - positions[i - 1] > gap_threshold) {
            count++;
        }
    }
    return count;
}

// Détecte les lignes et colonnes contenant des pixels rouges
void detect_grid_lines(Image img, int *rows, int *columns, int red_threshold, int gap_threshold) {
    int *horizontal_positions = (int *)malloc(img.height * sizeof(int));
    int *vertical_positions = (int *)malloc(img.width * sizeof(int));

    if (!horizontal_positions || !vertical_positions) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les positions.\n");
        exit(1);
    }

    int row_index = 0, col_index = 0;

    // Parcourt les lignes
    for (int y = 0; y < img.height; y++) {
        int red_found = 0;
        for (int x = 0; x < img.width; x++) {
            int index = (y * img.width + x) * 3;
            uint8_t r = img.data[index];
            uint8_t g = img.data[index + 1];
            uint8_t b = img.data[index + 2];

            if (r > red_threshold && g < 50 && b < 50) {
                red_found = 1;
                break;
            }
        }
        if (red_found) {
            horizontal_positions[row_index++] = y;
        }
    }

    // Parcourt les colonnes
    for (int x = 0; x < img.width; x++) {
        int red_found = 0;
        for (int y = 0; y < img.height; y++) {
            int index = (y * img.width + x) * 3;
            uint8_t r = img.data[index];
            uint8_t g = img.data[index + 1];
            uint8_t b = img.data[index + 2];

            if (r > red_threshold && g < 50 && b < 50) {
                red_found = 1;
                break;
            }
        }
        if (red_found) {
            vertical_positions[col_index++] = x;
        }
    }

    // Compte les lignes et colonnes en regroupant celles qui sont proches
    *rows = count_lines(horizontal_positions, row_index, gap_threshold);
    *columns = count_lines(vertical_positions, col_index, gap_threshold);

    free(horizontal_positions);
    free(vertical_positions);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_image.ppm>\n", argv[0]);
        return 1;
    }

    // Charge l'image
    Image img = load_ppm_image(argv[1]);

    // Détecte les lignes et colonnes
    int rows, columns;
    detect_grid_lines(img, &rows, &columns, 200, 5);

    // Écrit le résultat dans un fichier letters.txt
    FILE *output_file = fopen("letters.txt", "w");
    if (!output_file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier letters.txt.\n");
        free_image(img);
        return 1;
    }

    fprintf(output_file, "%d\n", columns);
    fclose(output_file);

    printf("Le fichier letters.txt a été créé avec succès.\n");

    // Libère la mémoire
    free_image(img);

    return 0;
}

