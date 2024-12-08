#include <stdio.h>
#include <stdlib.h>

// Fonction pour exécuter une commande shell
void execute_command(const char *command) {
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de : %s\n", command);
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Étape 1 : Exécuter `make`
    execute_command("./png_to_pgm image_source.png image.pgm");
    execute_command("./fichier image.pgm");
    execute_command("./pgm_to_png output_with_boxes.pgm output_with_boxes.png");
    execute_command("./fin");
    execute_command("./convert_png_to_pgm");
    execute_command("./parcoursextraire");
    return EXIT_SUCCESS;
}

