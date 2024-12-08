#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void set_pixels_after_line_white(const char *input_file, const char *output_file, int start_line) {
    FILE *fp_in, *fp_out;
    char format[3];
    int width, height, max_val;
    unsigned char *image_data;

    // Ouvrir le fichier PGM en lecture
    fp_in = fopen(input_file, "rb");
    if (!fp_in) {
        perror("Erreur d'ouverture du fichier PGM");
        exit(EXIT_FAILURE);
    }

    // Lire le format (P5 attendu)
    fscanf(fp_in, "%s", format);
    if (strcmp(format, "P5") != 0) {
        fprintf(stderr, "Format PGM non supporté : %s\n", format);
        fclose(fp_in);
        exit(EXIT_FAILURE);
    }

    // Lire les dimensions de l'image
    fscanf(fp_in, "%d %d", &width, &height);
    fscanf(fp_in, "%d", &max_val);
    fgetc(fp_in); // Sauter le caractère de nouvelle ligne

    // Vérifier si la ligne de départ est valide
    if (start_line < 0 || start_line >= height) {
        fprintf(stderr, "Ligne de départ (%d) hors limites pour une image de hauteur %d\n", start_line, height);
        fclose(fp_in);
        exit(EXIT_FAILURE);
    }

    // Allouer la mémoire pour les données de l'image
    image_data = (unsigned char *)malloc(width * height);
    if (!image_data) {
        perror("Erreur d'allocation mémoire");
        fclose(fp_in);
        exit(EXIT_FAILURE);
    }

    // Lire les données de l'image
    fread(image_data, 1, width * height, fp_in);
    fclose(fp_in);

    // Mettre tous les pixels après la ligne spécifiée en blanc
    for (int y = start_line + 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image_data[y * width + x] = (unsigned char)max_val;
        }
    }

    // Écrire l'image modifiée dans un nouveau fichier
    fp_out = fopen(output_file, "wb");
    if (!fp_out) {
        perror("Erreur d'ouverture du fichier de sortie");
        free(image_data);
        exit(EXIT_FAILURE);
    }

    fprintf(fp_out, "P5\n%d %d\n%d\n", width, height, max_val);
    fwrite(image_data, 1, width * height, fp_out);

    fclose(fp_out);
    free(image_data);

    printf("Tous les pixels après la ligne %d ont été mis en blanc dans le fichier %s\n", start_line, output_file);
}

int main() {
    const char *input_file = "input.pgm";
    const char *output_file = "output.pgm";
    int start_line = 410; // Ligne après laquelle tous les pixels seront blancs

    // Modifier l'image
    set_pixels_after_line_white(input_file, output_file, start_line);

    return 0;
}
