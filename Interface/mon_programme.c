#include <gtk/gtk.h>

// Fonction appelée lorsque le bouton est cliqué
void on_button_clicked(GtkWidget *widget, gpointer image)
{
    GtkWidget *dialog;
    GtkWidget *file_chooser;
    GtkFileFilter *filter;
    GtkImage *img = GTK_IMAGE(image);

    // Créer une fenêtre de dialogue pour choisir un fichier
    dialog = gtk_file_chooser_dialog_new("Choisir une image",
                                         GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Annuler", GTK_RESPONSE_CANCEL,
                                         "_Ouvrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Ajouter un filtre pour les fichiers image
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/gif");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Afficher la fenêtre de dialogue et attendre la réponse de l'utilisateur
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GdkPixbuf *pixbuf;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        if (pixbuf != NULL)
        {
            gtk_image_set_from_pixbuf(img, pixbuf);
            g_print("Image chargée !\n");
            g_object_unref(pixbuf);
        }
        else
        {
            g_print("Erreur lors du chargement de l'image.\n");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *box;

    // Initialiser GTK
    gtk_init(&argc, &argv);

    // Créer une nouvelle fenêtre
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interface avec un bouton et une image");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Créer une boîte pour organiser les widgets
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Créer un bouton
    button = gtk_button_new_with_label("Cliquez-moi pour charger une image");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

    // Créer un widget image vide
    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);

    // Afficher tous les widgets
    gtk_widget_show_all(window);

    // Exécuter la boucle principale
    gtk_main();

    return 0;
}

