#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000 // Nombre maximum de lignes à traiter
#define MAX_LENGTH 256 // Longueur maximale d'une ligne

void trier_fichiers(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char *lignes[MAX_LINES];
    int nombre_lignes = 0;

    // Lecture des lignes dans le fichier
    char buffer[MAX_LENGTH];
    while (fgets(buffer, sizeof(buffer), fichier) != NULL) {
        if (nombre_lignes >= MAX_LINES) {
            fprintf(stderr, "Erreur : Trop de lignes dans le fichier.\n");
            exit(EXIT_FAILURE);
        }

        // Supprimer le caractère de nouvelle ligne '\n'
        buffer[strcspn(buffer, "\n")] = '\0';

        lignes[nombre_lignes] = strdup(buffer); // Allouer une copie
        if (lignes[nombre_lignes] == NULL) {
            perror("Erreur d'allocation mémoire");
            exit(EXIT_FAILURE);
        }

        nombre_lignes++;
    }
    fclose(fichier);

    // Tri des lignes
    for (int i = 0; i < nombre_lignes - 1; i++) {
        for (int j = i + 1; j < nombre_lignes; j++) {
            if (strcmp(lignes[i], lignes[j]) > 0) {
                char *temp = lignes[i];
                lignes[i] = lignes[j];
                lignes[j] = temp;
            }
        }
    }

    // Écriture des lignes triées dans le fichier
    fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier pour écriture");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nombre_lignes; i++) {
        fprintf(fichier, "%s\n", lignes[i]);
        free(lignes[i]); // Libérer la mémoire allouée
    }

    fclose(fichier);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <nom_du_fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    trier_fichiers(argv[1]);
    return EXIT_SUCCESS;
}

