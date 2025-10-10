#include "research_organizer.h"

// --- Global Widget Definitions 
GtkListStore *paper_list_store;



GtkWidget *setup_tree_view() {
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

void update_paper_list(const gchar *search_query) {
    gtk_list_store_clear(paper_list_store);
    if (!current_user_id) return;

    // --- MOCK DATA  ---
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

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gint paper_id;
    gchar *title, *author, *topic, *keywords;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &paper_id, 1, &title, 2, &author, 3, &topic, 4, &keywords, -1);        
        // 1. Update Paper Title Display 
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



void on_edit_delete_clicked(GtkButton *button, gpointer data) {
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

void on_upload_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Upload Research Paper", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL,"Select", GTK_RESPONSE_ACCEPT, NULL);

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



GtkWidget *create_paper_management_page() {
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