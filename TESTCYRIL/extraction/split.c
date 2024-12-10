#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

        // Analyse adaptée pour les fichiers contenant "letter_grid_line" ou "letter_line"
        if (sscanf(buffer, "letter_grid_line%d_col%*d.pgm : %c", &line_number, &letter) == 2 ||
            sscanf(buffer, "letter_line%d_col%*d.pgm : %c", &line_number, &letter) == 2) {
            
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

        // Analyse pour les fichiers contenant "letter_grid"
        if (sscanf(buffer, "letter_grid_line%d_col%*d.pgm : %c", &line_number, &letter) == 2) {
            
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
        // Vérifie si la ligne contient "letter_grid" ou "letter_line"
        if (strstr(buffer, "letter_grid") != NULL) {
            fputs(buffer, output_grid);
        }
        if (strstr(buffer, "letter_line") != NULL) {
            fputs(buffer, output_line);
        }
    }

    fclose(input);
    fclose(output_grid);
    fclose(output_line);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <fichier entrée>\n", argv[0]);
        return 1;
    }

    split_file_by_prefix(argv[1], "fichier_grid.txt", "fichier_line.txt");
    process_letter_lines("fichier_line.txt", "list.txt");
    process_letter_grid("fichier_grid.txt", "grid.txt");

    return 0;
}

