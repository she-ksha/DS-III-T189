#include "research_organizer.h"

GtkTextView *notes_text_view;
GtkWidget *paper_title_label;



void on_save_notes_clicked(GtkButton *button, gpointer user_data) {
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

void on_generate_citation_clicked(GtkButton *button, gpointer user_data) {
    gint paper_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(notes_text_view), "current-paper-id"));
    if (paper_id == 0) {
        show_message_dialog("Error", "Please select a paper first to generate a citation.", GTK_MESSAGE_WARNING);
        return;
    }

    // Get the dynamically allocated strings from the data
    gchar *mock_author = g_object_get_data(G_OBJECT(notes_text_view), "current-paper-author");
    gchar *title = g_object_get_data(G_OBJECT(notes_text_view), "current-paper-title");
    gchar *mock_title = title ? title : "Unknown Title";
             
    gchar *citation_text = g_strdup_printf("Citation (Simplified APA Style):\n\n%s. (n.d.). %s. Retrieved from mock archive.", mock_author, mock_title);
    
    if (citation_text) {
        show_message_dialog("Generated Citation", citation_text, GTK_MESSAGE_INFO);
        g_free(citation_text);
    }
    
}


GtkWidget *create_collaboration_tools_page() {
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

GtkWidget *create_reports_export_page() {
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

GtkWidget *create_exit_program_page() {
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