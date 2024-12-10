#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_LENGTH 256

void process_letter_lines(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "r");
    if (!input) {
        perror("Erreur d'ouverture du fichier d'entrée");
        return;
    }

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        fclose(input);
        return;
    }

    char buffer[1024];
    int current_line = -1;

    while (fgets(buffer, sizeof(buffer), input)) {
        int line_number;
        char letter;

        if (sscanf(buffer, "letter_line%d_col%*d.pgm: %c", &line_number, &letter) == 2 ||
            sscanf(buffer, "mot_%d_lettre_%*d.pgm: %c", &line_number, &letter) == 2) {
            if (line_number != current_line) {
                if (current_line != -1) {
                    fputc('\n', output);
                }
                current_line = line_number;
            }
            fputc(letter, output);
        } else {
            printf("Ligne non reconnue : %s\n", buffer);
        }
    }

    fputc('\n', output);

    fclose(input);
    fclose(output);
}

void process_letter_grid(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "r");
    if (!input) {
        perror("Erreur d'ouverture du fichier d'entrée");
        return;
    }

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        fclose(input);
        return;
    }

    char buffer[1024];
    int current_line = -1;

    while (fgets(buffer, sizeof(buffer), input)) {
        int line_number;
        char letter;

        if (sscanf(buffer, "letter_grid_line%d_col%*d.pgm: %c", &line_number, &letter) == 2 ||
            sscanf(buffer, "mot_%d_lettre_%*d.pgm: %c", &line_number, &letter) == 2) {
            if (line_number != current_line) {
                if (current_line != -1) {
                    fputc('\n', output);
                }
                current_line = line_number;
            }
            fputc(letter, output);
        }
    }

    fputc('\n', output);

    fclose(input);
    fclose(output);
}

void split_file_by_prefix(const char *input_file, const char *output_file_grid, const char *output_file_line) {
    FILE *input = fopen(input_file, "r");
    if (!input) {
        perror("Erreur d'ouverture du fichier d'entrée");
        return;
    }

    FILE *output_grid = fopen(output_file_grid, "w");
    if (!output_grid) {
        perror("Erreur d'ouverture du fichier de sortie pour letter_grid");
        fclose(input);
        return;
    }

    FILE *output_line = fopen(output_file_line, "w");
    if (!output_line) {
        perror("Erreur d'ouverture du fichier de sortie pour letter_line");
        fclose(input);
        fclose(output_grid);
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), input)) {
        if (strncmp(buffer, "letter_grid", 11) == 0) {
            fputs(buffer, output_grid);
        } else if (strncmp(buffer, "letter_line", 11) == 0 || strncmp(buffer, "mot_", 4) == 0) {
            fputs(buffer, output_line);
        }
    }

    fclose(input);
    fclose(output_grid);
    fclose(output_line);
}

void extraire_indices(const char *ligne, int *line, int *col) {
    const char *line_str = strstr(ligne, "line");
    const char *col_str = strstr(ligne, "col");

    if (line_str && col_str) {
        sscanf(line_str, "line%d", line);
        sscanf(col_str, "col%d", col);
    } else {
        *line = -1;
        *col = -1;
    }
}





int comparer_lignes(const void *a, const void *b) {
    const char *ligne_a = *(const char **)a;
    const char *ligne_b = *(const char **)b;

    int mot_a, lettre_a, mot_b, lettre_b;

    // Extraire les indices des mots et des lettres des deux lignes
    if (sscanf(ligne_a, "mot_%d_lettre_%d.pgm", &mot_a, &lettre_a) != 2) {
        mot_a = 0; // Valeurs par défaut si l'extraction échoue
        lettre_a = 0;
    }
    if (sscanf(ligne_b, "mot_%d_lettre_%d.pgm", &mot_b, &lettre_b) != 2) {
        mot_b = 0;
        lettre_b = 0;
    }

    // Comparer d'abord les numéros de mot, puis les numéros de lettre
    if (mot_a != mot_b) {
        return mot_a - mot_b;
    }
    return lettre_a - lettre_b;
}


void trier_fichiers(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char *lignes[MAX_LINES];
    int nombre_lignes = 0;
    char buffer[MAX_LENGTH];

    while (fgets(buffer, sizeof(buffer), fichier) != NULL) {
        if (nombre_lignes >= MAX_LINES) {
            fprintf(stderr, "Erreur : Trop de lignes dans le fichier.\n");
            exit(EXIT_FAILURE);
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        lignes[nombre_lignes] = strdup(buffer);
        if (lignes[nombre_lignes] == NULL) {
            perror("Erreur d'allocation mémoire");
            exit(EXIT_FAILURE);
        }
        nombre_lignes++;
    }
    fclose(fichier);

    qsort(lignes, nombre_lignes, sizeof(char *), comparer_lignes);

    fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier pour écriture");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nombre_lignes; i++) {
        fprintf(fichier, "%s\n", lignes[i]);
        free(lignes[i]);
    }

    fclose(fichier);
}

int main() {
    split_file_by_prefix("matrice.txt", "fichier_grid.txt", "fichier_line.txt");
    trier_fichiers("fichier_line.txt");
    process_letter_lines("fichier_line.txt", "list.txt");
    trier_fichiers("fichier_grid.txt");
    process_letter_grid("fichier_grid.txt", "grid.txt");
    return 0;
}

