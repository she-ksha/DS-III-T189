#include "research_organizer.h"

// --- Signal Handlers ---

void on_report_export_clicked(GtkButton *button, gpointer data) {
    gchar *message = g_strdup_printf("Mock: %s generated and exported.", (const gchar *)data);
    show_message_dialog("Reports & Export", message, GTK_MESSAGE_INFO);
    g_free(message);
}

// --- UI Creation ---

GtkWidget *create_dashboard_page() {
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

    //   Total Papers -
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

    //   Recently Uploaded -
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

    //   Suggested Paper -
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

    //   Quick Action Section -
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