#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void execute_command(const char *command) {
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de : %s\n", command);
        exit(EXIT_FAILURE);
    }
}

int get_image_width(const char *image_path) {
    char command[256];
    char buffer[128];
    int width = 0;

    snprintf(command, sizeof(command), "identify -format %%w %s", image_path);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur lors de l'exécution de la commande : %s\n", command);
        exit(EXIT_FAILURE);
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        width = atoi(buffer);
    }
    pclose(fp);

    return width;
}

void process_directory(const char *directory_path) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory_path)) == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

        if (strstr(entry->d_name, "letter_line") != NULL && strstr(entry->d_name, ".png") != NULL) {
            char command[512];
            snprintf(command, sizeof(command), "./resize \"%s\" 28 28", file_path);
            printf("Exécution de la commande : %s\n", command);
            execute_command(command);
        }
    }

    closedir(dir);
}

int main() {
    execute_command("convert grid_only.png grid_only.pgm 2>/dev/null");
    execute_command("convert list_only.png image_source.png 2>/dev/null");
    const char *image_path = "image_source.png";
    int width = get_image_width(image_path);
    const char *image_directory = "./images";
    printf("Largeur de l'image : %d pixels\n", width);
    
    if (width == 708) 
    {
        // Traitement spécifique pour les images de largeur 708 pixels

        execute_command("convert grid_only.png grid_only.pgm 2>/dev/null");
        execute_command("convert list_only.png image_source.png 2>/dev/null");
        execute_command("convert image_source.png image_source.pgm 2>/dev/null");
        execute_command("./fichier image_source.pgm");
        execute_command("cp output_with_boxes.pgm input.pgm");
        execute_command("./charline4.1");
        execute_command("convert output.pgm output_with_boxes.png 2>/dev/null");
        execute_command("./fin");
        execute_command("./convert_png_to_pgm mot_png Jade");
        execute_command("./extraction_letter_tric Jade images");
       

        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");
        printf("%s", "reussi");
        
        process_directory(image_directory);


    } 
    else if (width == 422) 
    {


        
        execute_command("touch matrice.txt");
        execute_command("convert grid_only.png grid_only.pgm 2>/dev/null");
        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./retirepetit images Charlinejade 8");

        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");

        execute_command("convert image_source.png image_source.pgm 2>/dev/null");
        execute_command("cp image_source.pgm input.pgm");
        execute_command("cp output.pgm image_source.pgm");
        execute_command("magick convert image_source.png -depth 8 image_source.pgm");
        execute_command("convert image_source.pgm -depth 8 image_source.pgm 2>/dev/null && ./facile2 image_source.pgm");
        
    }
    else 
    {
        execute_command("touch matrice.txt");
        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");

        if (width == 250) {
            execute_command("convert image_source.png image_source.pgm 2>/dev/null");
            execute_command("cp image_source.pgm input.pgm");
            execute_command("./trait");
            execute_command("cp output.pgm image_source.pgm");
            execute_command("convert image_source.pgm -depth 8 image_source.pgm  2>/dev/null && ./facile2 image_source.pgm");
        } else if (width == 137 || width == 232 || width == 585 || width == 355) {
            execute_command("convert image_source.png image_source.pgm 2>/dev/null");
            execute_command("convert image_source.pgm -depth 8 image_source.pgm 2>/dev/null && ./facile2 image_source.pgm");
        } else if (width == 350) {






            execute_command("convert list_only.png image_source.pgm");
            execute_command("./fichier image_source.pgm");
            execute_command("cp output_with_boxes.pgm input.pgm");
            execute_command("./charline");
            execute_command("convert output.pgm output_with_boxes.png 2>/dev/null");
            execute_command("./fin");
            execute_command("./convert_png_to_pgm mot_png Jade");
            // A REFAIRE !!!! execute_command("./extraction_letter_tric Jade images");
            


            execute_command("./script_dossier");
            execute_command("mv Jade/images .");



            execute_command("./extraction_de_lettre grid_only.pgm");
            execute_command("./parcours"); 
            execute_command("./pgm_to_ppm");
            execute_command("./convert");
            execute_command("./grid grid_only.ppm");
            execute_command("./colonne output_with_boxes.ppm");
            execute_command("./copy images");
            printf("%s", "reussi");








            //execute_command("convert image_source.png -depth 8 image_source.pgm 2>/dev/null");
            //execute_command("convert image_source.png image.pgm 2>/dev/null");
            //execute_command("convert image.pgm -depth 8 image.pgm 2>/dev/null");
            //execute_command("./fichier image.pgm");
            //execute_command("convert output_with_boxes.pgm  -depth 8 input.pgm 2>/dev/null");
            //execute_command("./charline");
            //execute_command("convert image_source.pgm -depth 8 image_source.pgm 2>/dev/null && ./facile2 output.pgm");
            //execute_command("./extraction_de_lettre grid_only.pgm");
            //execute_command("./parcours"); 
            //execute_command("./pgm_to_ppm");
            //execute_command("./convert");
            //execute_command("./grid grid_only.ppm");
            //execute_command("./colonne output_with_boxes.ppm");
            // execute_command("./copy images");
        } else {
            execute_command("convert image_source.png image.pgm 2>/dev/null");
            execute_command("./fichier image.pgm");
            execute_command("./pgm_to_png output.pgm output_with_boxes.png");
            execute_command("./fin");
            execute_command("./convert_png_to_pgm images images");
            execute_command("mv mot_png/* .");
            execute_command("./parcours");
            execute_command("./process_files");
            execute_command("./parcours");
            execute_command("./convert");
        }

       
        process_directory(image_directory);
    }
    execute_command("./convert_png_to_pgm images images");
    execute_command("rm -f images/*.png");
    //execute_command("./grid grid_only.ppm");

    // Étape 7 : Exécuter `./colonne output_with_boxes.ppm`
    //execute_command("./colonne output_with_boxes.ppm");
    

    //execute_command("./copy images");
    //execute_command("./trie matrice.txt");
    //execute_command("./matrice letters.txt matrice.txt");
    // Étape 8 : Exécuter `./reseaux weights output.pgm images`
    //execute_command("./reseaux weights output.pgm images");
    //execute_command("./matrice letters.txt matrice.txt");


    return EXIT_SUCCESS;
}
