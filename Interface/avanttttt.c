#include <gtk/gtk.h>


GtkWidget *image;
gdouble angle = 0.0;
gint max_width = 800;
gint max_height = 600;
guchar threshold = 128; 

GdkPixbuf *resize_pixbuf_to_fit(GdkPixbuf *pixbuf)
{
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gdouble scale = MIN((gdouble)max_width / width, (gdouble)max_height / height);
    
    if (scale < 1.0)
    {
        gint new_width = (gint)(width * scale);
        gint new_height = (gint)(height * scale);
        GdkPixbuf *resized_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
        return resized_pixbuf;
    }
    return g_object_ref(pixbuf);
}


void BlackandWhite(GdkPixbuf *pixbuf) {
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    gint n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (gint y = 0; y < height; y++) {
        for (gint x = 0; x < width; x++) {
            guchar *px = pixels + y * rowstride + x * n_channels;

            guchar red = px[0];
            guchar green = px[1];
            guchar blue = px[2];
            guchar gray = (red + green + blue) / 3;

            guchar bw_color = (gray >= 130) ? 255 : 0;

            px[0] = bw_color;
            px[1] = bw_color;
            px[2] = bw_color;
        }
    }
}


GdkPixbuf *convert_pixbuf_to_bw(GdkPixbuf *pixbuf)
{
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    gint n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    
    GdkPixbuf *new_pixbuf = gdk_pixbuf_copy(pixbuf);
    guchar *new_pixels = gdk_pixbuf_get_pixels(new_pixbuf);

    for (gint y = 0; y < height; y++)
    {
        for (gint x = 0; x < width; x++)
        {
            guchar *px = pixels + y * rowstride + x * n_channels;
            guchar *new_px = new_pixels + y * rowstride + x * n_channels;
            
            guchar red = px[0];
            guchar green = px[1];
            guchar blue = px[2];
            guchar gray = (red + green + blue) / 3;

            new_px[0] = gray;
            new_px[1] = gray;
            new_px[2] = gray;
        }
    }
    return new_pixbuf;
}

GdkPixbuf *convert_pixbuf_to_binary(GdkPixbuf *pixbuf)
{
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    gint n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    
    GdkPixbuf *new_pixbuf = gdk_pixbuf_copy(pixbuf);
    guchar *new_pixels = gdk_pixbuf_get_pixels(new_pixbuf);

    for (gint y = 0; y < height; y++)
    {
        for (gint x = 0; x < width; x++)
        {
            guchar *px = pixels + y * rowstride + x * n_channels;
            guchar *new_px = new_pixels + y * rowstride + x * n_channels;
            guchar red = px[0];
            guchar green = px[1];
            guchar blue = px[2];
            guchar gray = (red + green + blue) / 3;
            guchar binary_color = (gray > threshold) ? 255 : 0;
            new_px[0] = binary_color;
            new_px[1] = binary_color;
            new_px[2] = binary_color;
        }
    }
    return new_pixbuf;
}



void on_button_rotate_clicked(GtkWidget *widget, gpointer data)
{
    angle += 90.0;
    if (angle >= 360.0)
    {
        angle = 0.0;
    }

    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    if (pixbuf != NULL)
    {
        GdkPixbuf *rotated_pixbuf = gdk_pixbuf_rotate_simple(pixbuf, GDK_PIXBUF_ROTATE_CLOCKWISE);
        GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(rotated_pixbuf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
        g_object_unref(rotated_pixbuf);
        g_object_unref(resized_pixbuf);
    }
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
    gtk_file_filter_add_mime_type(filter, "image/gif");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf)
        {
            GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(pixbuf);
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
            g_object_unref(pixbuf);
            g_object_unref(resized_pixbuf);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_button_remove_clicked(GtkWidget *widget, gpointer data)
{
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), NULL);
}

void on_button_bw_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    if (pixbuf != NULL)
    {
        BlackandWhite(pixbuf);
        GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(pixbuf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
        g_object_unref(resized_pixbuf);
    }
}

void on_button_binary_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    if (pixbuf != NULL)
    {
        GdkPixbuf *binary_pixbuf = convert_pixbuf_to_bw(pixbuf);
        GdkPixbuf *resized_pixbuf = resize_pixbuf_to_fit(binary_pixbuf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), resized_pixbuf);
        g_object_unref(binary_pixbuf);
        g_object_unref(resized_pixbuf);
    }
}
void on_button_close_clicked(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
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

GtkWidget* create_button(const gchar* label, GCallback callback, gpointer data)
{
    GtkWidget* button = gtk_button_new_with_label(label);
    
    g_signal_connect(button, "clicked", callback, data);
    gtk_widget_set_size_request(button, 225, 75); 
    gtk_widget_set_margin_end(button, 80); 
    set_button_font_size(button, 20); 
    return button;
}

void ajouter_un_boutton(GtkWidget* box, const gchar* label, GCallback callback, gpointer data)
{
    GtkWidget* button = create_button(label, callback, data);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button_rotate;
    GtkWidget *button_insert;
    GtkWidget *button_remove;
    GtkWidget *button_bw;
    GtkWidget *button_binary;
    GtkWidget *button_close;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *button_box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Manipulation d'image");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), max_width, max_height);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    ajouter_un_boutton(button_box, "Insérer une images", G_CALLBACK(on_button_insert_clicked), window);
    ajouter_un_boutton(button_box, "Faire tourner", G_CALLBACK(on_button_rotate_clicked), NULL);
    ajouter_un_boutton(button_box, "Supprimer l'image", G_CALLBACK(on_button_remove_clicked), NULL);
    ajouter_un_boutton(button_box, "Noir et Blanc", G_CALLBACK(on_button_binary_clicked), NULL);
    ajouter_un_boutton(button_box, "Binariser", G_CALLBACK(on_button_bw_clicked), NULL);
    ajouter_un_boutton(button_box, "Fermer", G_CALLBACK(on_button_close_clicked), NULL);



    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}