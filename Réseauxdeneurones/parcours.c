#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>

// Définir des constantes pour une meilleure lisibilité
#define THRESHOLD 128 // Seuil pour la binarisation
#define LINE_TOLERANCE 10 // Tolérance pour regrouper les lettres dans une ligne
#define COMMAND_BUFFER_SIZE 512
#define FILE_PATH_BUFFER_SIZE 512

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
            perror("Erreur lors de la création du dossier");
            exit(EXIT_FAILURE);
        }
    }
}

// Parcourir le dossier images et exécuter ./resize pour chaque fichier
void process_images_in_directory(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("Erreur: impossible d'ouvrir le dossier");
        exit(EXIT_FAILURE);
    }

    char command[COMMAND_BUFFER_SIZE];

    while ((entry = readdir(dp)) != NULL) {
        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier
        char file_path[FILE_PATH_BUFFER_SIZE];
        if (snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name) >= sizeof(file_path)) {
            fprintf(stderr, "Erreur: chemin de fichier trop long.\n");
            continue;
        }

        // Construire la commande ./resize "fichier" 28 28
        if (snprintf(command, sizeof(command), "./resize \"%s\" 28 28", file_path) >= sizeof(command)) {
            fprintf(stderr, "Erreur: commande trop longue.\n");
            continue;
        }

        // Exécuter la commande
        printf("Exécution de la commande: %s\n", command);
        int ret = system(command);
        if (ret != 0) {
            fprintf(stderr, "Erreur: la commande \"%s\" a échoué avec le code %d.\n", command, ret);
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
    return EXIT_SUCCESS;
}

