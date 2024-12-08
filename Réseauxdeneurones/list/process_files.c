#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Fonction pour exécuter une commande système
void execute_command(const char* filename) {
    char command[256];
    snprintf(command, sizeof(command), "./extraire %s", filename);
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de la commande : %s\n", command);
    }
}

// Fonction pour parcourir les fichiers et exécuter la commande pour chaque fichier correspondant
void process_files(const char* directory, const char* base_name) {
    struct dirent* entry;
    DIR* dir = opendir(directory);

    if (!dir) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le répertoire %s\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Vérifier si le fichier correspond au modèle "mot%numero.pgm"
        char* filename = entry->d_name;
        if (strstr(filename, base_name) == filename && strstr(filename, ".pgm")) {
            // Extraire la partie après "mot" pour vérifier le numéro
            const char* suffix = filename + strlen(base_name);
            char* endptr;
            long number = strtol(suffix, &endptr, 10);

            // Si le suffixe est un numéro valide suivi de ".pgm", traiter le fichier
            if (*endptr == '.' && strcmp(endptr, ".pgm") == 0 && number > 0) {
                printf("Traitement du fichier : %s\n", filename);
                execute_command(filename);
            }
        }
    }

    closedir(dir);
}

// Exemple d'utilisation
int main() {
    const char* directory = ".";      // Répertoire courant
    const char* base_name = "mot";    // Préfixe "mot"

    SDL_Init(SDL_INIT_VIDEO); // Initialisation de SDL (pas utilisée directement mais utile pour les projets SDL)

    process_files(directory, base_name);

    SDL_Quit(); // Libération des ressources SDL

    return 0;
}

