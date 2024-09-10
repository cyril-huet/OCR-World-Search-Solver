#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

gdouble angle = 0.0;
int max_width = 800;
int max_height = 600;
Uint8 threshold = 128;

SDL_Texture* resize_texture_to_fit(SDL_Texture *texture, int original_width, int original_height)
{
    double scale = SDL_min((double)max_width / original_width, (double)max_height / original_height);
    if (scale < 1.0)
    {
        int new_width = (int)(original_width * scale);
        int new_height = (int)(original_height * scale);
        SDL_Texture *resized_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, new_width, new_height);
        SDL_SetRenderTarget(renderer, resized_texture);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_SetRenderTarget(renderer, NULL);
        return resized_texture;
    }
    return texture;
}

void BlackAndWhite(SDL_Surface *surface)
{
    Uint8 *pixels = (Uint8 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    int pitch = surface->pitch;
    int n_channels = surface->format->BytesPerPixel;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint8 *px = pixels + y * pitch + x * n_channels;
            Uint8 red = px[0];
            Uint8 green = px[1];
            Uint8 blue = px[2];
            Uint8 gray = (red + green + blue) / 3;
            Uint8 bw_color = (gray >= threshold) ? 255 : 0;

            px[0] = bw_color;
            px[1] = bw_color;
            px[2] = bw_color;
        }
    }
}

void handle_image_rotation()
{
    angle += 90.0;
    if (angle >= 360.0)
    {
        angle = 0.0;
    }

    SDL_Surface *surface = IMG_Load("image.jpg");
    if (surface != NULL)
    {
        SDL_Texture *rotated_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopyEx(renderer, rotated_texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(rotated_texture);
        SDL_FreeSurface(surface);
    }
}

void handle_image_insert()
{
    SDL_Surface *surface = IMG_Load("image.jpg");
    if (surface != NULL)
    {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        texture = resize_texture_to_fit(texture, surface->w, surface->h);
        SDL_FreeSurface(surface);
    }
}

void handle_image_remove()
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

void handle_image_bw()
{
    SDL_Surface *surface = IMG_Load("image.jpg");
    if (surface != NULL)
    {
        BlackAndWhite(surface);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void handle_close()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Manipulation d'image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, max_width, max_height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_i:
                    handle_image_insert();
                    break;
                case SDLK_r:
                    handle_image_rotation();
                    break;
                case SDLK_d:
                    handle_image_remove();
                    break;
                case SDLK_b:
                    handle_image_bw();
                    break;
                case SDLK_q:
                    handle_close();
                    break;
                default:
                    break;
                }
            }
        }

        if (texture != NULL)
        {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    handle_close();

    return 0;
}
