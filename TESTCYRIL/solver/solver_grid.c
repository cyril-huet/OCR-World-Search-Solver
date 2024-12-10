#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    if (!f) return 0;
    *lines = 0;
    int max_width = 0;

    while (fgets(grid[*lines], MAX_COLUMNS, f) && *lines < MAX_LINES) {
        grid[*lines][strcspn(grid[*lines], "\n")] = '\0';  // Retirer le caractère de nouvelle ligne
        int line_length = (int)strlen(grid[*lines]);
        if (line_length > max_width) max_width = line_length;
        (*lines)++;
    }

    *cols = max_width;
    for (int i = 0; i < *lines; i++) {
        int line_length = (int)strlen(grid[i]);
        if (line_length < max_width) {
            for (int j = line_length; j < max_width; j++) {
                grid[i][j] = ' '; // Compléter avec des espaces
            }
            grid[i][max_width] = '\0';
        }
    }

    fclose(f);
    return 1;
}

int loadWordsFromFile(const char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH], int *word_count) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    *word_count = 0;
    while (fgets(words[*word_count], MAX_WORD_LENGTH, f) && *word_count < MAX_WORDS) {
        words[*word_count][strcspn(words[*word_count], "\n")] = '\0';
        if (strlen(words[*word_count]) > 0) {
            for (int i = 0; words[*word_count][i]; i++) {
                words[*word_count][i] = (char)toupper((unsigned char)words[*word_count][i]);
            }
            (*word_count)++;
        }
    }
    fclose(f);
    return 1;
}

void drawGrid(SDL_Renderer *renderer, char grid[MAX_LINES][MAX_COLUMNS], int lines, int cols, int cell_size, SDL_Surface *letters[26]) {
    SDL_Surface *white_square = SDL_CreateRGBSurfaceWithFormat(0, cell_size, cell_size, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(white_square, NULL, SDL_MapRGB(white_square->format, 255, 255, 255));
    SDL_Texture *white_texture = SDL_CreateTextureFromSurface(renderer, white_square);

    for (int l = 0; l < lines; l++) {
        for (int c = 0; c < cols; c++) {
            char letter = grid[l][c];
            SDL_Texture *tex = NULL;

            if (letter >= 'A' && letter <= 'Z') {
                tex = SDL_CreateTextureFromSurface(renderer, letters[letter - 'A']);
            } else {
                tex = white_texture;
            }

            SDL_Rect zone = {c * cell_size, l * cell_size, cell_size, cell_size};
            SDL_RenderCopy(renderer, tex, NULL, &zone);
            if (tex != white_texture) {
                SDL_DestroyTexture(tex);
            }
        }
    }

    SDL_FreeSurface(white_square);
    SDL_DestroyTexture(white_texture);
}

void drawThickLine(SDL_Renderer *renderer, int start_line, int start_col, int end_line, int end_col, int cell_size, int word_index) {
    int colors[][3] = {
    {255, 0, 0},    // Rouge
    {0, 255, 0},    // Vert
    {0, 0, 255},    // Bleu
    {255, 255, 0},  // Jaune
    {0, 255, 255},  // Cyan
    {255, 0, 255},  // Magenta
    {128, 0, 0},    // Rouge foncé
    {0, 128, 0},    // Vert foncé
    {0, 0, 128},    // Bleu foncé
    {128, 128, 0},  // Olive
    {128, 0, 128},  // Violet
    {0, 128, 128},  // Bleu sarcelle
    {192, 192, 192}, // Gris clair
    {128, 128, 128}, // Gris moyen
    {64, 64, 64},    // Gris foncé
    {255, 165, 0},   // Orange
    {255, 192, 203}, // Rose clair
    {75, 0, 130},    // Indigo
    {60, 179, 113},  // Vert menthe
    {176, 224, 230}, // Bleu poudre
    {139, 69, 19}    // Marron
    };
    int num_colors = sizeof(colors) / sizeof(colors[0]);

    int r = colors[word_index % num_colors][0];
    int g = colors[word_index % num_colors][1];
    int b = colors[word_index % num_colors][2];

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int x1 = start_col * cell_size + cell_size / 2;
    int y1 = start_line * cell_size + cell_size / 2;
    int x2 = end_col * cell_size + cell_size / 2;
    int y2 = end_line * cell_size + cell_size / 2;
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
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

int main() {
    char grid[MAX_LINES][MAX_COLUMNS];
    int lines, cols;

    if (!loadGridFromFile("../extraction/grid.txt", grid, &lines, &cols)) {
        printf("Failed to load the grid from file.\n");
        return 1;
    }

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int word_count;
    if (!loadWordsFromFile("../extraction/list.txt", words, &word_count)) {
        printf("Failed to load words from file.\n");
        return 1;
    }

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

    drawGrid(renderer, grid, lines, cols, cell_size, letters);

    for (int j = 0; j < foundCount; j++) {
        drawThickLine(renderer, results[j].start_line, results[j].start_col,
                      results[j].end_line, results[j].end_col, cell_size, j);
    }

    saveImage(renderer, width, height, "grid_solv.png");

    for (int i = 0; i < 26; i++) {
        SDL_FreeSurface(letters[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}


