#include <stdio.h>
#include <stdlib.h>

// Fonction pour exécuter une commande shell
void execute_command(const char *command) {
    printf("Exécution de : %s\n", command);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de : %s\n", command);
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Étape 1 : Exécuter `make`
    execute_command("make");

    // Étape 2 : Exécuter `./extraction_de_lettre grid_only.pgm`
    execute_command("./extraction_de_lettre grid_only.pgm");

    // Étape 3 : Exécuter `./parcours`
    execute_command("./parcours");

    // Étape 4 : Exécuter `./convert`
    execute_command("./convert");

    // Étape 5 : Exécuter `./pgm_to_ppm`
    execute_command("./pgm_to_ppm");

    // Étape 6 : Exécuter `./grid grid_only.ppm`
    execute_command("./grid grid_only.ppm");

    // Étape 7 : Exécuter `./colonne output_with_boxes.ppm`
    execute_command("./colonne output_with_boxes.ppm");
    

    execute_command("./copy images");
    execute_command("./trie matrice.txt");

    // Étape 8 : Exécuter `./reseaux weights output.pgm images`
    execute_command("./reseaux weights output.pgm images");

    printf("Toutes les étapes ont été exécutées avec succès.\n");
    return EXIT_SUCCESS;
}

