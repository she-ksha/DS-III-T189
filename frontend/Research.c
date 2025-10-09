#include <gtk/gtk.h>
#include <gio/gio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


gchar *current_user_id = NULL; 
GtkWidget *main_window = NULL; 
GtkApplication *global_app = NULL; 

// Widgets
GtkListStore *paper_list_store;
GtkTextView *notes_text_view;
GtkWidget *paper_title_label;


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


// --- Mock Backend Functions  ---

static void init_app_environment() {
    g_print("Frontend Init: Mock environment initialized. (No database or file system setup).\n");
}

static gboolean create_user(const gchar *username, const gchar *password) {
    g_print("Frontend Mock: Attempting to create user: %s\n", username);
    return strcmp(username, "taken") != 0;
}

static gboolean authenticate_user(const gchar *username, const gchar *password) {
    g_print("Frontend Mock: Attempting to authenticate user: %s\n", username);
    if (strcmp(username, "testuser") == 0 && strcmp(password, "password") == 0) {
        if (current_user_id) g_free(current_user_id);
        current_user_id = g_strdup("1"); 
        return TRUE;
    }
    return FALSE;
}

static gboolean insert_paper(const gchar *title, const gchar *author, const gchar *topic, const gchar *keywords, const gchar *file_path) {
    g_print("Frontend Mock: Paper upload simulated.\n Title: %s\n Path: %s\n", title, file_path);
    return TRUE;
}

static gboolean save_paper_notes(gint paper_id, const gchar *notes) {
    g_print("Frontend Mock: Notes saved for paper ID %d. Notes length: %zu\n", paper_id, strlen(notes));
    return TRUE;
}

static gchar *load_paper_notes(gint paper_id) {
    g_print("Frontend Mock: Loading mock notes for paper ID %d.\n", paper_id);
    return g_strdup("This is a mock note for the selected paper. Annotations and highlights would appear here!");
}


// --- GTK UI Helper Functions ---

static void update_paper_list(const gchar *search_query) {
    gtk_list_store_clear(paper_list_store);
    if (!current_user_id) return;

    // --- MOCK DATA  ---
    GtkTreeIter iter;

    struct {
        gint id;
        const gchar *title;
        const gchar *author;
        const gchar *topic;
        const gchar *keywords;
    } mock_papers[] = {
        {101, "The Future of AI in Research", "A. Turing", "Computer Science", "AI, Machine Learning, Future"},
        {102, "Quantum Entanglement for Dummies", "E. Schrodinger", "Physics", "Quantum, Entanglement, Mechanics"},
        {103, "GTK 3.0 Development in C", "J. Gnome", "Software Engineering", "GTK, C, UI"},
        {104, "A Study of Placeholder Text", "Dr. L. Ipsum", "Linguistics", "Mock, Text, Placeholders"}
    };

    gint i;
    for (i = 0; i < G_N_ELEMENTS(mock_papers); i++) {
        if (!search_query || search_query[0] == '\0' || 
            g_strrstr(mock_papers[i].title, search_query) ||
            g_strrstr(mock_papers[i].author, search_query) ||
            g_strrstr(mock_papers[i].keywords, search_query)) {

            gtk_list_store_append(paper_list_store, &iter);
            gtk_list_store_set(paper_list_store, &iter, 0, mock_papers[i].id, 1, mock_papers[i].title,2, mock_papers[i].author,3, mock_papers[i].topic,4, mock_papers[i].keywords,-1);
        }
    }
}

static GtkWidget *setup_tree_view() {
    paper_list_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(paper_list_store));
    g_object_unref(paper_list_store);

    const gchar *titles[] = {"ID", "Title", "Author", "Topic", "Keywords"};
    gint i;
    for (i = 0; i < G_N_ELEMENTS(titles); i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);

        if (i == 0) {
            gtk_tree_view_column_set_visible(column, FALSE);
        } else {
            gtk_tree_view_column_set_resizable(column, TRUE);
        }

        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }
    

    GtkTreeViewColumn *title_column = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 1);
    gtk_tree_view_column_set_expand(title_column, TRUE);

    return tree_view;
}

static void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gint paper_id;
    gchar *title, *author, *topic, *keywords;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,0, &paper_id,  1, &title, 2, &author,3, &topic,4, &keywords,-1);
        
        // 1. Update Paper Title Display (used in Notes tab)
        gtk_label_set_text(GTK_LABEL(paper_title_label), title);

        // 2. Load Notes (Mocked)
        gchar *notes_content = load_paper_notes(paper_id);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(notes_text_view);
        gtk_text_buffer_set_text(buffer, notes_content, -1);
        g_free(notes_content);

        // Store data for Notes/Citation/Edit buttons
        g_object_set_data(G_OBJECT(notes_text_view), "current-paper-id", GINT_TO_POINTER(paper_id));
        g_object_set_data_full(G_OBJECT(notes_text_view), "current-paper-title", title, g_free); 
        g_object_set_data_full(G_OBJECT(notes_text_view), "current-paper-author", author, g_free);
        g_object_set_data_full(G_OBJECT(notes_text_view), "current-paper-keywords", keywords, g_free);
    }
}

// --- Signal Handlers ---

static void show_message_dialog(const gchar *title, const gchar *message, GtkMessageType type) {
     GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                               GTK_DIALOG_MODAL,
                                               type,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void on_edit_delete_clicked(GtkButton *button, gpointer data) {
    const gchar *action = (const gchar *)data;
    if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id")) == 0) {
        show_message_dialog("Error", "Please select a paper first.", GTK_MESSAGE_ERROR);
        return;
    }
    
    if (strcmp(action, "Edit") == 0) {
        show_message_dialog("Paper Management", "Mock Metadata Editor launched for selected paper.", GTK_MESSAGE_INFO);
    } else if (strcmp(action, "Delete") == 0) {
        show_message_dialog("Paper Management", "Mock: Paper deleted successfully.", GTK_MESSAGE_INFO);
        gtk_list_store_clear(paper_list_store);
        update_paper_list(NULL);
    }
}

static void on_exit_clicked(GtkButton *button, gpointer data) {
    g_application_quit(G_APPLICATION(global_app));
}

static void on_report_export_clicked(GtkButton *button, gpointer data) {
    gchar *message = g_strdup_printf("Mock: %s generated and exported.", (const gchar *)data);
    show_message_dialog("Reports & Export", message, GTK_MESSAGE_INFO);
    g_free(message);
}

static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    gtk_widget_hide(widget);
    return TRUE;
}

static void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data) {
    update_paper_list(gtk_entry_get_text(GTK_ENTRY(entry))); 
}

static void on_save_notes_clicked(GtkButton *button, gpointer user_data) {
    gint paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id"));
    if (paper_id == 0) {
        show_message_dialog("Error", "Please select a paper first to save notes.", GTK_MESSAGE_WARNING);
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(notes_text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *notes = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (save_paper_notes(paper_id, notes)) {
        show_message_dialog("Notes Saved", "Notes and annotations mock-saved successfully.", GTK_MESSAGE_INFO);
    }

    g_free(notes);
}

static void on_upload_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Upload Research Paper", GTK_WINDOW(main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   "Select", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Documents (*.pdf, *.docx, *.txt)");
    gtk_file_filter_add_pattern(filter, "*.pdf");
    gtk_file_filter_add_pattern(filter, "*.docx");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (insert_paper("Mock Title", "Mock Author", "Mock Topic", "Mock Keywords", filename)) {
            update_paper_list(NULL); 
            show_message_dialog("Upload Complete", "Paper metadata extracted and mock-saved.", GTK_MESSAGE_INFO);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_generate_citation_clicked(GtkButton *button, gpointer user_data) {
    gint paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id"));
    if (paper_id == 0) {
        show_message_dialog("Error", "Please select a paper first to generate a citation.", GTK_MESSAGE_WARNING);
        return;
    }

    gchar *mock_author = g_object_get_data(G_OBJECT(notes_text_view), "current-paper-author");
    gchar *title = g_object_get_data(G_OBJECT(notes_text_view), "current-paper-title");
    gchar *mock_title = title ? title : "Unknown Title";
            
    gchar *citation_text = g_strdup_printf("Citation (Simplified APA Style):\n\n%s. (n.d.). %s. Retrieved from mock archive.", mock_author, mock_title);
    
    if (citation_text) {
        show_message_dialog("Generated Citation", citation_text, GTK_MESSAGE_INFO);
        g_free(citation_text);
    }
}

static void on_switch_to_signup(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "signup_page");
}

static void on_switch_to_login(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "login_page");
}


// --- Main Application Window Pages ---


static GtkWidget *create_dashboard_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 25);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 30);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold' foreground='#3f51b5'>DASHBOARD</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget *stats_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stats_title), "<span size='large' weight='bold' foreground='#333333'>Key Metrics</span>");
    gtk_box_pack_start(GTK_BOX(vbox), stats_title, FALSE, FALSE, 15);

    GtkWidget *stats_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(stats_grid), 25);
    gtk_grid_set_column_spacing(GTK_GRID(stats_grid), 25);
    gtk_grid_set_column_homogeneous(GTK_GRID(stats_grid), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), stats_grid, FALSE, FALSE, 20);

    // --- Card 1: Total Papers ---
    GtkWidget *card1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(card1), 20);
    gtk_style_context_add_class(gtk_widget_get_style_context(card1), "dashboard-stat");
    GtkWidget *total_papers_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(total_papers_title), "<span size='x-large' weight='bold' foreground='#5c6bc0'>\xE2\x98\x85 Total Papers</span>");
    GtkWidget *total_papers_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(total_papers_value), "<span size='xx-large' weight='ultrabold' foreground='#333333'>4</span>");
    gtk_box_pack_start(GTK_BOX(card1), total_papers_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card1), total_papers_value, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(stats_grid), card1, 0, 0, 1, 1);

    // --- Card 2: Recently Uploaded ---
    GtkWidget *card2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(card2), 20);
    gtk_style_context_add_class(gtk_widget_get_style_context(card2), "dashboard-stat");
    GtkWidget *uploaded_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(uploaded_title), "<span size='x-large' weight='bold' foreground='#5c6bc0'>\xF0\x9F\x93\x84 Last Upload</span>");
    GtkWidget *uploaded_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(uploaded_value), "<span size='medium' foreground='#333333'>GTK 3.0 Development in C</span>");
    gtk_box_pack_start(GTK_BOX(card2), uploaded_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card2), uploaded_value, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(stats_grid), card2, 1, 0, 1, 1);

    // --- Card 3: Suggested Paper ---
    GtkWidget *card3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(card3), 20);
    gtk_style_context_add_class(gtk_widget_get_style_context(card3), "dashboard-stat");
    GtkWidget *suggested_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(suggested_title), "<span size='x-large' weight='bold' foreground='#5c6bc0'>\xF0\x9F\x94\x8D Suggested Read</span>");
    GtkWidget *suggested_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(suggested_value), "<span size='medium' foreground='#333333'><i>Parallel Processing in C</i></span>");
    gtk_box_pack_start(GTK_BOX(card3), suggested_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card3), suggested_value, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(stats_grid), card3, 2, 0, 1, 1);

    // --- Quick Action Section ---
    GtkWidget *action_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(action_title), "<span size='large' weight='bold' foreground='#333333'>Quick Actions</span>");
    gtk_box_pack_start(GTK_BOX(vbox), action_title, FALSE, FALSE, 25);
    
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    
    GtkWidget *upload_btn = gtk_button_new_with_label("\xE2\xAC\x86 Upload New Paper");
    gtk_style_context_add_class(gtk_widget_get_style_context(upload_btn), "suggested-action");
    g_signal_connect(upload_btn, "clicked", G_CALLBACK(on_upload_clicked), NULL);
    
    GtkWidget *search_btn = gtk_button_new_with_label("\xF0\x9F\x94\x8D Go to Search");
    gtk_style_context_add_class(gtk_widget_get_style_context(search_btn), "secondary-action");
    g_signal_connect(search_btn, "clicked", G_CALLBACK(show_message_dialog), "Mock: Navigating to Search Tab.");

    gtk_box_pack_start(GTK_BOX(action_box), upload_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_box), search_btn, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), action_box, FALSE, FALSE, 0);

    return vbox;
}


static GtkWidget *create_paper_management_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *list_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(list_title), "<span size='large' weight='bold'>My Papers</span>");
    gtk_box_pack_start(GTK_BOX(left_box), list_title, FALSE, FALSE, 0);
    
    GtkWidget *tree_view = setup_tree_view();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_box_pack_start(GTK_BOX(left_box), scrolled_window, TRUE, TRUE, 0);
    
    g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
    
    gtk_paned_pack1(GTK_PANED(paned), left_box, TRUE, FALSE);
    
 
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 20);
    
    GtkWidget *manage_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(manage_title), "<span size='large' weight='bold'>Paper Actions</span>");
    gtk_box_pack_start(GTK_BOX(right_box), manage_title, FALSE, FALSE, 0);

    GtkWidget *upload_button = gtk_button_new_with_label("\xE2\xAC\x86 1. Upload & Extract Metadata");
    GtkWidget *edit_button = gtk_button_new_with_label("\xE2\x9C\x8D 2. Edit Metadata / Tags");
    GtkWidget *delete_button = gtk_button_new_with_label("\xF0\x9F\x97\x91 3. Delete Paper");
    
    gtk_style_context_add_class(gtk_widget_get_style_context(upload_button), "suggested-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(edit_button), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(delete_button), "destructive-action");
    
    g_signal_connect(upload_button, "clicked", G_CALLBACK(on_upload_clicked), NULL);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_delete_clicked), (gpointer)"Edit");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_edit_delete_clicked), (gpointer)"Delete");
    
    gtk_box_pack_start(GTK_BOX(right_box), upload_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), edit_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), delete_button, FALSE, FALSE, 0);

    GtkWidget *detail_label = gtk_label_new("Select a paper on the left to view and edit its metadata.");
    gtk_box_pack_start(GTK_BOX(right_box), detail_label, TRUE, TRUE, 0);
    
    gtk_paned_pack2(GTK_PANED(paned), right_box, FALSE, FALSE);

    return vbox;
}

/**
 search and filter
 */
static GtkWidget *create_search_filter_page() {
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
    gtk_grid_attach(GTK_GRID(filter_grid), quick_retrieval, 0, 0, 2, 1);

    GtkWidget *year_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(year_entry), "Filter by Year (e.g., 2023)");
    gtk_grid_attach(GTK_GRID(filter_grid), gtk_label_new("\xF0\x9F\x97\x93 Year:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), year_entry, 1, 1, 1, 1);

    GtkWidget *tag_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(tag_entry), "Filter by Tag");
    gtk_grid_attach(GTK_GRID(filter_grid), gtk_label_new("\xF0\x9F\x8F\xB7 Tag:"), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), tag_entry, 3, 1, 1, 1);
    
    gtk_box_pack_start(GTK_BOX(vbox), filter_grid, FALSE, FALSE, 10);

    GtkWidget *tree_view = setup_tree_view();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    return vbox;
}

/**
 Knowledge Graph tab.
 */
static GtkWidget *create_knowledge_graph_page() {
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

/**
 Collaboration Tools tab.
 */
static GtkWidget *create_collaboration_tools_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Notes &amp; Collaboration</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    paper_title_label = gtk_label_new("Selected Paper: None");
    gtk_label_set_markup(GTK_LABEL(paper_title_label), "<span size='large' weight='bold'>Selected Paper: None</span>");
    gtk_box_pack_start(GTK_BOX(vbox), paper_title_label, FALSE, FALSE, 0);

    notes_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(notes_text_view, GTK_WRAP_WORD);
    
    GtkWidget *notes_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(notes_scrolled_window), GTK_WIDGET(notes_text_view));
    gtk_box_pack_start(GTK_BOX(vbox), notes_scrolled_window, TRUE, TRUE, 0);

    GtkWidget *controls_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *save_button = gtk_button_new_with_label("\xF0\x9F\x93\x80 Save Notes/Annotations");
    gtk_style_context_add_class(gtk_widget_get_style_context(save_button), "suggested-action");
    
    GtkWidget *share_button = gtk_button_new_with_label("\xF0\x9F\x94\x83 Share Tagged Collection");
    GtkWidget *group_projects_button = gtk_button_new_with_label("\xF0\x9F\x91\xA5 View Group Project Graphs");
    gtk_style_context_add_class(gtk_widget_get_style_context(share_button), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(group_projects_button), "secondary-action");
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_notes_clicked), NULL);
    g_signal_connect(share_button, "clicked", G_CALLBACK(show_message_dialog), "Share Tagged Collection (Mock: Requires Networking)");
    g_signal_connect(group_projects_button, "clicked", G_CALLBACK(show_message_dialog), "Group Project Graphs (Mock: Requires Networking)");

    gtk_box_pack_start(GTK_BOX(controls_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(controls_box), share_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(controls_box), group_projects_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), controls_box, FALSE, FALSE, 0);

    return vbox;
}

/**
 Reports and Export tab.
 */
static GtkWidget *create_reports_export_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Reports and Export</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    GtkWidget *section_title_1 = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(section_title_1), "<span size='large' weight='bold'>\xF0\x9F\x92\xA1 Generation and Summaries</span>");
    gtk_box_pack_start(GTK_BOX(vbox), section_title_1, FALSE, FALSE, 10);

    GtkWidget *gen_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *summary_btn = gtk_button_new_with_label("\xF0\x9F\x98\x8A Generate Summaries (AI)");
    GtkWidget *citation_btn = gtk_button_new_with_label("\xF0\x9F\x93\x8B Generate Citation (Selected Paper)");
    gtk_style_context_add_class(gtk_widget_get_style_context(summary_btn), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(citation_btn), "secondary-action");

    g_signal_connect(summary_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"AI Summary");
    g_signal_connect(citation_btn, "clicked", G_CALLBACK(on_generate_citation_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(gen_box), summary_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gen_box), citation_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gen_box, FALSE, FALSE, 0);

    GtkWidget *section_title_2 = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(section_title_2), "<span size='large' weight='bold'>\xF0\x9F\x93\xA4 Export Options</span>");
    gtk_box_pack_start(GTK_BOX(vbox), section_title_2, FALSE, FALSE, 10);

    GtkWidget *export_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *export_csv_btn = gtk_button_new_with_label("\xF0\x9F\x93\x84 Export (CSV/PDF)");
    GtkWidget *export_graphml_btn = gtk_button_new_with_label("\xF0\x9F\x94\x8C Export (GraphML)");
    gtk_style_context_add_class(gtk_widget_get_style_context(export_csv_btn), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(export_graphml_btn), "secondary-action");

    g_signal_connect(export_csv_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"CSV/PDF Export");
    g_signal_connect(export_graphml_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"GraphML Export");
    gtk_box_pack_start(GTK_BOX(export_box), export_csv_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(export_box), export_graphml_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), export_box, FALSE, FALSE, 0);
    
    GtkWidget *section_title_3 = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(section_title_3), "<span size='large' weight='bold'>\xF0\x9F\x93\x88 Analysis Reports</span>");
    gtk_box_pack_start(GTK_BOX(vbox), section_title_3, FALSE, FALSE, 10);

    GtkWidget *analysis_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *keyword_freq_btn = gtk_button_new_with_label("\xF0\x9F\x97\x83 Keyword Frequency Report");
    GtkWidget *collab_charts_btn = gtk_button_new_with_label("\xF0\x9F\x92\x8C Collaboration Charts");
    gtk_style_context_add_class(gtk_widget_get_style_context(keyword_freq_btn), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(collab_charts_btn), "secondary-action");
    
    g_signal_connect(keyword_freq_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"Keyword Frequency Report");
    g_signal_connect(collab_charts_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"Collaboration Charts");
    gtk_box_pack_start(GTK_BOX(analysis_box), keyword_freq_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(analysis_box), collab_charts_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), analysis_box, FALSE, FALSE, 0);

    return vbox;
}

/**
  Exit Program tab.
 */
static GtkWidget *create_exit_program_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 50);

    GtkWidget *message = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(message), 
        "<span size='x-large' weight='bold'>Are you sure you want to exit the program?</span>"
    );
    gtk_box_pack_start(GTK_BOX(vbox), message, FALSE, FALSE, 0);

    GtkWidget *exit_button = gtk_button_new_with_label("\xE2\x9E\x94 EXIT PROGRAM NOW");
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(exit_button), "suggested-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(exit_button), "destructive-action");
    
    gtk_box_pack_start(GTK_BOX(vbox), exit_button, FALSE, FALSE, 0);

    return vbox;
}


static void create_main_app_window() {
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


// --- Login ---

static void on_login_btn_clicked(GtkButton *button, GtkStack *stack) {
    GtkWidget *parent_grid = gtk_widget_get_parent(GTK_WIDGET(button));
    GtkWidget *username_entry = g_object_get_data(G_OBJECT(parent_grid), "username-entry");
    GtkWidget *password_entry = g_object_get_data(G_OBJECT(parent_grid), "password-entry");
    GtkWidget *status_label = g_object_get_data(G_OBJECT(parent_grid), "status-label");

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (authenticate_user(username, password)) {
        GtkWidget *login_window = gtk_widget_get_toplevel(GTK_WIDGET(stack));
        
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='green'>Login Successful! Redirecting...</span>");
        
        create_main_app_window();
        
        gtk_widget_hide(login_window);
        
    } else {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Invalid Username or Password (Mock Failure).</span>");
    }
}

static void on_create_btn_clicked(GtkButton *button, GtkStack *stack) {
    GtkWidget *parent_grid = gtk_widget_get_parent(GTK_WIDGET(button));
    GtkWidget *username_entry = g_object_get_data(G_OBJECT(parent_grid), "username-entry");
    GtkWidget *password_entry = g_object_get_data(G_OBJECT(parent_grid), "password-entry");
    GtkWidget *status_label = g_object_get_data(G_OBJECT(parent_grid), "status-label");

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (strlen(username) < 3 || strlen(password) < 6) {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Username must be 3+ and password 6+ characters.</span>");
        return;
    }

    if (create_user(username, password)) {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='green'>Account Created! Please log in.</span>");
        gtk_stack_set_visible_child_name(stack, "login_page");
    } else {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Account Creation Failed (Username 'taken' or other mock error).</span>");
    }
}

static GtkWidget *create_login_ui(GtkStack *stack, gboolean is_login) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    
    gtk_widget_set_name(grid, "login-card");

    GtkWidget *title_label = gtk_label_new(is_login ? "User Login" : "Create Account");
    gtk_label_set_markup(GTK_LABEL(title_label), is_login ? "<span size='xx-large' weight='bold' foreground='#3f51b5'>User Login</span>" : "<span size='xx-large' weight='bold' foreground='#3f51b5'>Create Account</span>");
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 2, 1);

    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username (Try 'testuser')");
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Username:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), username_entry, 1, 1, 1, 1);

    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password (Try 'password')");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Password:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_entry, 1, 2, 1, 1);

    GtkWidget *status_label = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), status_label, 0, 3, 2, 1);

    GtkWidget *action_button = gtk_button_new_with_label(is_login ? "Login" : "Create Account");
    gtk_style_context_add_class(gtk_widget_get_style_context(action_button), "suggested-action");
    
    GtkWidget *switch_button = gtk_button_new_with_label(is_login ? "Need an Account? Sign Up" : "Already have an account? Log In");
    
    gtk_grid_attach(GTK_GRID(grid), action_button, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), switch_button, 0, 5, 2, 1);
    
    if (is_login) {
        g_signal_connect(action_button, "clicked", G_CALLBACK(on_login_btn_clicked), stack);
        g_signal_connect(switch_button, "clicked", G_CALLBACK(on_switch_to_signup), stack);
    } else {
        g_signal_connect(action_button, "clicked", G_CALLBACK(on_create_btn_clicked), stack);
        g_signal_connect(switch_button, "clicked", G_CALLBACK(on_switch_to_login), stack);
    }

    g_object_set_data(G_OBJECT(grid), "username-entry", username_entry);
    g_object_set_data(G_OBJECT(grid), "password-entry", password_entry);
    g_object_set_data(G_OBJECT(grid), "status-label", status_label);
    
    return grid;
}

static void create_login_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "USER LOGIN");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 450);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    g_signal_connect(window, "delete-event", G_CALLBACK(on_window_delete_event), NULL);


    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    GtkWidget *login_page = create_login_ui(GTK_STACK(stack), TRUE);
    GtkWidget *signup_page = create_login_ui(GTK_STACK(stack), FALSE);
    
    GtkWidget *center_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *center_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(center_vbox), stack, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(center_frame), center_vbox, TRUE, FALSE, 0);

    gtk_stack_add_named(GTK_STACK(stack), login_page, "login_page");
    gtk_stack_add_named(GTK_STACK(stack), signup_page, "signup_page");

    gtk_box_pack_start(GTK_BOX(main_box), center_frame, TRUE, TRUE, 50);

    gtk_widget_show_all(window);
}


// --- Main Application Flow ---

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
