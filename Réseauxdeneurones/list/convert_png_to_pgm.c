#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define COMMAND_BUFFER_SIZE 512
#define FILE_PATH_BUFFER_SIZE 512

void convert_png_to_pgm_and_delete(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("Erreur: impossible d'ouvrir le dossier");
        exit(EXIT_FAILURE);
    }

    char command[COMMAND_BUFFER_SIZE];
    char pgm_file[FILE_PATH_BUFFER_SIZE];

    while ((entry = readdir(dp)) != NULL) {
        // Ignorer les fichiers spéciaux "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Vérifier si le fichier a l'extension ".png"
        const char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".png") == 0) {
            // Construire le chemin complet du fichier PNG
            char png_file[FILE_PATH_BUFFER_SIZE];
            if ((size_t)snprintf(png_file, sizeof(png_file), "%s/%s", directory, entry->d_name) >= sizeof(png_file)) {
                fprintf(stderr, "Erreur: chemin de fichier PNG trop long.\n");
                continue;
            }

            // Construire le nom du fichier PGM en remplaçant l'extension
            snprintf(pgm_file, sizeof(pgm_file), "%s/%.*s.pgm", directory,
                     (int)(strlen(entry->d_name) - strlen(ext)), entry->d_name);

            // Construire la commande pour convertir PNG en PGM
            if ((size_t)snprintf(command, sizeof(command), "convert \"%s\" \"%s\"", png_file, pgm_file) >= sizeof(command)) {
                fprintf(stderr, "Erreur: commande de conversion trop longue.\n");
                continue;
            }

            // Exécuter la commande
            int ret = system(command);
            if (ret != 0) {
                fprintf(stderr, "Erreur: la conversion de %s vers %s a échoué avec le code %d.\n", png_file, pgm_file, ret);
                continue;
            }

            // Supprimer le fichier PNG original
            if (unlink(png_file) != 0) {
                perror("Erreur: échec de la suppression du fichier PNG");
            } else {
                printf("Fichier PNG supprimé : %s\n", png_file);
            }
        }
    }

    closedir(dp);
}

int main() {
    const char *directory = "mot_png"; // Dossier contenant les fichiers PNG

    // Vérifier si le dossier existe
    struct stat st = {0};
    if (stat(directory, &st) == -1) {
        fprintf(stderr, "Erreur: le dossier %s n'existe pas.\n", directory);
        return EXIT_FAILURE;
    }

    // Convertir les fichiers PNG en PGM et supprimer les PNG
    convert_png_to_pgm_and_delete(directory);

    return EXIT_SUCCESS;
}

