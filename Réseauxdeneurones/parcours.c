#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define THRESHOLD 128 // Seuil pour la binarisation
#define LINE_TOLERANCE 10 // Tolérance pour regrouper les lettres dans une ligne

typedef struct {
    int min_x, min_y, max_x, max_y;
} BoundingBox;

typedef struct {
    BoundingBox *boxes;
    int count;
    int capacity;
} BoundingBoxList;

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

// Création d'un dossier s'il n'existe pas
void create_output_directory(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0700) != 0) {
            fprintf(stderr, "Erreur lors de la création du dossier %s.\n", dir_name);
            exit(1);
        }
    }
}

// Parcourir le dossier images et exécuter ./resize pour chaque fichier
void process_images_in_directory(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le dossier %s.\n", directory);
        exit(1);
    }

    char command[512];

    while ((entry = readdir(dp)) != NULL) {
        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        // Construire la commande ./resize "fichier" 28 28
        snprintf(command, sizeof(command), "./resize \"%s\" 28 28", file_path);

        // Exécuter la commande
        printf("Exécution de la commande: %s\n", command);
        int ret = system(command);
        if (ret != 0) {
            fprintf(stderr, "Erreur: la commande \"%s\" a échoué.\n", command);
        }
    }

    closedir(dp);
}

// Exemple de fonction main qui utilise ces outils
int main(int argc, char *argv[]) {
    const char *output_directory = "images";

    // Créer le dossier "images" s'il n'existe pas
    create_output_directory(output_directory);

    // Parcourir les images et redimensionner
    process_images_in_directory(output_directory);

    printf("Traitement terminé.\n");
    return 0;
}

