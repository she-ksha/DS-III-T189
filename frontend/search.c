#include "research_organizer.h"

// --- Signal Handlers ---

void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data) {
    update_paper_list(gtk_entry_get_text(GTK_ENTRY(entry))); 
}


GtkWidget *create_search_filter_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Quick Search &amp; Filter</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    GtkWidget *search_entry = gtk_search_entry_new();
    //gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "\xF0\N{U+1F50D} Search by Title, Author, or Keywords..."); // Using \N{} syntax
   // --- NEW (FIXED) ---
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "\xF0\x9F\x94\x8D Search by Title, Author, or Keywords...");
    g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_entry_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), search_entry, FALSE, FALSE, 0);

    // --- This filter_grid code is correct ---
    GtkWidget *filter_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(filter_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(filter_grid), 20);
    // ... (all your gtk_grid_attach calls) ...
    gtk_box_pack_start(GTK_BOX(vbox), filter_grid, FALSE, FALSE, 10);
    // --- End of filter_grid code ---


    // --- THIS IS THE FIX ---
    // 1. Create a TreeView and tell it to use the SAME GLOBAL 'paper_list_store'
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(paper_list_store));
    
    // 2. Connect the signal
    g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
    
    // 3. Add the columns
    const gchar *titles[] = {"ID", "Title", "Author", "Year", "Keywords"};
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        if (i == 0) gtk_tree_view_column_set_visible(column, FALSE);
        if (i == 1) gtk_tree_view_column_set_expand(column, TRUE); 
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }
    // --- END FIX ---

    // 4. Add the new tree_view to the scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    return vbox;
}


