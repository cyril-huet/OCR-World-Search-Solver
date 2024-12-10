#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

// Fonction pour lire les dimensions d'une image PGM
int lire_dimensions_pgm(const char *chemin, int *largeur, int *hauteur) {
    FILE *fichier = fopen(chemin, "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    char buffer[256];

    // Lire le type de fichier (P2 ou P5)
    if (!fgets(buffer, sizeof(buffer), fichier)) {
        fclose(fichier);
        return -1;
    }

    if (strncmp(buffer, "P2", 2) != 0 && strncmp(buffer, "P5", 2) != 0) {
        fclose(fichier);
        return -1;
    }

    // Lire les commentaires (si présents)
    do {
        if (!fgets(buffer, sizeof(buffer), fichier)) {
            fclose(fichier);
            return -1;
        }
    } while (buffer[0] == '#');

    // Lire les dimensions
    if (sscanf(buffer, "%d %d", largeur, hauteur) != 2) {
        fclose(fichier);
        return -1;
    }

    fclose(fichier);
    return 0;
}

// Fonction principale pour filtrer les images
void filtrer_images_pgm(const char *dossier_source, const char *dossier_destination) {
    struct dirent *entree;
    DIR *rep = opendir(dossier_source);

    if (!rep) {
        perror("Erreur lors de l'ouverture du dossier source");
        return;
    }

    // Créer le dossier de destination s'il n'existe pas
    mkdir(dossier_destination, 0755);

    while ((entree = readdir(rep)) != NULL) {
        // Ignorer "." et ".."
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0) {
            continue;
        }

        char chemin_source[512];
        snprintf(chemin_source, sizeof(chemin_source), "%s/%s", dossier_source, entree->d_name);

        struct stat st;
        if (stat(chemin_source, &st) == -1) {
            perror("Erreur lors de la lecture des informations du fichier");
            continue;
        }

        // Si c'est un fichier
        if (S_ISREG(st.st_mode)) {
            int largeur = 0, hauteur = 0;
            if (lire_dimensions_pgm(chemin_source, &largeur, &hauteur) == 0) {
                if (largeur > 31 && hauteur > 31) {
                    char chemin_destination[512];
                    snprintf(chemin_destination, sizeof(chemin_destination), "%s/%s", dossier_destination, entree->d_name);

                    // Copier le fichier
                    FILE *src = fopen(chemin_source, "rb");
                    FILE *dest = fopen(chemin_destination, "wb");

                    if (src && dest) {
                        char buffer[4096];
                        size_t bytes;
                        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                            fwrite(buffer, 1, bytes, dest);
                        }
                    }

                    if (src) fclose(src);
                    if (dest) fclose(dest);
                }
            }
        }
    }

    closedir(rep);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dossier_source> <dossier_destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    filtrer_images_pgm(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
