#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

// Fonction pour calculer la taille maximale de la matrice
void get_matrix_size(const char *filename, int *max_row, int *max_col) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    *max_row = 0;
    *max_col = 0;
    char line[MAX_LINE_LENGTH];
    int row, col;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        sscanf(line, "letter_grid_line%d_col%d.pgm:", &row, &col);
        if (row > *max_row) *max_row = row;
        if (col > *max_col) *max_col = col;
    }

    fclose(file);
    (*max_row)++;
    (*max_col)++;
}

// Fonction pour lire les données et remplir la matrice
char** create_letter_matrix(const char *filename, int max_row, int max_col) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Allouer de la mémoire pour la matrice
    char **matrix = malloc(max_row * sizeof(char *));
    for (int i = 0; i < max_row; i++) {
        matrix[i] = malloc(max_col * sizeof(char));
        memset(matrix[i], ' ', max_col); // Remplir de caractères vides
    }

    char line[MAX_LINE_LENGTH];
    int row, col;
    char letter;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        sscanf(line, "letter_grid_line%d_col%d.pgm: %c", &row, &col, &letter);
        matrix[row][col] = letter;
    }

    fclose(file);
    return matrix;
}

// Fonction pour écrire la matrice dans un fichier
void write_matrix_to_file(char **matrix, int max_row, int max_col, const char *output_filename) {
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < max_row; i++) {
        for (int j = 0; j < max_col; j++) {
            fprintf(output_file, "%c", matrix[i][j]);
     
        }
         fprintf(output_file, "\n");
    }

    fclose(output_file);
}

// Fonction pour libérer la mémoire de la matrice
void free_matrix(char **matrix, int max_row) {
    for (int i = 0; i < max_row; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    const char *input_filename = "fichier_grid.txt";  // Nom du fichier d'entrée
    const char *output_filename = "output_matrix.txt"; // Nom du fichier de sortie
    int max_row, max_col;

    // Calculer les dimensions de la matrice
    get_matrix_size(input_filename, &max_row, &max_col);

    // Créer et remplir la matrice
    char **matrix = create_letter_matrix(input_filename, max_row, max_col);

    // Écrire la matrice dans un fichier
    write_matrix_to_file(matrix, max_row, max_col, output_filename);

    // Libérer la mémoire
    free_matrix(matrix, max_row);

    return 0;
}

