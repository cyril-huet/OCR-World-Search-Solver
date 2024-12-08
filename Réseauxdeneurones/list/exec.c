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

// Fonction pour vérifier la largeur de l'image
int get_image_width(const char *image_path) {
    char command[256];
    char buffer[128];
    int width = 0;

    // Commande pour obtenir les dimensions de l'image
    snprintf(command, sizeof(command), "identify -format %%w %s", image_path);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur lors de l'exécution de la commande : %s\n", command);
        exit(EXIT_FAILURE);
    }

    // Lire la largeur
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        width = atoi(buffer);
    }
    pclose(fp);

    return width;
}


void process_directory(const char *directory_path) {
    DIR *dir;
    struct dirent *entry;

    // Ouvrir le dossier
    if ((dir = opendir(directory_path)) == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        exit(EXIT_FAILURE);
    }

    // Parcourir chaque fichier/dossier
    while ((entry = readdir(dir)) != NULL) {
        // Ignorer "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet du fichier
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

        // Vérifier si c'est un fichier image avec un nom correspondant à la convention "letter_lineX_colY.png"
        if (strstr(entry->d_name, "letter_line") != NULL && strstr(entry->d_name, ".png") != NULL) {
            char command[512];
            snprintf(command, sizeof(command), "./resize \"%s\" 28 28", file_path);
            printf("Exécution de la commande : %s\n", command);
            execute_command(command);
        }
    }

    // Fermer le dossier
    closedir(dir);
}


int main() {
    const char *image_path = "image_source.png";
    int width = get_image_width(image_path);
    printf("Largeur de l'image : %d pixels\n", width);

    // Vérification des cas de largeur d'image
    if (width == 250) {
        execute_command("convert image_source.png image_source.pgm");
        execute_command("cp image_source.pgm input.pgm");
        execute_command("./trait");
        execute_command("cp output.pgm image_source.pgm");
        execute_command("./facile image_source.pgm");
    } else if (width == 137 || width == 232 || width == 585 || width ==350) {
        execute_command("convert image_source.png image_source.pgm");
        execute_command("./facile image_source.pgm");
    } else if (width == 708) {
        execute_command("convert image_source.png image.pgm");
        execute_command("./fichier image.pgm");
        execute_command("convert output_with_boxes.pgm input.pgm");
        execute_command("./charline");
    } else {
        // Cas par défaut pour des largeurs autres
        execute_command("convert image_source.png image.pgm");
        execute_command("./fichier image.pgm");
    

    // Étapes communes à tous les cas
    execute_command("./pgm_to_png output.pgm output_with_boxes.png");
    execute_command("./fin");
    execute_command("./convert_png_to_pgm");
    execute_command("mv mot_png/* .");
    execute_command("./parcoursextraire");
    execute_command("./process_files");
    execute_command("./parcours");
    }
    const char *image_directory = "./images";
    process_directory(image_directory);

    return EXIT_SUCCESS;

}

