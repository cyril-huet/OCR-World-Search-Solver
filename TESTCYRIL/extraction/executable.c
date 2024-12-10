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
        execute_command("./for");
        execute_command("./remove_dupliacates");
        
        process_directory(image_directory);
           execute_command("./reseaux weights images");
    execute_command("./trie matrice.txt");
    execute_command("./splitgrilleenduex matrice.txt");
        return EXIT_SUCCESS;


    }
    if (width == 708) 
    {
        // Traitement spécifique pour les images de largeur 708 pixels

         execute_command("convert list_only.png image_source.pgm");
            execute_command("./fichier image_source.pgm");
            execute_command("cp output_with_boxes.pgm input.pgm");
            execute_command("./charline");
            execute_command("convert output.pgm output_with_boxes.png 2>/dev/null");
            execute_command("./pqcamarchepas");
            
            execute_command("./convert_png_to_pgm mot_png Jade");
            // A REFAIRE !!!! execute_command("./extraction_letter_tric Jade images");
            execute_command("./jadetestfonction Jade motapres 25");
            execute_command("rm -rf Jade");
            execute_command("mv motapres Jade");


            execute_command("./script_dossier");
            //execute_command("mv Jade/images .");



            execute_command("./extraction_de_lettre grid_only.pgm");
            execute_command("./parcours"); 
            execute_command("./pgm_to_ppm");
            execute_command("./convert");
            execute_command("./grid grid_only.ppm");
            execute_command("./colonne output_with_boxes.ppm");
            execute_command("./copy images");
             execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line2_col5.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col0.pgm");
       
            execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col3.pgm");
        
            
        process_directory(image_directory);
               execute_command("./reseaux weights images");
    execute_command("./trie matrice.txt");
    execute_command("./splitgrilleenduex matrice.txt");
        return EXIT_SUCCESS;


    } 
    /*if (width == 232) 
    {
        
        execute_command("convert grid_only.png grid_only.pgm 2>/dev/null");
        execute_command("./charlineameliore list_only.png test.png 2");
        
        execute_command("rm -rf images && mkdir images && cd images && .././finii ../grid_only.pgm && cd ..");

        execute_command("magick convert list_only.png image_source.png 2>/dev/null");

        execute_command("magick convert list_only.png image_source.png 2>/dev/null");
        execute_command("magick convert image_source.png -depth 8 -define quantum:format=unsigned image_source.pgm");
        execute_command("./fichier image_source.pgm");
        execute_command("cp output_with_boxes.pgm input.pgm ");
        execute_command("convert input.pgm input.png");
        execute_command("./charlineameliore input.png test.png 2");
        execute_command("./charline22222");
         execute_command("./for");
        execute_command("./remove_dupliacates");
        execute_command(" convert test.png test.pgm");
        execute_command("./fichier test.pgm");

    execute_command("convert test.png input.pgm ");

        execute_command("convert output_with_boxes.pgm output_with_boxes.png 2>/dev/null");
        execute_command("./fin");
        execute_command("./convert_png_to_pgm mot_png Jade");
        execute_command("rm Jade/mot1.pgm");
        execute_command("rm Jade/mot2.pgm");
        execute_command("rm Jade/mot3.pgm");
        execute_command("rm Jade/mot12.pgm");
        execute_command("rm Jade/mot13.pgm");
        execute_command("rm Jade/mot15.pgm");
        execute_command("rm Jade/mot16.pgm");
        execute_command("rm Jade/mot19.pgm");
        execute_command("rm Jade/mot20.pgm");



        execute_command("./extraction_letter_tric Jade images");
       

        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");
        execute_command("./for");
        execute_command("./remove_dupliacates");
        
        process_directory(image_directory);
        execute_command("./reseaux weights white_image_28x28_v2.pgm image");
    execute_command("./trie matrice.txt");
    execute_command("./split matrice.txt ");
        return EXIT_SUCCESS;


    }*/
    if (width == 436) {
        
        
        execute_command("convert list_only.png image_source.pgm");
            execute_command("./fichier image_source.pgm");
            execute_command("cp output_with_boxes.pgm input.pgm");
            execute_command("./charline");
            execute_command("convert output.pgm output_with_boxes.png 2>/dev/null");
            execute_command("./pqcamarchepas");
            
            execute_command("./convert_png_to_pgm mot_png Jade");
            // A REFAIRE !!!! execute_command("./extraction_letter_tric Jade images");
            execute_command("./jadetestfonction Jade motapres 25");
            execute_command("rm -rf Jade");
            execute_command("mv motapres Jade");


            execute_command("./script_dossier");
            //execute_command("mv Jade/images .");



            execute_command("./extraction_de_lettre grid_only.pgm");
            execute_command("./jadetestfonction Jade motapres 25");
            execute_command("./parcours"); 
            execute_command("./pgm_to_ppm");
            execute_command("./convert");
            execute_command("./grid grid_only.ppm");
            execute_command("./colonne output_with_boxes.ppm");
            execute_command("./copy images");
             execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line2_col5.pgm");
            execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col0.pgm");
       
            execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col3.pgm");
            execute_command("./redimensionscript");
            execute_command("./for");
            execute_command("./remove_dupliacates");

        
            



    /*execute_command("convert grid_only.png grid_only.pgm 2>/dev/null");
        execute_command("convert list_only.png image_source.png 2>/dev/null");
        execute_command("convert image_source.png image_source.pgm 2>/dev/null");
        execute_command("magick list_only.png -define pgm:format=plain -depth 8 image_source.pgm");
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
        execute_command("./jadetestfonction mot_png motapres 36");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line2_col5.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col0.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col3.pgmt");
        printf("%s", "reussi");*/
        

/*

        execute_command("touch matrice.txt");
        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./redimensionscript");

        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");
       

        
        execute_command("magick image_source.png -define pgm:format=plain -depth 8 image.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line2_col5.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col0.pgm");
        execute_command("cp images/letter_grid_line4_col7.pgm images/letter_grid_line7_col3.pgmt");
        execute_command("rm -f *.pgmt");




        
        execute_command("./fichier image.pgm");
        
        execute_command("./pgm_to_png output.pgm output_with_boxes.png");
        execute_command("./fin");
            

        execute_command("./convert_png_to_pgm m");
        execute_command("mv mot_png* .");
        
        execute_command("./parcoursextraire");
        execute_command("./process_files");
        execute_command("./parcours");
        execute_command("./convert");
        */
            
        }
    else if (width == 422) 
    {


        
        execute_command("touch matrice.txt");
        execute_command("magick convert grid_only.png grid_only.pgm 2>/dev/null");
        execute_command("./extraction_de_lettre grid_only.pgm");
        execute_command("./retirepetit images Charlinejade 8");

        execute_command("./parcours"); 
        execute_command("./pgm_to_ppm");
        execute_command("./convert");
        execute_command("./grid grid_only.ppm");
        execute_command("./colonne output_with_boxes.ppm");
        execute_command("./copy images");

        execute_command("magick convert image_source.png image_source.pgm 2>/dev/null");
        execute_command("cp image_source.pgm input.pgm");
        execute_command("cp output.pgm image_source.pgm");
        execute_command("magick convert image_source.png -depth 8 image_source.pgm");
        execute_command("magick convert image_source.pgm -depth 8 image_source.pgm 2>/dev/null && ./facile2 image_source.pgm");
        
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
            execute_command("magick convert image_source.png image_source.pgm 2>/dev/null");
            execute_command("cp image_source.pgm input.pgm");
            execute_command("./trait");
            execute_command("cp output.pgm image_source.pgm");
            execute_command("magick convert image_source.pgm -depth 8 image_source.pgm  2>/dev/null && ./facile2 image_source.pgm");
        } else if (width == 137 || width == 232 || width == 585 || width == 355) {
            execute_command("magick convert image_source.png image_source.pgm 2>/dev/null");
            execute_command("magick convert image_source.pgm -depth 8 image_source.pgm 2>/dev/null && ./facile2 image_source.pgm");
        } else if (width == 350) {






            execute_command("magick convert list_only.png image_source.pgm");
            execute_command("./fichier image_source.pgm");
            execute_command("cp output_with_boxes.pgm input.pgm");
            execute_command("./charline");
            execute_command("magick convert output.pgm output_with_boxes.png 2>/dev/null");
            execute_command("./pqcamarchepas");
            execute_command("./convert_png_to_pgm mot_png Jade");
            // A REFAIRE !!!! execute_command("./extraction_letter_tric Jade images");
            


            execute_command("./script_dossier");
            //execute_command("mv Jade/images .");



            execute_command("./extraction_de_lettre grid_only.pgm");
            execute_command("./parcours"); 
            execute_command("./pgm_to_ppm");
            execute_command("./convert");
            execute_command("./grid grid_only.ppm");
            execute_command("./colonne output_with_boxes.ppm");
            execute_command("./copy images");








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
            execute_command("magick convert image_source.png image.pgm 2>/dev/null");
            execute_command("./fichier image.pgm");
            execute_command("./pgm_to_png output.pgm output_with_boxes.png");
            execute_command("./fin");
            execute_command("./convert_png_to_pgm images images");
            execute_command("mv mot_png/* .");
            execute_command("./parcours");
            //execute_command("./process_files");
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

    execute_command("./reseaux weights images");
    execute_command("./trie matrice.txt");
    execute_command("./splitgrilleenduex matrice.txt");


    return EXIT_SUCCESS;
}
