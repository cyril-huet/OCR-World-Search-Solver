#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void convert_pgm_to_ppm(const char *input_pgm, const char *output_ppm) {
    FILE *pgm_file = fopen(input_pgm, "rb");
    if (!pgm_file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s.\n", input_pgm);
        exit(EXIT_FAILURE);
    }

    // Lire l'en-tête du fichier PGM
    char format[3];
    int width, height, max_val;

    fscanf(pgm_file, "%2s", format);
    if (strcmp(format, "P5") != 0) {
        fprintf(stderr, "Le fichier doit être au format PGM binaire (P5).\n");
        fclose(pgm_file);
        exit(EXIT_FAILURE);
    }

    fscanf(pgm_file, "%d %d %d", &width, &height, &max_val);
    fgetc(pgm_file); // Consomme le caractère de nouvelle ligne

    if (max_val != 255) {
        fprintf(stderr, "La valeur maximale doit être 255.\n");
        fclose(pgm_file);
        exit(EXIT_FAILURE);
    }

    // Lire les données de l'image PGM
    unsigned char *pgm_data = (unsigned char *)malloc(width * height);
    if (!pgm_data) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les données PGM.\n");
        fclose(pgm_file);
        exit(EXIT_FAILURE);
    }

    fread(pgm_data, 1, width * height, pgm_file);
    fclose(pgm_file);

    // Ouvrir le fichier PPM pour écriture
    FILE *ppm_file = fopen(output_ppm, "wb");
    if (!ppm_file) {
        fprintf(stderr, "Erreur lors de la création du fichier %s.\n", output_ppm);
        free(pgm_data);
        exit(EXIT_FAILURE);
    }

    // Écrire l'en-tête du fichier PPM
    fprintf(ppm_file, "P6\n%d %d\n255\n", width, height);

    // Écrire les données en nuances de gris dans le fichier PPM
    for (int i = 0; i < width * height; i++) {
        unsigned char gray = pgm_data[i];
        fwrite(&gray, 1, 1, ppm_file); // R
        fwrite(&gray, 1, 1, ppm_file); // G
        fwrite(&gray, 1, 1, ppm_file); // B
    }

    // Libérer la mémoire et fermer le fichier
    free(pgm_data);
    fclose(ppm_file);

    printf("Conversion de %s en %s réussie.\n", input_pgm, output_ppm);
}

int main() {
    const char *input_pgm = "grid_only.pgm";
    const char *output_ppm = "grid_only.ppm";

    convert_pgm_to_ppm(input_pgm, output_ppm);

    return 0;
}
