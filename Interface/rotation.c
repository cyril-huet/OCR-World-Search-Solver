#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL2_rotozoom.h>

#define OUTPUT_PATH "output.png"

// Rotates an image by a given angle and saves it to OUTPUT_PATH
int rotate(const char *path, double a) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *img = IMG_Load(path);
    if (img == NULL) {
        fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *rot_img = rotozoomSurface(img, a, 1.0, 1);
    if (rot_img == NULL) {
        fprintf(stderr, "rotozoomSurface Error: %s\n", SDL_GetError());
        SDL_FreeSurface(img);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (IMG_SavePNG(rot_img, OUTPUT_PATH) != 0) {
        fprintf(stderr, "IMG_SavePNG Error: %s\n", IMG_GetError());
    } else {
        printf("Saved rotated image as %s\n", OUTPUT_PATH);
    }

    SDL_FreeSurface(rot_img);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

// Main function to get image path and angle from command line
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <image_path> <angle>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    double a = atof(argv[2]);
    
    return rotate(path, a);
}

