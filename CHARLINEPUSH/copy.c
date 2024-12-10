#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void copy_and_rename_first_file(const char *source_dir, const char *new_name) {
    struct dirent *entry;
    DIR *dp = opendir(source_dir);

    if (dp == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le dossier %s.\n", source_dir);
        exit(EXIT_FAILURE);
    }

    char source_file_path[512];
    int file_found = 0;

    // Trouver le premier fichier dans le dossier
    while ((entry = readdir(dp)) != NULL) {
        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier source
        snprintf(source_file_path, sizeof(source_file_path), "%s/%s", source_dir, entry->d_name);

        file_found = 1;
        break;
    }

    closedir(dp);

    if (!file_found) {
        fprintf(stderr, "Aucun fichier trouvé dans le dossier %s.\n", source_dir);
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier source en lecture
    FILE *source = fopen(source_file_path, "rb");
    if (!source) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier source %s.\n", source_file_path);
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier destination en écriture avec le nouveau nom
    FILE *destination = fopen(new_name, "wb");
    if (!destination) {
        fprintf(stderr, "Erreur lors de la création du fichier destination %s.\n", new_name);
        fclose(source);
        exit(EXIT_FAILURE);
    }

    // Copier le contenu du fichier source dans le fichier destination
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    // Fermer les fichiers
    fclose(source);
    fclose(destination);

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source_dir = argv[1];
    const char *new_name = "output.pgm";

    copy_and_rename_first_file(source_dir, new_name);

    return EXIT_SUCCESS;
}

