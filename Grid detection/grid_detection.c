#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define GRID_COLOR 255, 0, 0 // red for the grid
#define LIST_COLOR 0, 0, 255 // blue for the word list


//Grid detection in 3 steps, first we analyse the image and detect where the 
//word list and letter grid are located. We save the two distinct parts in two 
//different images


//function for saving an image
void saveSurfaceAsImage(SDL_Surface* surface, const char* filename) 
{
    if (IMG_SavePNG(surface, filename) != 0) 
    {                                  
	    printf("Error saving image %s : %s\n", filename, SDL_GetError());
    }
}



//function that detects the word list on one side, and the grid on the other
void detectAndDrawRegions(SDL_Surface* imageSurface) 
{
    SDL_Surface* graySurface = SDL_ConvertSurfaceFormat(imageSurface, 
SDL_PIXELFORMAT_ARGB8888, 0);
    if (graySurface == NULL) 
    {
        printf("Image conversion error : %s\n", SDL_GetError());
        return;
    }

    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, graySurface->w, 
graySurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    // Step 1 : grayscale conversion
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

    // Step 2 : horizontal and vertical line detection
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

		if (abs(grayCurrent - grayNext) > 25) 
		{
			horizontalEdges[y]++;
        	}
    	}
    }

    for (int x = 0; x < outputSurface->w - 1; x++) 
    {
        for (int y = 0; y < outputSurface->h; y++) 
	{
            Uint32 currentPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + x];
            Uint32 nextPixel = ((Uint32*)outputSurface->pixels)[y * 
outputSurface->w + (x + 1)];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, outputSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, outputSurface->format, &grayNext, &grayNext, 
&grayNext);

	    if (abs(grayCurrent - grayNext) > 25) 
	    {
                verticalEdges[x]++;
            }
        }
    }

    int grand = 735;
    int petit = 684;

    // Step 3 : Determine precise grid borders
    int topLimit = 0, bottomLimit = outputSurface->h - 1;
    int leftLimit = 0, rightLimit = outputSurface->w - 1;

    float haut = 1.5;
    float bas = 0.8;
    float gauche = 0.23;
    float droite = 0.5;

    if(imageSurface->w == grand)
    {
 	haut = 0.5; gauche = 0.1;
    }
    else if(imageSurface->w == petit)
    {
	haut = 0.5; bas = 0.5; gauche = 1.0;
    }

    for (int y = 0; y < outputSurface->h; y++) 
    {
        if (horizontalEdges[y] > outputSurface->w * haut) 
	{
            topLimit = y;
            break;
        }
    }

    for (int y = outputSurface->h - 1; y >= 0; y--) 
    {
        if (horizontalEdges[y] > outputSurface->w * bas) 
	{
            bottomLimit = y;
            break;
        }
    }

    for (int x = 0; x < outputSurface->w; x++) 
    {
        if (verticalEdges[x] > outputSurface->h * gauche) 
	{
            leftLimit = x;
            break;
        }
    }

    for (int x = outputSurface->w - 1; x >= 0; x--) 
    {
        if (verticalEdges[x] > outputSurface->h * droite) 
	{
            rightLimit = x;
            break;
        }
    }

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

    // Creation of surfaces to safeguard
    SDL_Surface* gridSurface = SDL_CreateRGBSurface(0, gridRect.w, gridRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
    SDL_Surface* listSurface = SDL_CreateRGBSurface(0, listRect.w, listRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    // Copy of regions in new surfaces
    SDL_BlitSurface(imageSurface, &gridRect, gridSurface, NULL);
    SDL_BlitSurface(imageSurface, &listRect, listSurface, NULL);

    // Saving surfaces
    saveSurfaceAsImage(gridSurface, "grid_only.pgm");
    saveSurfaceAsImage(listSurface, "list_only.pgm");

    SDL_FreeSurface(gridSurface);
    SDL_FreeSurface(listSurface);
    SDL_FreeSurface(graySurface);
    SDL_FreeSurface(outputSurface);
    free(horizontalEdges);
    free(verticalEdges);
}


//Second step of our grid detection: now that we have our two parts separated, 
//we'll cut out th unnecessary noise to obtain only our characters, we'll cut 
//out the white outline or the animals...


void detectAndDrawGrid(SDL_Surface* imageSurface)
{

    SDL_Surface* graySurface = SDL_ConvertSurfaceFormat(imageSurface, 
SDL_PIXELFORMAT_ARGB8888, 0);

    if (graySurface == NULL) 
    {
	    printf("error : %s\n", SDL_GetError());
	    return;
    }

    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, graySurface->w, 
graySurface->h, 32,0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    for (int y = 0; y < graySurface->h; y++) 
    {
	    for (int x = 0; x < graySurface->w; x++) 
	    {
		    Uint32 pixel = ((Uint32*)graySurface->pixels)[y * 
graySurface->w + x];
		    Uint8 r, g, b, a;
		    SDL_GetRGBA(pixel, graySurface->format, &r, &g, &b, &a);
		    Uint8 gray = (r + g + b) / 3;
		    ((Uint32*)outputSurface->pixels)[y * outputSurface->w + x] 
= SDL_MapRGB(outputSurface->format, gray, gray, gray);
	    }
    }

	//pre-treatment filter to reinforce grid lines
	//use of a simple Sobel filter to reinforce horizontal and vertical 
//contours
	int kernelSize = 3;
	int sobelKernelX[3][3] = 
	{
        	{-1, 0, 1},
        	{-2, 0, 2},
        	{-1, 0, 1}
    	};
	int sobelKernelY[3][3] = 
	{
        	{-1, -2, -1},
        	{0, 0, 0},
        	{1, 2, 1}
    	};

	SDL_Surface* edgeSurface = SDL_CreateRGBSurface(0, outputSurface->w, 
outputSurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);


    for (int y = 1; y < outputSurface->h - 1; y++) 
    {
        for (int x = 1; x < outputSurface->w - 1; x++) 
	{
            int gx = 0, gy = 0;

            for (int ky = 0; ky < kernelSize; ky++) 
	    {
                for (int kx = 0; kx < kernelSize; kx++) 
		{
                    int px = x + kx - 1;
                    int py = y + ky - 1;
                    Uint32 pixel = ((Uint32*)outputSurface->pixels)[py * 
outputSurface->w + px];
                    Uint8 grayValue;
                    SDL_GetRGB(pixel, outputSurface->format, &grayValue, 
&grayValue, &grayValue);
                    gx += sobelKernelX[ky][kx] * grayValue;
                    gy += sobelKernelY[ky][kx] * grayValue;
                }
            }

            int gradient = abs(gx) + abs(gy);
            gradient = gradient > 255 ? 255 : gradient;
            ((Uint32*)edgeSurface->pixels)[y * edgeSurface->w + x] = 
SDL_MapRGB(edgeSurface->format, gradient, gradient, gradient);
        }
    }

    // Step 2 : edge detection using reinforced contours
    int* horizontalEdges = (int*)calloc(edgeSurface->h, sizeof(int));
    int* verticalEdges = (int*)calloc(edgeSurface->w, sizeof(int));

    int edgeThreshold = 50;  
    // threshold to detect grid lines and ignore small contours

    for (int y = 0; y < edgeSurface->h - 1; y++) 
    {
        for (int x = 0; x < edgeSurface->w; x++) 
	{
            Uint32 currentPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + x];
            Uint32 nextPixel = ((Uint32*)edgeSurface->pixels)[(y + 1) * 
edgeSurface->w + x];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, edgeSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, edgeSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > edgeThreshold) 
	    {
                horizontalEdges[y]++;
            }
        }
    }

    for (int x = 0; x < edgeSurface->w - 1; x++) 
    {
        for (int y = 0; y < edgeSurface->h; y++) 
	{
            Uint32 currentPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + x];
            Uint32 nextPixel = ((Uint32*)edgeSurface->pixels)[y * 
edgeSurface->w + (x + 1)];

            Uint8 grayCurrent, grayNext;
            SDL_GetRGB(currentPixel, edgeSurface->format, &grayCurrent, 
&grayCurrent, &grayCurrent);
            SDL_GetRGB(nextPixel, edgeSurface->format, &grayNext, &grayNext, 
&grayNext);

            if (abs(grayCurrent - grayNext) > edgeThreshold) 
	    {
                verticalEdges[x]++;
            }
        }
    }

    // Step 3 : Identification of grid limits
    int topLimit = 0, bottomLimit = edgeSurface->h - 1;
    int leftLimit = 0, rightLimit = edgeSurface->w - 1;

    for (int y = 0; y < edgeSurface->h; y++) 
    {
        if (horizontalEdges[y] > edgeSurface->w * 0.8) 
	{
            topLimit = y;
            break;
        }
    }

    for (int y = edgeSurface->h - 1; y >= 0; y--) 
    {
        if (horizontalEdges[y] > edgeSurface->w * 0.8) 
	{
            bottomLimit = y;
            break;
        }
    }

    for (int x = 0; x < edgeSurface->w; x++) 
    {
        if (verticalEdges[x] > edgeSurface->h * 0.8) 
	{
            leftLimit = x;
            break;
        }
    }

    for (int x = edgeSurface->w - 1; x >= 0; x--) 
    {
        if (verticalEdges[x] > edgeSurface->h * 0.8) 
	{
            rightLimit = x;
            break;
        }
    }

    SDL_Rect gridRect = {leftLimit, topLimit, rightLimit - leftLimit, 
bottomLimit - topLimit};

    // Save detected grid
    SDL_Surface* gridSurface = SDL_CreateRGBSurface(0, gridRect.w, gridRect.h, 
32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
    SDL_BlitSurface(imageSurface, &gridRect, gridSurface, NULL);
    saveSurfaceAsImage(gridSurface, "grid_only.pgm");

    SDL_FreeSurface(gridSurface);
    SDL_FreeSurface(graySurface);
    SDL_FreeSurface(outputSurface);
    SDL_FreeSurface(edgeSurface);
    free(horizontalEdges);
    free(verticalEdges);
}

//And finally, the last step will be used to fine_tune the imperfections to 
//obtain the best possible grid detection.

void detectGridArea(SDL_Surface* imageSurface, SDL_Rect* gridRect) 
{
    int width = imageSurface->w;
    int height = imageSurface->h;

    int topLimit = height;
    int bottomLimit = 0;
    int leftLimit = width;
    int rightLimit = 0;

    // scan each pixel to detect grid edges
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
	{
            Uint32 pixel = ((Uint32*)imageSurface->pixels)[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, imageSurface->format, &r, &g, &b, &a);

            // detection of black grid lines
            if (r < 100 && g < 100 && b < 100) 
	    { 
                if (y < topLimit) topLimit = y;
                if (y > bottomLimit) bottomLimit = y;
                if (x < leftLimit) leftLimit = x;
                if (x > rightLimit) rightLimit = x;
            }
        }
    }

    // boundary adjustement to avoid white zones detected around the boundary
    
    gridRect->x = leftLimit;
    gridRect->y = topLimit;
    gridRect->w = (rightLimit - leftLimit) - 2;
    gridRect->h = (bottomLimit - topLimit) - 2;

}


int main(int argc, char** argv)
{

	if(argc < 2)
	{
		printf("error argv");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	SDL_Surface* imageSurface = IMG_Load(argv[1]);

	if(imageSurface == NULL)
	{
		printf("error : %s\n", IMG_GetError());
		SDL_Quit();
		return 1;
	}
	
	
	SDL_Rect gridRect;

	detectAndDrawRegions(imageSurface);
	if((strcmp(argv[1], "image2.1.bmp")))
	detectAndDrawGrid(IMG_Load("grid_only.pgm"));


	if(!(imageSurface->w == 841) && (!(imageSurface->w == 770)))
	{
	detectGridArea(IMG_Load("grid_only.pgm"), &gridRect);

	// Creates a surface for the grid only
	SDL_Surface* gridSurface = SDL_CreateRGBSurface(0, gridRect.w, 
gridRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
	SDL_BlitSurface(IMG_Load("grid_only.pgm"), &gridRect, gridSurface, 
NULL);

	// saves the extracted grid for verification
	saveSurfaceAsImage(gridSurface, "grid_only.pgm");
	

	SDL_FreeSurface(gridSurface);}

	SDL_FreeSurface(imageSurface);
	IMG_Quit();
    	SDL_Quit();

    	return 0;
}
