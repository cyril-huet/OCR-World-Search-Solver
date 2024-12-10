#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINES 50
#define MAX_COLUMNS 50
#define MAX_WORDS 100
#define MAX_WORD_LENGTH 20

typedef struct {
    int start_line, start_col;
    int end_line, end_col;
    char found_word[MAX_WORD_LENGTH];
} FoundWord;

int directions[8][2] = {
    {0, 1}, {0, -1},
    {1, 0}, {-1, 0},
    {1, 1}, {-1, -1},
    {1, -1}, {-1, 1}
};

int findWordInGrid(char grid[MAX_LINES][MAX_COLUMNS], int lines, int cols, const char *word, FoundWord *res) {
    int length = (int)strlen(word);
    for (int line = 0; line < lines; line++) {
        for (int col = 0; col < cols; col++) {
            for (int d = 0; d < 8; d++) {
                int dx = directions[d][0];
                int dy = directions[d][1];
                int i, l = line, c = col;
                for (i = 0; i < length; i++) {
                    if (l < 0 || l >= lines || c < 0 || c >= cols || grid[l][c] != word[i]) break;
                    l += dx;
                    c += dy;
                }
                if (i == length) {
                    res->start_line = line;
                    res->start_col = col;
                    res->end_line = line + (length - 1) * dx;
                    res->end_col = col + (length - 1) * dy;
                    strncpy(res->found_word, word, MAX_WORD_LENGTH);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int loadGridFromFile(const char *filename, char grid[MAX_LINES][MAX_COLUMNS], int *lines, int *cols) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    *lines = 0;
    int initial_width = -1;
    while (fgets(grid[*lines], MAX_COLUMNS, f) && *lines < MAX_LINES) {
        grid[*lines][strcspn(grid[*lines], "\n")] = '\0'; // Remove newline
        int line_length = (int)strlen(grid[*lines]);
        if (line_length == 0) continue; // Skip empty lines
        if (initial_width == -1) initial_width = line_length;
        if (line_length != initial_width) {
            printf("Error: Line %d has inconsistent length (%d vs %d)\n", *lines + 1, line_length, initial_width);
            fclose(f);
            return 0;
        }
        (*lines)++;
    }
    if (*lines == 0) {
        printf("Error: File %s is empty or invalid\n", filename);
        fclose(f);
        return 0;
    }
    *cols = (int)strlen(grid[0]);
    fclose(f);
    printf("File %s loaded successfully: %d lines, %d columns\n", filename, *lines, *cols);
    return 1;
}

int loadWordsFromFile(const char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH], int *word_count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    *word_count = 0;
    while (fgets(words[*word_count], MAX_WORD_LENGTH, f) && *word_count < MAX_WORDS) {
        words[*word_count][strcspn(words[*word_count], "\n")] = '\0'; // Remove newline
        if (strlen(words[*word_count]) > 0) {
            for (int i = 0; words[*word_count][i]; i++) {
                words[*word_count][i] = (char)toupper((unsigned char)words[*word_count][i]);
            }
            (*word_count)++;
        }
    }
    fclose(f);
    printf("File %s loaded successfully: %d words\n", filename, *word_count);
    return 1;
}

void saveImage(SDL_Renderer *renderer, int width, int height, const char *file_name) {
    SDL_RenderPresent(renderer);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return;
    }
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        printf("Failed to read pixels: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    if (SDL_SaveBMP(surface, file_name) != 0) {
        printf("Failed to save BMP: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);
}

void drawWordList(SDL_Renderer *renderer, char words[MAX_WORDS][MAX_WORD_LENGTH], int word_count, int width, int height) {
    int y = 0;
    int line_height = height / word_count;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < word_count; i++) {
        SDL_Surface *surface = TTF_RenderText_Solid(/* Your TTF font and text here */, words[i], {0, 0, 0, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {10, y, width - 20, line_height};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y += line_height;
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    char grid[MAX_LINES][MAX_COLUMNS];
    int lines, cols;
    if (!loadGridFromFile("matrice_grid.txt", grid, &lines, &cols)) return 1;

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int word_count;
    if (!loadWordsFromFile("list_grid.txt", words, &word_count)) return 1;

    FoundWord results[MAX_WORDS];
    int foundCount = 0;
    for (int i = 0; i < word_count; i++) {
        FoundWord r;
        if (findWordInGrid(grid, lines, cols, words[i], &r)) {
            results[foundCount++] = r;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0 || IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Failed to initialize SDL or SDL_image: %s\n", SDL_GetError());
        return 1;
    }

    int cell_size = 50;
    int width = cols * cell_size;
    int height = lines * cell_size;

    // Partie 1 : Création de la grille avec les mots barrés (grid_solv.png)
    SDL_Window *window = SDL_CreateWindow("Word Search", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_HIDDEN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *letters[26];
    for (char c = 'A'; c <= 'Z'; c++) {
        char filename[16];
        snprintf(filename, sizeof(filename), "assets/%c.png", c);
        letters[c - 'A'] = IMG_Load(filename);
        if (!letters[c - 'A']) {
            printf("Failed to load %s: %s\n", filename, IMG_GetError());
            return 1;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int l = 0; l < lines; l++) {
        for (int c = 0; c < cols; c++) {
            char letter = grid[l][c];
            if (letter >= 'A' && letter <= 'Z') {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, letters[letter - 'A']);
                SDL_Rect zone = {c * cell_size, l * cell_size, cell_size, cell_size};
                SDL_RenderCopy(renderer, tex, NULL, &zone);
                SDL_DestroyTexture(tex);
            }
        }
    }

    for (int j = 0; j < foundCount; j++) {
        drawThickLine(renderer, results[j].start_line, results[j].start_col,
                      results[j].end_line, results[j].end_col, cell_size, j);
    }

    saveImage(renderer, width, height, "grid_solv.png");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Partie 2 : Création de l'image avec la liste des mots trouvés (list_solv.png)
    SDL_Window *list_window = SDL_CreateWindow("Word List", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_HIDDEN);
    SDL_Renderer *list_renderer = SDL_CreateRenderer(list_window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(list_renderer, 255, 255, 255, 255); // Fond blanc
    SDL_RenderClear(list_renderer);

    int y_offset = 10;
    int line_height = 50;
    SDL_Color black = {0, 0, 0, 255};

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 24); // Chargez une police TrueType
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    for (int i = 0; i < foundCount; i++) {
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, results[i].found_word, black);
        if (!text_surface) {
            printf("Failed to create text surface: %s\n", TTF_GetError());
            continue;
        }
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(list_renderer, text_surface);
        SDL_Rect dest_rect = {10, y_offset, text_surface->w, text_surface->h};
        SDL_RenderCopy(list_renderer, text_texture, NULL, &dest_rect);

        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);

        y_offset += line_height;
    }

    saveImage(list_renderer, width, height, "list_solv.png");

    SDL_DestroyRenderer(list_renderer);
    SDL_DestroyWindow(list_window);

    // Nettoyage
    for (int i = 0; i < 26; i++) SDL_FreeSurface(letters[i]);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();

    return 0;
}
