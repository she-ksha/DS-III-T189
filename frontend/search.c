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
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "\xF0\x9F\x94\x8D Search by Title, Author, or Keywords...");
    g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_entry_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), search_entry, FALSE, FALSE, 0);

    GtkWidget *filter_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(filter_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(filter_grid), 20);
    
    GtkWidget *quick_retrieval = gtk_label_new("Quick Retrieval (Trie/Hash Map): Requires Backend Implementation");
    gtk_grid_attach(GTK_GRID(filter_grid), quick_retrieval, 0, 0, 4, 1);

    GtkWidget *year_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(year_entry), "Filter by Year (e.g., 2023)");
    gtk_grid_attach(GTK_GRID(filter_grid), gtk_label_new("\xF0\x9F\x97\x93 Year:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), year_entry, 1, 1, 1, 1);

    GtkWidget *tag_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(tag_entry), "Filter by Tag");
    gtk_grid_attach(GTK_GRID(filter_grid), gtk_label_new("\xF0\x9F\x8F\xB7 Tag:"), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), tag_entry, 3, 1, 1, 1);
    
    gtk_box_pack_start(GTK_BOX(vbox), filter_grid, FALSE, FALSE, 10);

    GtkWidget *tree_view = setup_tree_view(); // Uses shared setup_tree_view and paper_list_store
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    return vbox;
}


GtkWidget *create_knowledge_graph_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Knowledge Graph</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *ref_graph_btn = gtk_button_new_with_label("\xF0\x9F\x94\x80 Build Reference Graph");
    GtkWidget *collab_graph_btn = gtk_button_new_with_label("\xF0\x9F\x91\xAC Build Author Collaboration Graph");
    GtkWidget *traversal_label = gtk_label_new("Graph Traversal: DFS/BFS (Requires Graph Engine)");
    
    gtk_style_context_add_class(gtk_widget_get_style_context(ref_graph_btn), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(collab_graph_btn), "secondary-action");

    gtk_box_pack_start(GTK_BOX(control_box), ref_graph_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), collab_graph_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), traversal_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), control_box, FALSE, FALSE, 0);


    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, -1, 400);
    
    gtk_style_context_add_class(gtk_widget_get_style_context(drawing_area), "graph-area");

    GtkWidget *graph_info = gtk_label_new(
        "\n\xF0\x9F\x8C\x9F Graph Visualization Place: \xF0\x9F\x8C\x9F\n"
        "Requires a rendering engine (like Cairo, OpenGL, or GtkSourceView/GraphViz integration)\n"
        "to display nodes and edges based on paper relationships."
    );
    
    GtkWidget *drawing_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(drawing_box), drawing_area, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(drawing_box), graph_info, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), drawing_box, TRUE, TRUE, 0);

    return vbox;
}