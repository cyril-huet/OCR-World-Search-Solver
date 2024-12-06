#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define THRESHOLD 128  // Threshold for binarization

// Structure for a grayscale image
typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

// Function to create a new image
Image create_image(int width, int height) 
{
    Image img;
    if (width > 0 && height > 0) 
    {
        img.width = width;
        img.height = height;
        img.data = (uint8_t *)malloc(width * height);
        if (img.data == NULL) {
            exit(1);
        }
        return img;
    }
    else 
    {
        fprintf(stderr, "Invalid image dimensions.\n");
        exit(1);
    }
    
}

// Function to free the memory of an image
void free_image(Image img) 
{
    free(img.data);
}

// Function to read a grayscale PGM image
Image load_pgm_image(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error opening file %s.\n", filename);
        exit(1);
    }

    char format[3];
    int width, height, max_val;
    
    // Read the format
    fscanf(fp, "%2s", format);
    if (format[0] != 'P' || format[1] != '5') {
        fprintf(stderr, "The image must be in binary PGM format (P5).\n");
        fclose(fp);
        exit(1);
    }

    // Read the dimensions and the maximum value
    fscanf(fp, "%d %d %d", &width, &height, &max_val);
    fgetc(fp);  // Consume the newline character

    if (max_val != 255) {
        fprintf(stderr, "The maximum value must be 255.\n");
        fclose(fp);
        exit(1);
    }

    Image img = create_image(width, height);
    fread(img.data, 1, width * height, fp);

    fclose(fp);
    return img;
}

// Function to save an image in PGM format
void save_pgm_image(const char *filename, Image img) 
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error opening file %s.\n", filename);
        exit(1);
    }

    // Write the PGM format header
if (img.width > 0 && img.height > 0) {
       
        fprintf(fp, "P5\n%d %d\n255\n", img.width, img.height);
        fwrite(img.data, 1, img.width * img.height, fp);
    }

    fclose(fp);
}

// Function to binarize the image
void binarize_image(Image *img) {
    for (int i = 0; i < img->width * img->height; i++) {
        img->data[i] = (img->data[i] < THRESHOLD) ? 0 : 255;
    }
}

// Function to detect the contours of letters and extract them
void detect_and_extract_letters(Image *src) {
    int visited[src->height][src->width];
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            visited[i][j] = 0;
        }
    }
    
    int label = 0;

    // Detecting the contours of letters
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            if (src->data[y * src->width + x] == 0 && !visited[y][x]) {
                // Beginning of a new letter
                int min_x = x, min_y = y, max_x = x, max_y = y;

                // DFS to find the contours of the letter
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

                // Extract and save the letter
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

int main(int argc, char *argv[]) 
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_image.pgm>\n", argv[0]);
        return 1;
    }
    if (argc > 2) {
        fprintf(stderr, "Usage: %s <input_image.pgm>\n", argv[0]);
        return 1;
    }

    // Load the source image
    Image src = load_pgm_image(argv[1]);
    
    // Binarize the image
    binarize_image(&src);
    
    // Detect letters and extract them
    detect_and_extract_letters(&src);
    
    // Free the memory of the source image
    free_image(src);
    
    return 0;
}
