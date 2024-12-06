#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define GRID_COLOR 255, 0, 0  // red for the grid
#define LIST_COLOR 0, 0, 255  // blue for the list

void detectAndDrawRegions(SDL_Surface* imageSurface, SDL_Renderer* renderer) 
{
    SDL_Surface* graySurface = SDL_ConvertSurfaceFormat(imageSurface, 
SDL_PIXELFORMAT_ARGB8888, 0);
    if (graySurface == NULL) {
        printf("Erreur lors de la conversion de l'image : %s\n", 
SDL_GetError());
        return;
    }

    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, graySurface->w, 
graySurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    // Step 1
    for (int y = 0; y < graySurface->h; y++) 
	{
        for (int x = 0; x < graySurface->w; x++) 
	{
            Uint32 pixel = ((Uint32*)graySurface->pixels)[y * graySurface->w + 
x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, graySurface->format, &r, &g, &b, &a);

            Uint8 gray = (r + g + b) / 3;
            ((Uint32*)outputSurface->pixels)[y * outputSurface->w + x] = 
SDL_MapRGB(outputSurface->format, gray, gray, gray);
        }
    }

    // Step 2
    int* horizontalEdges = (int*)calloc(outputSurface->h, sizeof(int));
    int* verticalEdges = (int*)calloc(outputSurface->w, sizeof(int));

    for (int y = 0; y < outputSurface->h - 1; y++) 
	{
        for (int x = 0; x < outputSurface->w; x++) 
	{
            Uint32 currentPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + x];
            Uint32 nextPixel = ((Uint32*)outputSurface->pixels)[(y + 1) * 
outputSurface->w + x];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, outputSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, outputSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > 25) {
                horizontalEdges[y]++;
            }
        }
    }

    for (int x = 0; x < outputSurface->w - 1; x++) {
        for (int y = 0; y < outputSurface->h; y++) {
            Uint32 currentPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + x];
            Uint32 nextPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + (x + 1)];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, outputSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, outputSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > 25) {
                verticalEdges[x]++;
            }
        }
    }

    // Step 3
    int topLimit = 0, bottomLimit = outputSurface->h - 1;
    int leftLimit = 0, rightLimit = outputSurface->w - 1;

    for (int y = 0; y < outputSurface->h; y++) {
        if (horizontalEdges[y] > outputSurface->w * 1.5) {
            topLimit = y;
            break;
        }
    }

    for (int y = outputSurface->h - 1; y >= 0; y--) {
        if (horizontalEdges[y] > outputSurface->w * 0.8) {
            bottomLimit = y;
            break;
        }
    }

    for (int x = 0; x < outputSurface->w; x++) {
        if (verticalEdges[x] > outputSurface->h * 0.23) {
            leftLimit = x;
            break;
        }
    }

    for (int x = outputSurface->w - 1; x >= 0; x--) {
        if (verticalEdges[x] > outputSurface->h * 0.5) {
            rightLimit = x;
            break;
        }
    }

    // Step 4
    int topEmptySpace = topLimit;
    int bottomEmptySpace = outputSurface->h - bottomLimit - 1;
    int leftEmptySpace = leftLimit;
    int rightEmptySpace = outputSurface->w - rightLimit - 1;

    SDL_Rect gridRect = {leftLimit, topLimit, rightLimit - leftLimit, 
bottomLimit - topLimit};
    SDL_Rect listRect = {0, 0, 0, 0};

    if (bottomEmptySpace > 50 && bottomEmptySpace > topEmptySpace) {
        listRect.x = leftLimit;
        listRect.y = bottomLimit + 1;
        listRect.w = rightLimit - leftLimit;
        listRect.h = outputSurface->h - bottomLimit - 1;
    } else if (topEmptySpace > 50 && topEmptySpace > bottomEmptySpace) {
        listRect.x = leftLimit;
        listRect.y = 0;
        listRect.w = rightLimit - leftLimit;
        listRect.h = topLimit;
    } else if (rightEmptySpace > 50 && rightEmptySpace > leftEmptySpace) {
        listRect.x = rightLimit + 1;
        listRect.y = topLimit;
        listRect.w = outputSurface->w - rightLimit - 1;
        listRect.h = bottomLimit - topLimit;
    } else if (leftEmptySpace > 50 && leftEmptySpace > rightEmptySpace) {
        listRect.x = 0;
        listRect.y = topLimit;
        listRect.w = leftLimit;
        listRect.h = bottomLimit - topLimit;
    }

    SDL_SetRenderDrawColor(renderer, GRID_COLOR, 255);
    SDL_RenderDrawRect(renderer, &gridRect);

    SDL_SetRenderDrawColor(renderer, LIST_COLOR, 255);
    if (listRect.w > 0 && listRect.h > 0) {
        SDL_RenderDrawRect(renderer, &listRect);
    }

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(graySurface);
    SDL_FreeSurface(outputSurface);
    free(horizontalEdges);
    free(verticalEdges);
}

void loadAndDisplayImage(const char* imagePath) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* imageSurface = IMG_Load(imagePath);
    if (imageSurface == NULL) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Contours and Rectangle", 
SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, imageSurface->w, 
imageSurface->h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
SDL_RENDERER_ACCELERATED);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, 
imageSurface);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);
    detectAndDrawRegions(imageSurface, renderer);

    SDL_Delay(10000);

    SDL_FreeSurface(imageSurface);
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }
    loadAndDisplayImage(argv[1]);
    return 0;
}
