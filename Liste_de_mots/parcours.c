
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



// Fonction pour vérifier l'extension d'un fichier
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, extension) == 0;
}

// Fonction pour convertir un fichier PNG en PGM
void convertir_png_en_pgm(const char *chemin_fichier, const char *dossier_sortie) {
    char commande[512];
    char nom_sortie[256];

    // Construire le nom du fichier PGM de sortie
    const char *nom_fichier = strrchr(chemin_fichier, '/');
    if (!nom_fichier) {
        nom_fichier = chemin_fichier;
    } else {
        nom_fichier++;
    }

    // Remplacer l'extension .png par .pgm
    snprintf(nom_sortie, sizeof(nom_sortie), "%s/%.*s.pgm", dossier_sortie, (int)(strrchr(nom_fichier, '.') - nom_fichier), nom_fichier);

    // Construire la commande shell
    snprintf(commande, sizeof(commande), "./png_to_pgm %s %s", chemin_fichier, nom_sortie);
    printf("Exécution de la commande : %s\n", commande);

    // Exécuter la commande
    int resultat = system(commande);
    if (resultat != 0) {
        fprintf(stderr, "Erreur lors de la conversion de %s\n", chemin_fichier);
    }
}

int main() {
    const char *dossier_entree = "mot_png";
    const char *dossier_sortie = "dossier_pgm";

    // Créer le dossier de sortie s'il n'existe pas
    struct stat st = {0};
    if (stat(dossier_sortie, &st) == -1) {
        if (mkdir(dossier_sortie, 0700) != 0) {
            fprintf(stderr, "Erreur lors de la création du dossier %s\n", dossier_sortie);
            return 1;
        }
    }

    DIR *dossier = opendir(dossier_entree);
    if (!dossier) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le dossier %s\n", dossier_entree);
        return 1;
    }

    struct dirent *entree;
    while ((entree = readdir(dossier)) != NULL) {
        // Ignorer les fichiers spéciaux "." et ".."
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier
        char chemin_fichier[256];
        snprintf(chemin_fichier, sizeof(chemin_fichier), "%s/%s", dossier_entree, entree->d_name);

        // Vérifier si c'est un fichier PNG
        if (has_extension(entree->d_name, ".png")) {
            printf("Conversion du fichier : %s\n", chemin_fichier);
            convertir_png_en_pgm(chemin_fichier, dossier_sortie);
        }
    }

    closedir(dossier);
    return 0;
}

