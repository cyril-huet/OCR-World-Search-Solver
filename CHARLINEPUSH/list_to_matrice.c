#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

char **read_text_to_matrix(const char *file_path, int *rows, int *cols) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    // Variables pour déterminer la taille de la matrice
    int max_row = -1, max_col = -1;

    // Lecture pour identifier la taille de la grille
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        int row, col;
        sscanf(line, "letter_grid_line%d_col%d.pgm:", &row, &col);
        if (row > max_row) max_row = row;
        if (col > max_col) max_col = col;
    }

    // Ajuste les tailles pour commencer à 0
    *rows = max_row + 1;
    *cols = max_col + 1;

    // Allocation de la matrice
    char **matrix = (char **)malloc(*rows * sizeof(char *));
    for (int i = 0; i < *rows; i++) {
        matrix[i] = (char *)malloc(*cols * sizeof(char));
        memset(matrix[i], ' ', *cols); // Initialise avec des espaces
    }

    // Revenir au début du fichier pour remplir la matrice
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        int row, col;
        char value;
        sscanf(line, "letter_grid_line%d_col%d.pgm: %c", &row, &col, &value);
        matrix[row][col] = value;
    }

    fclose(file);
    return matrix;
}

// Fonction pour afficher la matrice
void print_matrix(char **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour libérer la mémoire de la matrice
void free_matrix(char **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    const char *file_path = "grille.txt";
    int rows, cols;

    char **matrix = read_text_to_matrix(file_path, &rows, &cols);
    if (matrix != NULL) {
        printf("Matrice lue :\n");
        print_matrix(matrix, rows, cols);
        free_matrix(matrix, rows);
    }

    return 0;
}

