#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_FILES 1000

typedef struct {
    char lettre;
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
    // Comparaison sur num2 si num1 est égal
    return fa->num2 - fb->num2;
}

void genererMatrice(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    Fichier fichiers[MAX_FILES];
    size_t nombreFichiers = 0;
    int nombreColonnes = 0;

    // Lecture du fichier
    char ligne[MAX_LINE_LENGTH];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0'; // Supprime le '\n'

        // Si c'est la première ligne, on récupère le nombre de colonnes
        if (nombreFichiers == 0) {
            if (sscanf(ligne, "%d", &nombreColonnes) != 1) {
                fprintf(stderr, "Erreur : Nombre de colonnes invalide.\n");
                fclose(fichier);
                return;
            }
            continue;
        }

        // Extraction des informations (num1, num2, lettre)
        int num1, num2;
        char lettre;
        if (sscanf(ligne, "letter_grid_line%d_col%d.pgm: %c", &num1, &num2, &lettre) == 3) {
            fichiers[nombreFichiers].num1 = num1;
            fichiers[nombreFichiers].num2 = num2;
            fichiers[nombreFichiers].lettre = lettre;
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

    // Génération de la matrice
    printf("Matrice :\n");
    for (size_t i = 0; i < nombreFichiers; i++) {
        printf("%c", fichiers[i].lettre);
        if ((i + 1) % nombreColonnes == 0) {
            printf("\n"); // Nouvelle ligne après chaque ligne complète
        }
    }
    if (nombreFichiers % nombreColonnes != 0) {
        printf("\n"); // Ajout d'une nouvelle ligne si nécessaire
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <nom_du_fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    genererMatrice(argv[1]);
    return EXIT_SUCCESS;
}

