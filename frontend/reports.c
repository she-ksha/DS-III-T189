#include "research_organizer.h"

// --- MODIFIED: This is the central handler for this page ---
void on_report_export_clicked(GtkButton *button, gpointer data) {
    const gchar *action = (const gchar *)data;

    if (strcmp(action, "Keyword Frequency Report") == 0) {
        // --- REAL BACKEND CALL ---
        gchar *report_string = get_keyword_frequency_report_string();
        show_message_dialog(GTK_WINDOW(main_window), "Keyword Report", report_string, GTK_MESSAGE_INFO);
        g_free(report_string);
    } 
    else if (strcmp(action, "CSV/PDF Export") == 0) {
        // --- REAL BACKEND CALL ---
        export_to_csv();
        show_message_dialog(GTK_WINDOW(main_window), "Export Complete", 
            "All data saved to 'organizer_data.csv'", GTK_MESSAGE_INFO);
    }
    else {
        // Mock for other buttons
        gchar *message = g_strdup_printf("Mock: %s not yet implemented.", action);
        show_message_dialog(GTK_WINDOW(main_window), "Not Implemented", message, GTK_MESSAGE_INFO);
        g_free(message);
    }
}

// --- Page Creation (Modified to connect to the *real* citation button) ---
GtkWidget *create_reports_export_page() {
    // (This code is identical to your provided ui_collaboration.c)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Reports and Export</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    GtkWidget *gen_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *citation_btn = gtk_button_new_with_label("Generate Citation (Selected Paper)");
    // --- REAL SIGNAL HANDLER (from collaboration.c) ---
    g_signal_connect(citation_btn, "clicked", G_CALLBACK(on_generate_citation_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(gen_box), citation_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gen_box, FALSE, FALSE, 0);

    GtkWidget *export_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *export_csv_btn = gtk_button_new_with_label("Export All Data (CSV)");
    // --- REAL SIGNAL HANDLER ---
    g_signal_connect(export_csv_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"CSV/PDF Export");
    gtk_box_pack_start(GTK_BOX(export_box), export_csv_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), export_box, FALSE, FALSE, 0);
    
    GtkWidget *analysis_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *keyword_freq_btn = gtk_button_new_with_label("Keyword Frequency Report");
    // --- REAL SIGNAL HANDLER ---
    g_signal_connect(keyword_freq_btn, "clicked", G_CALLBACK(on_report_export_clicked), (gpointer)"Keyword Frequency Report");
    gtk_box_pack_start(GTK_BOX(analysis_box), keyword_freq_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), analysis_box, FALSE, FALSE, 0);

    return vbox;
}