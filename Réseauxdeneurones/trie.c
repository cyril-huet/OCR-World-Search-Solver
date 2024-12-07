#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_FILES 1000

typedef struct {
    char nom[MAX_LINE_LENGTH];
    int num1;
    int num2;
} Fichier;

// Fonction de comparaison pour le tri
int comparer(const void *a, const void *b) {
    const Fichier *fa = (const Fichier *)a;
    const Fichier *fb = (const Fichier *)b;

    // Comparaison sur num1
    if (fa->num1 != fb->num1) {
        return fa->num1 - fb->num1;
    }
    // Comparaison sur num2 si num1 est ?gal
    return fa->num2 - fb->num2;
}

void trierFichiers(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    Fichier fichiers[MAX_FILES];
    size_t nombreFichiers = 0;

    // Lecture des noms de fichiers dans un tableau
    char ligne[MAX_LINE_LENGTH];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0'; // Supprime le '\n'
        int num1, num2;

        // Extraction de num1 et num2
        if (sscanf(ligne, "letter_grid_line%d_col%d.pgm", &num1, &num2) == 2) {
            strncpy(fichiers[nombreFichiers].nom, ligne, MAX_LINE_LENGTH - 1);
            fichiers[nombreFichiers].nom[MAX_LINE_LENGTH - 1] = '\0'; // S?curit?
            fichiers[nombreFichiers].num1 = num1;
            fichiers[nombreFichiers].num2 = num2;
            nombreFichiers++;
            if (nombreFichiers >= MAX_FILES) {
                fprintf(stderr, "Limite maximale de fichiers atteinte (%d)\n", MAX_FILES);
                break;
            }
        } else {
            fprintf(stderr, "Format incorrect pour la ligne : %s\n", ligne);
        }
    }

    fclose(fichier);

    // Tri des fichiers
    qsort(fichiers, nombreFichiers, sizeof(Fichier), comparer);

    // Affichage des fichiers tri?s
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <nom_du_fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    trierFichiers(argv[1]);
    return EXIT_SUCCESS;
}

