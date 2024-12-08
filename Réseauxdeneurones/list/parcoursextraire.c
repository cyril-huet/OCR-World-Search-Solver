#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// Fonction pour exécuter une commande avec un fichier donné
void executer_commande(const char *chemin_fichier) {
    char commande[512];
    snprintf(commande, sizeof(commande), "./extraire %s", chemin_fichier);
    printf("Exécution de la commande : %s\n", commande);

    int resultat = system(commande);
    if (resultat != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de la commande pour %s\n", chemin_fichier);
    }
}

int main() {
    const char *dossier = "mot_png";
    DIR *dir = opendir(dossier);

    if (!dir) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le dossier %s\n", dossier);
        return 1;
    }

    struct dirent *entree;

    while ((entree = readdir(dir)) != NULL) {
        // Ignorer les fichiers spéciaux "." et ".."
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier
        char chemin_fichier[512];
        snprintf(chemin_fichier, sizeof(chemin_fichier), "%s/%s", dossier, entree->d_name);

        // Vérifier si c'est un fichier régulier
        FILE *fichier = fopen(chemin_fichier, "r");
        if (fichier) {
            fclose(fichier);
            printf("Traitement du fichier : %s\n", chemin_fichier);
            executer_commande(chemin_fichier);
        }
    }

    closedir(dir);
    return 0;
}

