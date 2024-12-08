#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Fonction pour exécuter une commande shell
void execute_command(const char *command) {
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de : %s\n", command);
        exit(EXIT_FAILURE);
    }
}

// Fonction pour exécuter ./extraire sur tous les fichiers "mot*.pgm"
void process_pgm_files_with_extraire(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        exit(EXIT_FAILURE);
    }

    char pgm_file[512];
    char command[1024];

    while ((entry = readdir(dp)) != NULL) {
        // Vérifier si le fichier correspond au motif "mot*.pgm"
        if (strstr(entry->d_name, "mot") == entry->d_name && strstr(entry->d_name, ".pgm")) {
            snprintf(pgm_file, sizeof(pgm_file), "%s/%s", directory, entry->d_name);

            // Construire et exécuter la commande ./extraire
            snprintf(command, sizeof(command), "./extraire %s", pgm_file);
            printf("Exécution de la commande : %s\n", command);
            execute_command(command);
        }
    }

    closedir(dp);
}

int main() {
    const char *directory = "."; // Dossier courant ou spécifier un autre chemin

    // Appeler la fonction pour traiter les fichiers "mot*.pgm"
    process_pgm_files_with_extraire(directory);

    return EXIT_SUCCESS;
}
