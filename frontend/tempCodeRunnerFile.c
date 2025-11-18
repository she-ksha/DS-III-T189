#include "research_organizer.h"

// --- Global Variable Definitions ---
GtkApplication *global_app = NULL;
GtkWidget *main_window = NULL;
// --- ADD THESE DEFINITIONS ---
GtkWidget *total_papers_value_label = NULL;
GtkWidget *hash_map_load_label = NULL;
// --- END ADD ---

// Global Widgets (defined in header, initialized in UI files)
GtkListStore *paper_list_store = NULL;
GtkTextView *notes_text_view = NULL;
GtkWidget *paper_title_label = NULL;



// --- MODIFIED: This is where we load the real backend ---
static void on_app_activate(GtkApplication *app, gpointer user_data) {
    global_app = app;
    
    // 1. Init paper storage (empties memory)
    init_paper_storage(); 
    
    // 2. Load all users from 'users.csv' into memory
    load_users();
    // --- 3. LOAD THE GLOBAL PAPER DATABASE ---
    import_from_csv();
    
    // 3. Load CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider, APP_CSS, -1, NULL);
    g_object_unref(provider);
    
    // 4. Show Login Window
    // (We will load papers *after* login is successful)
    create_login_window(app);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.gemini.researchpaperorganizer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
