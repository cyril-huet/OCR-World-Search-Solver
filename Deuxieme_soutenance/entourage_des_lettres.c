#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define THRESHOLD 128 // Seuil pour la binarisation

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

Image create_image(int width, int height) {
    Image img;
    img.width = width;
    img.height = height;
    img.data = (uint8_t *)malloc(width * height);
    if (img.data == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour l'image.\n");
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
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    char format[3];
    int width, height, max_val;

    fscanf(fp, "%2s", format);
    if (format[0] != 'P' || format[1] != '5') {
        fprintf(stderr, "L'image doit être au format PGM binaire (P5).\n");
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d %d %d", &width, &height, &max_val);
    fgetc(fp);

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

void save_pgm_image(const char *filename, Image img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image %s.\n", filename);
        exit(1);
    }

    fprintf(fp, "P5\n%d %d\n255\n", img.width, img.height);
    fwrite(img.data, 1, img.width * img.height, fp);

    fclose(fp);
}

void binarize_image(Image *img) {
    for (int i = 0; i < img->width * img->height; i++) {
        img->data[i] = (img->data[i] < THRESHOLD) ? 0 : 255;
    }
}

void draw_rectangle(Image *img, int x1, int y1, int x2, int y2, uint8_t color) {
    // Dessine un rectangle autour de la région (x1, y1) -> (x2, y2)
    for (int x = x1; x <= x2; x++) {
        if (y1 >= 0 && y1 < img->height) img->data[y1 * img->width + x] = color;
        if (y2 >= 0 && y2 < img->height) img->data[y2 * img->width + x] = color;
    }

    for (int y = y1; y <= y2; y++) {
        if (x1 >= 0 && x1 < img->width) img->data[y * img->width + x1] = color;
        if (x2 >= 0 && x2 < img->width) img->data[y * img->width + x2] = color;
    }
}

void detect_and_extract_letters(Image *src, int min_size, int max_size, int padding) {
    int *visited = (int *)calloc(src->width * src->height, sizeof(int));
    if (!visited) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la matrice visited.\n");
        exit(1);
    }

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            if (src->data[y * src->width + x] == 0 && !visited[y * src->width + x]) {
                int min_x = x, min_y = y, max_x = x, max_y = y;

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

                min_x = (min_x - padding < 0) ? 0 : min_x - padding;
                min_y = (min_y - padding < 0) ? 0 : min_y - padding;
                max_x = (max_x + padding >= src->width) ? src->width - 1 : max_x + padding;
                max_y = (max_y + padding >= src->height) ? src->height - 1 : max_y + padding;

                int letter_width = max_x - min_x + 1;
                int letter_height = max_y - min_y + 1;

                if (letter_width >= min_size && letter_height >= min_size &&
                    letter_width <= max_size && letter_height <= max_size) {
                    // Dessine un rectangle autour de la lettre détectée
                    draw_rectangle(src, min_x, min_y, max_x, max_y, 128);
                }
            }
        }
    }

    free(visited);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_image.pgm>\n", argv[0]);
        return 1;
    }

    Image src = load_pgm_image(argv[1]);

    binarize_image(&src);

    detect_and_extract_letters(&src, 5, 100, 2);

    save_pgm_image("output_with_boxes.pgm", src);

    free_image(src);

    return 0;
}
