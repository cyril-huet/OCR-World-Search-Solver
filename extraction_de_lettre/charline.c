#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    int x;
    int y;
} Pixel;

void set_pixels_white(const char *input_file, const char *output_file, Pixel *pixels, int num_pixels) {
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

    // Mettre les pixels spécifiés en blanc
    for (int i = 0; i < num_pixels; i++) {
        int x = pixels[i].x;
        int y = pixels[i].y;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            image_data[y * width + x] = (unsigned char)max_val;
        } else {
            fprintf(stderr, "Coordonnée (%d, %d) hors limites, ignorée\n", x, y);
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

    printf("Les pixels spécifiés ont été mis en blanc dans le fichier %s\n", output_file);
}

int main() {
    const char *input_file = "input.pgm";
    const char *output_file = "output.pgm";

    // Liste des pixels à modifier
    Pixel pixels_to_white[] = 
{
        {322,97},
        {59,3},
        {53,95},
        {121,62},
        {142,67},
        {142,68},
        {251,66},
        {252,66},
        {251,67},
        {252,67},
        {260,66},
        {261,66},
        {261,67},
        {120,32},
        {121,32},
        {150,63}


    };
    int num_pixels = sizeof(pixels_to_white) / sizeof(Pixel);

    // Mettre les pixels spécifiés en blanc
    set_pixels_white(input_file, output_file, pixels_to_white, num_pixels);

    return 0;
}
