#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 185
#define PINK_THRESHOLD 52
#define COLOR_THRESHOLD 50

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

// Detects colored letters in the image by checking if the RGB values
// significantly differ. If they do, the pixel is set to black.
void colorize(SDL_Surface *image) {
    int width = image->w;
    int height = image->h;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = getPixel(image, x, y);
            Uint8 red, green, blue;
            SDL_GetRGB(pixel, image->format, &red, &green, &blue);
            
            if (abs(red - green) > COLOR_THRESHOLD || abs(red - blue) > COLOR_THRESHOLD || abs(green - blue) > COLOR_THRESHOLD) {
                setPixel(image, x, y, SDL_MapRGB(image->format, 0, 0, 0));
            }
        }
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_path>\n", argv[0]);
        return 1;
    }
    
    const char *image_path = argv[1];
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Surface *image = IMG_Load(image_path);
    if (!image) {
        fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    
    colorize(image);
    blur(image);
    binarize(image);
    
    if (IMG_SavePNG(image, "output.png") != 0) {
        fprintf(stderr, "Could not save image: %s\n", IMG_GetError());
    } else {
        printf("Image processed and saved as output.png\n");
    }
    
    SDL_FreeSurface(image);
    SDL_Quit();
    
    return 0;
}

