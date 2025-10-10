#include "research_organizer.h"

// --- Global Variable Definitions (Externs from Header) ---
const gchar *APP_CSS = 
    "GtkWindow { background-color: #e8eaf6; font-family: Inter, sans-serif; }"
    
    "#login-card { background-color: #ffffff; border-radius: 12px; padding: 30px; box-shadow: 0 4px 18px rgba(0, 0, 0, 0.15); border: 1px solid #c5cae9; }"

    // Primary Button Styling 
    "button.suggested-action { background-image: none; background-color: #3f51b5; color: white; border: none; font-weight: bold; transition: background-color 0.2s; }"
    "button.suggested-action:hover { background-color: #303f9f; }"

    // Secondary Button Styling 
    "button.secondary-action { background-color: #e0e0e0; color: #333333; border: 1px solid #c0c0c0; }"
    "button.secondary-action:hover { background-color: #d0d0d0; }"
    
    // General Button Styling 
    "button { border-radius: 8px; padding: 10px 15px; margin: 5px 0; font-size: 14px; }"
    
    // TreeView (List) Styling 
    "GtkTreeView { background-color: white; border-radius: 8px; border: 1px solid #c5cae9; }"
    
    //TextView (Notes) Styling 
    "GtkTextView { background-color: #ffffff; border-radius: 8px; padding: 10px; border: 1px solid #c5cae9; }"
    
    // Search Entry Styling 
    "GtkSearchEntry, GtkEntry { border-radius: 6px; padding: 8px; border: 1px solid #c5cae9; }"
    
    // Styling for Knowledge Graph 
    ".graph-area { background-color: #e3e7ff; border: 2px dashed #3f51b5; border-radius: 8px; }"

    // Dashboard Stat 
    ".dashboard-stat { background-color: #ffffff; border-radius: 8px; padding: 20px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.08); }"
;



void show_message_dialog(const gchar *title, const gchar *message, GtkMessageType type) {
     GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),GTK_DIALOG_MODAL, type,GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    gtk_widget_hide(widget);
    return TRUE;
}


//   Main Window  

void create_main_app_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Research Paper Organizer and Knowledge Graph");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 800);
    
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(main_window), notebook);
    
    gtk_widget_set_name(notebook, "main-notebook");

   
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_dashboard_page(), gtk_label_new(" DASHBOARD"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_paper_management_page(), gtk_label_new(" PAPER MANAGEMENT"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_search_filter_page(), gtk_label_new(" SEARCH PAPER"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_knowledge_graph_page(), gtk_label_new(" KNOWLEDGE GRAPH"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_collaboration_tools_page(), gtk_label_new(" COLLABORATION TOOLS"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_reports_export_page(), gtk_label_new(" REPORTS "));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_exit_program_page(), gtk_label_new(" EXIT PROGRAM"));


    update_paper_list(NULL);
    gtk_widget_show_all(main_window);
}