#include "research_organizer.h"
// --- Global CSS Definition ---
const gchar *APP_CSS = 
    "GtkWindow { background-color: #e8eaf6; font-family: Inter, sans-serif; }"
    "#login-card { background-color: #ffffff; border-radius: 12px; padding: 30px; box-shadow: 0 4px 18px rgba(0, 0, 0, 0.15); border: 1px solid #c5cae9; }"
    "button.suggested-action { background-image: none; background-color: #3f51b5; color: white; border: none; font-weight: bold; transition: background-color 0.2s; }"
    "button.suggested-action:hover { background-color: #303f9f; }"
    "button.secondary-action { background-color: #e0e0e0; color: #333333; border: 1px solid #c0c0c0; }"
    "button.secondary-action:hover { background-color: #d0d0d0; }"
    "button.destructive-action { background-color: #D32F2F; color: white; }"
    "button.destructive-action:hover { background-color: #C62828; }"
    "button { border-radius: 8px; padding: 10px 15px; margin: 5px 0; font-size: 14px; }"
    "GtkTreeView { background-color: white; border-radius: 8px; border: 1px solid #c5cae9; }"
    "GtkTextView { background-color: #ffffff; border-radius: 8px; padding: 10px; border: 1px solid #c5cae9; }"
    "GtkSearchEntry, GtkEntry { border-radius: 6px; padding: 8px; border: 1px solid #c5cae9; }"
    ".graph-area { background-color: #e3e7ff; border: 2px dashed #3f51b5; border-radius: 8px; }"
    ".dashboard-stat { background-color: #ffffff; border-radius: 8px; padding: 20px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.08); }";

// --- UI Helper ---
void show_message_dialog(GtkWindow *parent, const gchar *title, const gchar *message, GtkMessageType type) {
     GtkWidget *dialog = gtk_message_dialog_new(parent,
                                            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                            type,
                                            GTK_BUTTONS_OK,
                                            "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// --- Window Close Helper ---
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    gtk_widget_hide(widget);
    return TRUE; // This stops the window from being destroyed
}

// --- Exit Page Logic ---
 void on_exit_clicked(GtkButton *button, gpointer data) {
    g_print("[SYSTEM]: Exit clicked. Saving data to CSV...\n");
    
    // --- 1. Save all data to the file ---
    export_to_csv();
    
    // --- 2. Quit the application ---
    g_application_quit(G_APPLICATION(global_app));
}

GtkWidget *create_exit_program_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 50);
    GtkWidget *message = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(message), 
        "<span size='x-large' weight='bold'>Are you sure you want to exit?</span>\n\n"
        "<span size='large'>All changes will be saved to 'all_papers.csv'.</span>");
    gtk_label_set_justify(GTK_LABEL(message), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), message, FALSE, FALSE, 0);

    GtkWidget *exit_button = gtk_button_new_with_label("Save and Exit Program");
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(exit_button), "destructive-action");
    gtk_box_pack_start(GTK_BOX(vbox), exit_button, FALSE, FALSE, 0);
    return vbox;
}

// --- END OF MISSING CODE ---
// In window.c
// REPLACE your old function with this
void create_main_app_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Research Paper Organizer");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 800);
    g_signal_connect(main_window, "destroy", G_CALLBACK(g_application_quit), global_app);

    // 1. The store is created ONCE.
    paper_list_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(main_window), notebook);

    // 2. Create, SHOW, and append all pages
    GtkWidget *page;

    page = create_dashboard_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("DASHBOARD"));

    page = create_paper_management_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("PAPER MANAGEMENT"));

    page = create_search_filter_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("SEARCH"));

    page = create_knowledge_graph_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("KNOWLEDGE GRAPH"));

    page = create_collaboration_tools_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("NOTES & COLLAB"));

    page = create_reports_export_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("REPORTS"));

    page = create_exit_program_page();
    gtk_widget_show(page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("EXIT"));
}