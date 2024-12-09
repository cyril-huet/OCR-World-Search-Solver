#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure pour stocker une lettre avec ses coordonnées
typedef struct {
    int line;
    int col;
    char letter;
} Letter;

// Fonction pour comparer deux lettres selon leurs coordonnées
int compareLetters(const void *a, const void *b) {
    Letter *letterA = (Letter *)a;
    Letter *letterB = (Letter *)b;

    // Comparer par ligne (line), puis par colonne (col)
    if (letterA->line != letterB->line) {
        return letterA->line - letterB->line;
    }
    return letterA->col - letterB->col;
}

// Fonction principale pour trier, créer une matrice séquentielle et écrire dans le fichier de sortie
void processAndSortLetters(const char *inputFile, const char *outputFile, const char *configFile) {
    FILE *config = fopen(configFile, "r");
    if (!config) {
        perror("Erreur lors de l'ouverture du fichier de configuration");
        exit(EXIT_FAILURE);
    }

    int cols;
    if (fscanf(config, "%d", &cols) != 1 || cols <= 0) {
        fprintf(stderr, "Erreur : Le fichier de configuration doit contenir un nombre valide de colonnes.\n");
        fclose(config);
        exit(EXIT_FAILURE);
    }
    fclose(config);

    FILE *input = fopen(inputFile, "r");
    if (!input) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        exit(EXIT_FAILURE);
    }

    Letter letters[1000];
    int count = 0;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), input)) {
        int line, col;
        char letter;

        // Extraire les informations si la ligne contient une majuscule
        if (sscanf(buffer, "letter_grid_line%d_col%d.pgm: %c", &line, &col, &letter) == 3) {
            if (isupper(letter)) {
                letters[count].line = line;
                letters[count].col = col;
                letters[count].letter = letter;
                count++;
            }
        }
    }
    fclose(input);

    // Trier les lettres par ligne et par colonne
    qsort(letters, count, sizeof(Letter), compareLetters);

    // Calculer le nombre de lignes nécessaires
    int rows = (count + cols - 1) / cols;

    // Créer une matrice vide remplie d'espaces
    char **matrix = malloc(rows * sizeof(char *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(char));
        memset(matrix[i], ' ', cols);
    }

    // Remplir la matrice avec les lettres triées
    for (int i = 0; i < count; i++) {
        int row = i / cols;
        int col = i % cols;
        matrix[row][col] = letters[i].letter;
    }

    // Écrire la matrice dans le fichier de sortie
    FILE *output = fopen(outputFile, "w");
    if (!output) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        fwrite(matrix[i], sizeof(char), cols, output); // Écrire chaque ligne
        fputc('\n', output); // Ajouter un saut de ligne après chaque ligne
    }

    fclose(output);

    // Libérer la mémoire allouée pour la matrice
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    processAndSortLetters("matrice.txt", "finale.txt", "letters.txt");
    return 0;
}
