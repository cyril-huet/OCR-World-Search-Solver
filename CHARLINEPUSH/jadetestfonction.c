#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h> // Pour unlink

void create_directory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0755);
    }
}

void filter_images_by_size(const char *input_dir, const char *output_dir, int size_threshold) {
    DIR *dir = opendir(input_dir);
    if (!dir) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le dossier %s\n", input_dir);
        return;
    }

    struct dirent *entry;

    // Créer le dossier de sortie
    create_directory(output_dir);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".pgm")) {
            char filepath[1024], outputpath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", input_dir, entry->d_name);
            snprintf(outputpath, sizeof(outputpath), "%s/%s", output_dir, entry->d_name);

            // Ouvrir l'image PGM et lire ses dimensions
            FILE *fp = fopen(filepath, "rb");
            if (!fp) {
                fprintf(stderr, "Erreur d'ouverture du fichier %s\n", filepath);
                continue;
            }

            char format[3];
            int width, height, max_val;

            fscanf(fp, "%2s", format);
            if (strcmp(format, "P5") != 0) {
                fclose(fp);
                continue; // Ignorer les fichiers non-PGM
            }

            fscanf(fp, "%d %d %d", &width, &height, &max_val);
            fclose(fp);

            // Vérifier la taille de l'image
            if (width >= size_threshold && height >= size_threshold) {
                // Copier l'image dans le dossier de sortie
                FILE *out_fp = fopen(outputpath, "wb");
                if (!out_fp) {
                    fprintf(stderr, "Erreur lors de la création de %s\n", outputpath);
                    continue;
                }

                fp = fopen(filepath, "rb");
                char buffer[1024];
                size_t bytes;

                while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                    fwrite(buffer, 1, bytes, out_fp);
                }

                fclose(fp);
                fclose(out_fp);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Utilisation : %s <dossier_images> <dossier_sortie> <taille_minimale>\n", argv[0]);
        return 1;
    }

    const char *input_dir = argv[1];
    const char *output_dir = argv[2];
    int size_threshold = atoi(argv[3]);

    if (size_threshold <= 0) {
        fprintf(stderr, "La taille minimale doit être un entier strictement positif.\n");
        return 1;
    }

    filter_images_by_size(input_dir, output_dir, size_threshold);


    return 0;
}
