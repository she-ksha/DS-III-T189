#include "research_organizer.h"

// --- Global Variable Definitions ---
gchar *current_user_id = NULL; 
GtkWidget *main_window = NULL; 
GtkApplication *global_app = NULL; 
//mock
static void init_app_environment() {
    g_print("Frontend Init: Mock environment initialized. (No database or file system setup).\n");
}



void on_exit_clicked(GtkButton *button, gpointer data) {
    g_application_quit(G_APPLICATION(global_app));
}



static void on_app_activate(GtkApplication *app, gpointer user_data) {
    global_app = app;
    
 
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider, APP_CSS, -1, NULL);
    g_object_unref(provider);
    
    create_login_window(app);
}

int main(int argc, char **argv) {
    init_app_environment();

    GtkApplication *app;
    int status;

    app = gtk_application_new("com.gemini.researchpaperorganizer", G_APPLICATION_DEFAULT_FLAGS);
    
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);

    if (current_user_id) g_free(current_user_id);

    return status;
}