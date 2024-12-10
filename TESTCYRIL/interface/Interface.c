#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

// Fonction pour créer un fichier indicateur
void create_flag(const char *flag_name) {
    FILE *file = fopen(flag_name, "w");
    if (file != NULL) {
        fclose(file);
    }
}

// Variable globale pour stocker le chemin de l'image sélectionnée
static gchar *selected_image_path = NULL;

// Declaration of variables for image display settings.
GtkWidget *image;
gdouble angle = 0.0;
gint max_width = 800;
gint max_height = 600;
guchar threshold = 128; 

// Structure representing a pixel with color.
typedef struct {
    guint8 r;
    guint8 g;
    guint8 b;
} Pixel;

// Structure representing an image composed of pixels.
typedef struct {
    int width;
    int height;
    Pixel **pixels;
} Image;

// Fonction pour redimensionner une image pour qu'elle s'adapte à l'affichage
GdkPixbuf *resize_pixbuf_to_fit(GdkPixbuf *pixbuf) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    // Dimensions de l'image redimensionnée
    int new_width = 800;
    int new_height = 800;

    // Redimensionnement
    GdkPixbuf *resized_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
    return resized_pixbuf;
}

void on_button_insert_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Sélectionner une image",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Annuler", GTK_RESPONSE_CANCEL,
                                         "_Ouvrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/bmp");
    gtk_file_filter_add_mime_type(filter, "image/gif");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf)
        {
            GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(pixbuf);
            GtkWidget *image = GTK_WIDGET(data); // Image cible pour afficher le fichier sélectionné
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
            g_object_unref(pixbuf);
            g_object_unref(resized_pixbuf);
        }

        // Mettre à jour le chemin de l'image sélectionnée
        if (selected_image_path) {
            g_free(selected_image_path);
        }
        selected_image_path = g_strdup(filename);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Fonction appelée lorsque le bouton de prétraitement est cliqué
void on_button_preprocessing_clicked(GtkWidget *widget, gpointer data) {
    create_flag(".preprocess_ready");
    system("make preprocess");
    g_print("Prétraitement prêt pour la compilation.\n"); 
   GtkWidget *image = GTK_WIDGET(data);

    if (selected_image_path != NULL) {
        char command[512];
        snprintf(command, sizeof(command), "cd ../link_pretraitement && ./link_pretraitement %s", selected_image_path);
        int result = system(command);

        if (result == 0) {
            g_print("Prétraitement terminé.\n");

            // Charger et afficher l'image générée (interface.png)
            const gchar *generated_image_path = "../link_pretraitement/interface.png";
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(generated_image_path, NULL);
            if (pixbuf) {
                GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(pixbuf);
                GtkWidget *image = GTK_WIDGET(data); // Image cible
                gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
                g_object_unref(pixbuf);
                g_object_unref(resized_pixbuf);
                g_print("Image mise à jour avec interface.png.\n");
            } else {
                g_print("Erreur : Impossible de charger interface.png.\n");
            }
        } else {
            g_print("Erreur lors du prétraitement.\n");
        }
    } else {
        g_print("Aucune image sélectionnée.\n");
    }
}

void on_button_remove_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *image = GTK_WIDGET(data);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), NULL);
}

void on_button_close_clicked(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

// Fonction pour sauvegarder l'image affichée dans GtkImage
void save_current_image(GtkWidget *image_widget, const char *output_filename) {
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image_widget));
    if (pixbuf == NULL) {
        g_print("Erreur : Aucune image n'est actuellement affichée.\n");
        return;
    }

    GError *error = NULL;

    // Sauvegarder le Pixbuf sous forme de fichier image (PNG)
    if (!gdk_pixbuf_save(pixbuf, output_filename, "png", &error, NULL)) {
        g_print("Erreur lors de la sauvegarde de l'image : %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("Image sauvegardée avec succès sous le nom : %s\n", output_filename);
    }
}

void set_button_font_size(GtkWidget *button, int font_size) {
    PangoFontDescription *font_desc;
    font_desc = pango_font_description_new();
    pango_font_description_set_family(font_desc, "Sans"); 
    pango_font_description_set_weight(font_desc, PANGO_WEIGHT_BOLD); 
    pango_font_description_set_size(font_desc, pango_units_from_double(font_size));
    gtk_widget_override_font(button, font_desc);
    pango_font_description_free(font_desc);
}

// Fonction appelée lorsque le bouton de solver est cliqué
void on_button_solver_clicked(GtkWidget *widget, gpointer data) {
    create_flag(".solver_ready");
    system("make solver");
    g_print("Solver prêt pour la compilation.\n");

    GtkWidget *image = GTK_WIDGET(data);

    if (selected_image_path != NULL) {
        char command[512];
        snprintf(command, sizeof(command), "cd ../extraction && ./executable %s", selected_image_path);
        int result = system(command);

        if (result == 0) {
            g_print("Solver fonctionne.\n");

            // Charger et afficher l'image générée (solver_final.png)
            const gchar *generated_image_path = "../solver/solver_final.png";
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(generated_image_path, NULL);
            if (pixbuf) {
                GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(pixbuf);
                GtkWidget *image = GTK_WIDGET(data); // Image cible
                gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
                g_object_unref(pixbuf);
                g_object_unref(resized_pixbuf);
                g_print("Image mise à jour avec le solver.png.\n");
            } else {
                g_print("Erreur : Impossible de charger solver.png.\n");
            }
        } else {
            g_print("Erreur lors du solver.\n");
        }
    } else {
        g_print("Aucune image sélectionnée.\n");
    }
}

// function to create a button
GtkWidget* create_button(const gchar* label, GCallback callback, gpointer data)
{
    GtkWidget* button = gtk_button_new_with_label(label);
    
    g_signal_connect(button, "clicked", callback, data);
    gtk_widget_set_size_request(button, 225, 75); 
    gtk_widget_set_margin_end(button, 80); 
    set_button_font_size(button, 20); 
    return button;
}

// function to add all buttons
void ajouter_un_boutton(GtkWidget* box, const gchar* label, GCallback callback, gpointer data)
{
    GtkWidget* button = create_button(label, callback, data);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
}

void on_button_save_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *image_widget = GTK_WIDGET(data);
    save_current_image(image_widget, "image_resolut.png");
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button_insert;
    GtkWidget *button_pretraitement;
    GtkWidget *button_remove;
    GtkWidget *button_solver;
    GtkWidget *enregistre;
    GtkWidget *button_close;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *button_box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interface Search Avenir");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    //gtk_window_set_default_size(GTK_WINDOW(window), max_width, max_height);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    ajouter_un_boutton(button_box, "Insérer une image", G_CALLBACK(on_button_insert_clicked), image);
    ajouter_un_boutton(button_box, "Supprimer l'image", G_CALLBACK(on_button_remove_clicked), image);
    ajouter_un_boutton(button_box, "Prétraitement", G_CALLBACK(on_button_preprocessing_clicked), image);
        gtk_grid_attach(GTK_GRID(window), button_pretraitement, 0, 0, 1, 1);
	ajouter_un_boutton(button_box, "Solver", G_CALLBACK(on_button_solver_clicked), image);
        gtk_grid_attach(GTK_GRID(window), button_solver, 0, 1, 1, 1);
	ajouter_un_boutton(button_box, "Enregistrer", G_CALLBACK(on_button_save_clicked), image);
    ajouter_un_boutton(button_box, "Fermer", G_CALLBACK(on_button_close_clicked), NULL);


    gtk_widget_show_all(window);

        gtk_main();

    // Libérer la mémoire utilisée pour le chemin de l'image sélectionnée
    if (selected_image_path) {
        g_free(selected_image_path);
    }

    gtk_main();

    return 0;
}


