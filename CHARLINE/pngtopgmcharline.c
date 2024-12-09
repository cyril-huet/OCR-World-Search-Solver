#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

void convert_to_pgm(const char *filename) {
    // Construire la commande pour convert
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s.pgm", filename); // Génère le nom de sortie

    char *args[] = {"convert", (char *)filename, output_file, NULL}; // Arguments pour execvp

    // Exécuter la commande
    if (fork() == 0) { // Créer un processus enfant
        execvp("convert", args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    // Attendre le processus enfant pour éviter les processus zombies
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        printf("Converted %s -> %s\n", filename, output_file);
    } else {
        fprintf(stderr, "Conversion failed for %s\n", filename);
    }
}

int main() {
    DIR *d;
    struct dirent *dir;

    // Ouvrir le répertoire courant
    d = opendir(".");
    if (!d) {
        perror("opendir failed");
        return EXIT_FAILURE;
    }

    // Parcourir les fichiers dans le répertoire
    while ((dir = readdir(d)) != NULL) {
        // Ignorer "." et ".."
        if (dir->d_name[0] == '.') continue;

        // Vérifier si c'est un fichier image (par exemple, .jpg ou .png)
        if (strstr(dir->d_name, ".jpg") || strstr(dir->d_name, ".png")) {
            convert_to_pgm(dir->d_name);
        }
    }

    closedir(d);
    return EXIT_SUCCESS;
}

