#include "research_organizer.h"
#include <string.h>

// --- GLOBAL VARIABLE ---
// This allows the buttons to find the list, no matter what.
GtkWidget *global_paper_list_view = NULL; 

// In papermanage.c

// --- Helper: Aggressively clean file paths ---
static void clean_path(gchar *path) {
    if (!path) return;
    size_t len = strlen(path);
    if (len == 0) return;

    // 1. Trim Trailing Whitespace & Newlines (The likely fix)
    while (len > 0 && (path[len-1] == ' ' || path[len-1] == '\n' || path[len-1] == '\r' || path[len-1] == '\t')) {
        path[len-1] = '\0';
        len--;
    }

    // 2. Remove surrounding quotes "..."
    if (len >= 2) {
        if ((path[0] == '"' && path[len-1] == '"') || 
            (path[0] == '\'' && path[len-1] == '\'')) {
            memmove(path, path + 1, len - 2);
            path[len - 2] = '\0';
        }
    }
}

// --- BUTTON HANDLER: Open File ---
void on_open_file_clicked(GtkButton *button, gpointer user_data) {
    if (!global_paper_list_view) {
        g_print("[Error]: Global list view not found!\n");
        return;
    }

    // 1. Get the selection directly from the list
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(global_paper_list_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        // 2. Get the ID from the selected row
        gint paper_id;
        gtk_tree_model_get(model, &iter, 0, &paper_id, -1);

        // 3. Find paper data
        Paper* p = find_paper_by_id(paper_id);
        if (!p) {
            show_message_dialog(GTK_WINDOW(main_window), "Error", "Paper data not found in memory.", GTK_MESSAGE_ERROR);
            return;
        }

        // 4. Clean and Debug the path
        gchar clean_path_str[512];
        strcpy(clean_path_str, p->filepath);
        clean_path(clean_path_str); // Remove quotes if they exist

        g_print("------------------------------------------------\n");
        g_print("[DEBUG] Selected Paper ID: %d\n", paper_id);
        g_print("[DEBUG] Original Path: '%s'\n", p->filepath);
        g_print("[DEBUG] Cleaned Path:  '%s'\n", clean_path_str);

        // 5. Validate Path
        if (strlen(clean_path_str) == 0) {
            show_message_dialog(GTK_WINDOW(main_window), "No File", "No file path is saved for this paper.", GTK_MESSAGE_WARNING);
            return;
        }

        // 6. Check if file exists
        if (!g_file_test(clean_path_str, G_FILE_TEST_EXISTS)) {
            gchar *msg = g_strdup_printf("File not found on disk:\n%s", clean_path_str);
            show_message_dialog(GTK_WINDOW(main_window), "File Missing", msg, GTK_MESSAGE_ERROR);
            g_free(msg);
            return;
        }

        // 7. Launch
        GError *error = NULL;
        gchar *uri = g_filename_to_uri(clean_path_str, NULL, &error);
        
        if (!uri) {
            g_print("[Error] URI Conversion failed: %s\n", error->message);
            g_error_free(error);
            return;
        }

        g_print("[DEBUG] Launching URI: %s\n", uri);
        if (!g_app_info_launch_default_for_uri(uri, NULL, &error)) {
            gchar *msg = g_strdup_printf("Windows could not open this file type:\n%s", error->message);
            show_message_dialog(GTK_WINDOW(main_window), "Launch Error", msg, GTK_MESSAGE_ERROR);
            g_error_free(error);
        }
        g_free(uri);
        g_print("------------------------------------------------\n");

    } else {
        show_message_dialog(GTK_WINDOW(main_window), "Selection", "Please click a row to select a paper.", GTK_MESSAGE_INFO);
    }
}

// --- Select File Dialog (For Adding New Papers) ---
void on_select_file_clicked(GtkButton *button, gpointer user_data) {
    GtkLabel *file_label = GTK_LABEL(user_data);
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Paper File",
                                                    GTK_WINDOW(main_window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_label_set_text(file_label, filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// --- Edit/Delete Dialog Handler ---
static void on_edit_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GtkWidget *content_area = gtk_dialog_get_content_area(dialog);
        GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
        GtkGrid *grid = GTK_GRID(children->data);
        g_list_free(children);

        GtkWidget *title_entry = gtk_grid_get_child_at(grid, 1, 0);
        GtkWidget *author_entry = gtk_grid_get_child_at(grid, 1, 1);
        GtkWidget *year_entry = gtk_grid_get_child_at(grid, 1, 2);
        GtkWidget *keywords_entry = gtk_grid_get_child_at(grid, 1, 3);
        
        const gchar *title = gtk_entry_get_text(GTK_ENTRY(title_entry));
        const gchar *author = gtk_entry_get_text(GTK_ENTRY(author_entry));
        int year = atoi(gtk_entry_get_text(GTK_ENTRY(year_entry)));
        const gchar *keywords = gtk_entry_get_text(GTK_ENTRY(keywords_entry));

        int paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(dialog), "edit-paper-id"));
        update_paper_details(paper_id, title, author, year, keywords);

        update_paper_list(NULL);
        refresh_dashboard_metrics();
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

// --- Edit/Delete Button Handler ---
void on_edit_delete_clicked(GtkButton *button, gpointer data) {
    const gchar *action = (const gchar *)data;
    
    // Use global selection here too
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(global_paper_list_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        show_message_dialog(GTK_WINDOW(main_window), "Error", "Please select a paper first.", GTK_MESSAGE_WARNING);
        return;
    }

    gint paper_id;
    gtk_tree_model_get(model, &iter, 0, &paper_id, -1);
    Paper* p = find_paper_by_id(paper_id);

    if (strcmp(action, "Delete") == 0) {
        delete_paper(p->title);
        update_paper_list(NULL);
        refresh_dashboard_metrics();
        show_message_dialog(GTK_WINDOW(main_window), "Success", "Paper deleted.", GTK_MESSAGE_INFO);
    } else if (strcmp(action, "Edit") == 0) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Paper Details",
                                                    GTK_WINDOW(main_window),
                                                    GTK_DIALOG_MODAL,
                                                    "_Cancel", GTK_RESPONSE_REJECT,
                                                    "_Save Changes", GTK_RESPONSE_ACCEPT,
                                                    NULL);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkGrid *grid = GTK_GRID(gtk_grid_new());
        gtk_grid_set_column_spacing(grid, 10);
        gtk_grid_set_row_spacing(grid, 10);
        gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
        
        GtkWidget *title_entry = gtk_entry_new();
        GtkWidget *author_entry = gtk_entry_new();
        GtkWidget *year_entry = gtk_entry_new();
        GtkWidget *keywords_entry = gtk_entry_new();

        gtk_entry_set_text(GTK_ENTRY(title_entry), p->title);
        gtk_entry_set_text(GTK_ENTRY(author_entry), p->author);
        gchar year_str[10]; 
        sprintf(year_str, "%d", p->year);
        gtk_entry_set_text(GTK_ENTRY(year_entry), year_str);
        gtk_entry_set_text(GTK_ENTRY(keywords_entry), p->keywords);

        gtk_grid_attach(grid, gtk_label_new("Title:"), 0, 0, 1, 1);
        gtk_grid_attach(grid, title_entry, 1, 0, 1, 1);
        gtk_grid_attach(grid, gtk_label_new("Author:"), 0, 1, 1, 1);
        gtk_grid_attach(grid, author_entry, 1, 1, 1, 1);
        gtk_grid_attach(grid, gtk_label_new("Year:"), 0, 2, 1, 1);
        gtk_grid_attach(grid, year_entry, 1, 2, 1, 1);
        gtk_grid_attach(grid, gtk_label_new("Keywords (comma-sep):"), 0, 3, 1, 1);
        gtk_grid_attach(grid, keywords_entry, 1, 3, 1, 1);
        
        g_object_set_data(G_OBJECT(dialog), "edit-paper-id", GINT_TO_POINTER(p->id));
        g_signal_connect(dialog, "response", G_CALLBACK(on_edit_dialog_response), NULL);

        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(grid));
        gtk_widget_show_all(dialog);
    }
}

// --- Add Paper Dialog Handler ---
static void on_add_paper_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GtkWidget *content_area = gtk_dialog_get_content_area(dialog);
        GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
        GtkGrid *grid = GTK_GRID(children->data);
        g_list_free(children);

        GtkWidget *title_entry = gtk_grid_get_child_at(grid, 1, 0);
        GtkWidget *author_entry = gtk_grid_get_child_at(grid, 1, 1);
        GtkWidget *year_entry = gtk_grid_get_child_at(grid, 1, 2);
        GtkWidget *keywords_entry = gtk_grid_get_child_at(grid, 1, 3);
        GtkLabel *file_label = g_object_get_data(G_OBJECT(dialog), "file-label");

        const gchar *title = gtk_entry_get_text(GTK_ENTRY(title_entry));
        const gchar *author = gtk_entry_get_text(GTK_ENTRY(author_entry));
        int year = atoi(gtk_entry_get_text(GTK_ENTRY(year_entry)));
        const gchar *keywords = gtk_entry_get_text(GTK_ENTRY(keywords_entry));
        const gchar *filepath = gtk_label_get_text(file_label);

        if (strlen(title) > 0 && strlen(author) > 0) {
            const gchar *path_to_save = g_str_equal(filepath, "(None selected)") ? "" : filepath;
            add_paper_from_gui(title, author, year, keywords, path_to_save);
            update_paper_list(NULL);
            refresh_dashboard_metrics();
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

// --- Add Paper Button Handler ---
void on_upload_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add New Paper", GTK_WINDOW(main_window), GTK_DIALOG_MODAL, "_Cancel", GTK_RESPONSE_REJECT, "_Add Paper", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_column_spacing(grid, 10);
    gtk_grid_set_row_spacing(grid, 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    gtk_grid_attach(grid, gtk_label_new("Title:"), 0, 0, 1, 1);
    gtk_grid_attach(grid, gtk_entry_new(), 1, 0, 1, 1);
    gtk_grid_attach(grid, gtk_label_new("Author:"), 0, 1, 1, 1);
    gtk_grid_attach(grid, gtk_entry_new(), 1, 1, 1, 1);
    gtk_grid_attach(grid, gtk_label_new("Year:"), 0, 2, 1, 1);
    gtk_grid_attach(grid, gtk_entry_new(), 1, 2, 1, 1);
    gtk_grid_attach(grid, gtk_label_new("Keywords (comma-sep):"), 0, 3, 1, 1);
    gtk_grid_attach(grid, gtk_entry_new(), 1, 3, 1, 1);

    gtk_grid_attach(grid, gtk_label_new("File:"), 0, 4, 1, 1);
    GtkWidget *file_button = gtk_button_new_with_label("Select Paper (PDF, DOCX)...");
    GtkWidget *file_label = gtk_label_new("(None selected)");
    g_signal_connect(file_button, "clicked", G_CALLBACK(on_select_file_clicked), file_label);
    gtk_grid_attach(grid, file_button, 1, 4, 1, 1);
    gtk_grid_attach(grid, file_label, 1, 5, 1, 1);
    
    g_object_set_data(G_OBJECT(dialog), "file-label", file_label);
    g_signal_connect(dialog, "response", G_CALLBACK(on_add_paper_dialog_response), NULL);

    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(grid));
    gtk_widget_show_all(dialog);
}

// --- Double Click / Row Activation Handler ---
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data) {
    // When you double click a row, this runs.
    // We can update the "Selected Paper" label in the Notes tab here.
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gint paper_id;
        gchar *title, *author, *keywords;
        gint year;
        gtk_tree_model_get(model, &iter, 0, &paper_id, 1, &title, 2, &author, 3, &year, 4, &keywords, -1);
        
        // Update the global variables needed for the Notes tab
        if (notes_text_view) {
            g_object_set_data(G_OBJECT(notes_text_view), "current-paper-id", GINT_TO_POINTER(paper_id));
            
            if (paper_title_label) {
                gchar *title_markup = g_strdup_printf("<span size='large' weight='bold'>Selected Paper: %s</span>", title);
                gtk_label_set_markup(GTK_LABEL(paper_title_label), title_markup);
                g_free(title_markup);
            }
            
            // Load notes
            gchar *notes_content = load_notes_for_paper(paper_id);
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(notes_text_view);
            gtk_text_buffer_set_text(buffer, notes_content, -1);
            g_free(notes_content);
        }
        g_free(title); g_free(author); g_free(keywords);
    }
}

// --- MISSING FUNCTION: Refreshes the list view ---
void update_paper_list(const gchar *search_query) {
    if (!paper_list_store) return;

    gtk_list_store_clear(paper_list_store);
    GtkTreeIter iter;
    gboolean searching = (search_query && search_query[0] != '\0');
    gchar *search_lower = NULL;

    if (searching) {
        search_lower = g_utf8_strdown(search_query, -1);
    }

    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            gboolean matches = TRUE;
            if (searching) {
                gchar *title_lower = g_utf8_strdown(current->title, -1);
                gchar *author_lower = g_utf8_strdown(current->author, -1);
                gchar *keywords_lower = g_utf8_strdown(current->keywords, -1);
                
                matches = (g_strrstr(title_lower, search_lower) ||
                           g_strrstr(author_lower, search_lower) ||
                           g_strrstr(keywords_lower, search_lower));
                
                g_free(title_lower); g_free(author_lower); g_free(keywords_lower);
            }

            if (matches) {
                gtk_list_store_append(paper_list_store, &iter);
                gtk_list_store_set(paper_list_store, &iter,
                                   0, current->id,
                                   1, current->title,
                                   2, current->author,
                                   3, current->year,
                                   4, current->keywords,
                                   -1);
            }
            current = current->next;
        }
    }
    
    if (search_lower) {
        g_free(search_lower);
    }
}


// --- PAGE CREATION ---
GtkWidget *create_paper_management_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    // LEFT SIDE: Tree View
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *list_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(list_title), "<span size='large' weight='bold'>My Papers</span>");
    gtk_box_pack_start(GTK_BOX(left_box), list_title, FALSE, FALSE, 0);
    
    // Create Tree View using global store
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(paper_list_store));
    
    // --- SAVE TO GLOBAL VARIABLE SO BUTTONS CAN FIND IT ---
    global_paper_list_view = tree_view; 

    g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
    
    const gchar *titles[] = {"ID", "Title", "Author", "Year", "Keywords"};
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        if (i == 0) gtk_tree_view_column_set_visible(column, FALSE); 
        if (i == 1) gtk_tree_view_column_set_expand(column, TRUE); 
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_box_pack_start(GTK_BOX(left_box), scrolled_window, TRUE, TRUE, 0);
    
    gtk_paned_pack1(GTK_PANED(paned), left_box, TRUE, FALSE);
    
    // RIGHT SIDE: Buttons
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 20);
    
    GtkWidget *manage_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(manage_title), "<span size='large' weight='bold'>Paper Actions</span>");
    gtk_box_pack_start(GTK_BOX(right_box), manage_title, FALSE, FALSE, 0);

    GtkWidget *upload_button = gtk_button_new_with_label("1. Add New Paper");
    GtkWidget *open_button = gtk_button_new_with_label("Open Paper File");
    GtkWidget *edit_button = gtk_button_new_with_label("2. Edit Metadata");
    GtkWidget *delete_button = gtk_button_new_with_label("3. Delete Selected Paper");
    
    gtk_style_context_add_class(gtk_widget_get_style_context(upload_button), "suggested-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(open_button), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(edit_button), "secondary-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(delete_button), "destructive-action");
    
    g_signal_connect(upload_button, "clicked", G_CALLBACK(on_upload_clicked), NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file_clicked), NULL);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_delete_clicked), (gpointer)"Edit");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_edit_delete_clicked), (gpointer)"Delete");
    
    gtk_box_pack_start(GTK_BOX(right_box), upload_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), open_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), edit_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), delete_button, FALSE, FALSE, 0);

    GtkWidget *detail_label = gtk_label_new("Select a paper on the left to view and edit its metadata.");
    gtk_box_pack_start(GTK_BOX(right_box), detail_label, TRUE, TRUE, 0);
    
    gtk_paned_pack2(GTK_PANED(paned), right_box, FALSE, FALSE);

    return vbox;
}