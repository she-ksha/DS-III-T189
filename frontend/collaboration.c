#include "research_organizer.h"

// --- MODIFIED: Calls real backend function ---
void on_save_notes_clicked(GtkButton *button, gpointer user_data) {
    gint paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id"));
    if (paper_id == 0) {
        show_message_dialog(GTK_WINDOW(main_window), "Error", "Please select a paper first.", GTK_MESSAGE_WARNING);
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(notes_text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *notes = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // --- REAL BACKEND CALL ---
    save_notes_for_paper(paper_id, notes);
    export_to_csv(); // <-- ADD THIS LIN
    
    show_message_dialog(GTK_WINDOW(main_window), "Notes Saved", "Notes and annotations saved successfully.", GTK_MESSAGE_INFO);
    g_free(notes);
}

// --- MODIFIED: Calls real backend function ---
void on_generate_citation_clicked(GtkButton *button, gpointer user_data) {
    gint paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id"));
    if (paper_id == 0) {
        show_message_dialog(GTK_WINDOW(main_window), "Error", "Please select a paper first.", GTK_MESSAGE_WARNING);
        return;
    }
    
    // --- REAL BACKEND CALL ---
    Paper* p = find_paper_by_id(paper_id);
    if (!p) {
        show_message_dialog(GTK_WINDOW(main_window), "Error", "Could not find paper data.", GTK_MESSAGE_ERROR);
        return;
    }
    
    gchar *citation_text = g_strdup_printf("Citation (Simplified APA Style):\n\n%s. (%d). %s.", 
                                           p->author, 
                                           p->year, 
                                           p->title);
    
    show_message_dialog(GTK_WINDOW(main_window), "Generated Citation", citation_text, GTK_MESSAGE_INFO);
    g_free(citation_text);
}

// --- Page Creation (Unchanged from your file) ---
GtkWidget *create_collaboration_tools_page() {
    // (This code is identical to your provided ui_collaboration.c)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Notes &amp; Collaboration</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    // This global label is initialized here
    paper_title_label = gtk_label_new("Selected Paper: None");
    gtk_label_set_markup(GTK_LABEL(paper_title_label), "<span size='large' weight='bold'>Selected Paper: None</span>");
    gtk_box_pack_start(GTK_BOX(vbox), paper_title_label, FALSE, FALSE, 0);

    // This global text view is initialized here
    notes_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(notes_text_view, GTK_WRAP_WORD);
    
    GtkWidget *notes_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(notes_scrolled_window), GTK_WIDGET(notes_text_view));
    gtk_box_pack_start(GTK_BOX(vbox), notes_scrolled_window, TRUE, TRUE, 0);

    GtkWidget *controls_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *save_button = gtk_button_new_with_label("Save Notes/Annotations");
    gtk_style_context_add_class(gtk_widget_get_style_context(save_button), "suggested-action");
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_notes_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(controls_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), controls_box, FALSE, FALSE, 0);

    return vbox;
}