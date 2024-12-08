#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

// Fonction pour exécuter une commande shell
void execute_command(const char *command) {
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de : %s\n", command);
        exit(EXIT_FAILURE);
    }
}

// Fonction pour convertir les fichiers PNG en PGM et supprimer les PNG
void convert_and_delete_png(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        exit(EXIT_FAILURE);
    }

    char png_file[512];
    char pgm_file[512];
    char command[1024];

    while ((entry = readdir(dp)) != NULL) {
        // Vérifier si le fichier correspond au motif "mot*.png"
        if (strstr(entry->d_name, "mot") == entry->d_name && strstr(entry->d_name, ".png")) {
            snprintf(png_file, sizeof(png_file), "%s/%s", directory, entry->d_name);
            snprintf(pgm_file, sizeof(pgm_file), "%s/%s", directory, entry->d_name);
            strcpy(strrchr(pgm_file, '.'), ".pgm"); // Remplacer l'extension .png par .pgm

            // Convertir PNG en PGM
            snprintf(command, sizeof(command), "convert %s %s", png_file, pgm_file);
            execute_command(command);

            // Supprimer le fichier PNG d'origine
            if (unlink(png_file) != 0) {
                perror("Erreur lors de la suppression du fichier PNG");
            } else {
                printf("Fichier supprimé : %s\n", png_file);
            }
        }
    }

    closedir(dp);
}

int main() {
    const char *directory = "."; // Dossier courant ou spécifier un autre chemin

    // Appeler la fonction pour parcourir le dossier et traiter les fichiers
    convert_and_delete_png(directory);

    return EXIT_SUCCESS;
}
