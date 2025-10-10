#ifndef RESEARCH_ORGANIZER_H
#define RESEARCH_ORGANIZER_H

#include <gtk/gtk.h>
#include <gio/gio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// --- Global Variables 
extern gchar *current_user_id; 
extern GtkWidget *main_window; 
extern GtkApplication *global_app; 

// Widgets 
extern GtkListStore *paper_list_store;
extern GtkTextView *notes_text_view;
extern GtkWidget *paper_title_label;


extern const gchar *APP_CSS; 

// --- Function Prototypes 

// mock
gboolean create_user(const gchar *username, const gchar *password);
gboolean authenticate_user(const gchar *username, const gchar *password);
gboolean insert_paper(const gchar *title, const gchar *author, const gchar *topic, const gchar *keywords, const gchar *file_path);
gboolean save_paper_notes(gint paper_id, const gchar *notes);
gchar *load_paper_notes(gint paper_id);

// window
void create_main_app_window();
void show_message_dialog(const gchar *title, const gchar *message, GtkMessageType type);
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void on_exit_clicked(GtkButton *button, gpointer data);

// pmanage and search
void update_paper_list(const gchar *search_query);
GtkWidget *setup_tree_view();
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);
void on_upload_clicked(GtkButton *button, gpointer user_data);

// From dashboard
GtkWidget *create_dashboard_page();
void on_report_export_clicked(GtkButton *button, gpointer data);

// From pmanage
GtkWidget *create_paper_management_page();

// From search
GtkWidget *create_search_filter_page();
GtkWidget *create_knowledge_graph_page();
void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data);

// collab
GtkWidget *create_collaboration_tools_page();
GtkWidget *create_reports_export_page();
GtkWidget *create_exit_program_page();
void on_save_notes_clicked(GtkButton *button, gpointer user_data);
void on_generate_citation_clicked(GtkButton *button, gpointer user_data);

// From login
void create_login_window(GtkApplication *app);

#endif //has all the header files and function prototypes 