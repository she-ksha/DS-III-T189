#ifndef RESEARCH_ORGANIZER_H
#define RESEARCH_ORGANIZER_H
#define DATABASE_FILE "all_papers.csv"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For bool type
#include <cairo.h>

// --- Backend Data Structures ---

#define MAX_TITLE_LEN 256
#define MAX_AUTHOR_LEN 128
#define MAX_KEYWORDS_LEN 256
#define MAX_ANNOTATIONS 50
#define MAX_ANNOTATION_LEN 256
#define MAX_TAGS 20
#define MAX_TAG_LEN 50
#define MAX_REFERENCES 50
#define HASH_MAP_SIZE 100 // Size of the hash map array
// For the Paper struct
#define MAX_CITATIONS 50

// For the Graph structs
#define MAX_GRAPH_NODES 500
#define MAX_GRAPH_EDGES 1000
#define MAX_NAME_LEN 100

// Add this new function prototype
void load_users();

// Add this new global variable (replaces the #define)
extern gchar database_filename[256];

// Structure for a research paper
typedef struct Paper {
    int id;
    char title[MAX_TITLE_LEN];
    char author[MAX_AUTHOR_LEN];
    int year;
    char keywords[MAX_KEYWORDS_LEN];
    char annotations[MAX_ANNOTATIONS][MAX_ANNOTATION_LEN];
    int num_annotations;
    char tags[MAX_TAGS][MAX_TAG_LEN];
    int num_tags;
    // --- ADD THIS LINE ---
    char filepath[512]; // Stores the path to the PDF/DOCX file
    // --- END ADD ---
    // --- ADD THIS LINE ---
    char owner[MAX_AUTHOR_LEN]; // Stores the username of who added this
    // --- END ADD ---
    struct Paper* next;

    // --- NEW FIELD ---
    int cited_paper_ids[MAX_CITATIONS]; // e.g., an array of paper IDs it cites
    int num_citations;
} Paper;

// A node in our graph
typedef struct GraphNode {
    int id;
    gchar label[MAX_NAME_LEN];
    gchar type[20]; // "Paper", "Author", "Keyword"

    // For drawing
    double x, y; // Its X/Y position on the screen
    int papers_positioned;
    double r, g, b; // RGB color for drawing
} GraphNode;

// An edge connecting two nodes
typedef struct GraphEdge {
    int from_node_id;
    int to_node_id;
    gchar label[50]; // "CITES", "WROTE_BY"
} GraphEdge;

// The main graph structure
typedef struct KnowledgeGraph {
    GraphNode *nodes[MAX_GRAPH_NODES];
    GraphEdge *edges[MAX_GRAPH_EDGES];
    int num_nodes;
    int num_edges;
} KnowledgeGraph;

// --- Add these new global variables ---
extern KnowledgeGraph *global_graph; // The main graph
extern int g_graph_node_id_counter; // To give each node a unique ID

// --- Add this new function prototype ---
void build_knowledge_graph();
void build_citation_graph();

// --- Global Backend Variables ---
extern Paper* paper_storage[HASH_MAP_SIZE];
extern int g_paper_id_counter;


// --- Backend Core Functions (backend.c) ---
void init_paper_storage();
unsigned int hash(const char* key);
Paper* find_paper_by_id(int id_to_find);
Paper* retrieve_paper_by_title(const char* title);
void insert_paper(Paper* p);
bool delete_paper(const char* title);
// Add this new function declaration with your others
void update_paper_details(int id, const gchar *title, const gchar *author, int year, const gchar *keywords);

// --- Backend GUI-Friendly Functions (backend.c) ---
void add_paper_from_gui(const char* title, const char* author, int year, const char* keywords, const char* filepath);
// Add this new prototype (for the "Open File" button)
void on_open_file_clicked(GtkButton *button, gpointer user_data);

gchar* load_notes_for_paper(int paper_id);
void save_notes_for_paper(int paper_id, const char* all_notes);
int get_total_papers();
int get_hash_map_load_percent();
gchar* get_keyword_frequency_report_string();
gchar* get_dfs_traversal_string(const char* start_title);
gchar* get_bfs_traversal_string(const char* start_title);
bool add_reference_edge_from_gui(const char* source_title, const char* target_title);

// --- Backend Persistence (backend.c) ---
void import_from_csv();
void export_to_csv();

// --- Auth Functions (auth_backend.c) ---
gboolean create_user(const gchar *username, const gchar *password);
gboolean authenticate_user(const gchar *username, const gchar *password);
extern gchar *current_user_id; // <-- MISSING VARIABLE

// --- Global GTK Widgets ---
extern GtkApplication *global_app;
extern GtkWidget *main_window;
extern GtkListStore *paper_list_store;
extern GtkTextView *notes_text_view;
extern GtkWidget *paper_title_label;
extern GtkWidget *total_papers_value_label;
extern GtkWidget *hash_map_load_label;

// --- Global CSS ---
extern const gchar *APP_CSS;

// --- UI Window Creation ---
void create_login_window(GtkApplication *app);
void create_main_app_window();
GtkWidget *create_dashboard_page();
GtkWidget *create_paper_management_page();
GtkWidget *create_search_filter_page();
GtkWidget *create_knowledge_graph_page();
GtkWidget *create_collaboration_tools_page();
GtkWidget *create_reports_export_page();
GtkWidget *create_exit_program_page();

// --- UI Helpers / Callbacks ---
void show_message_dialog(GtkWindow *parent, const gchar *title, const gchar *message, GtkMessageType type);
void update_paper_list(const gchar *search_query);
void on_exit_clicked(GtkButton *button, gpointer data);
void refresh_dashboard_metrics();
// --- ADD THIS LINE to your .h file ---
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);

// --- ALL MISSING PROTOTYPES ---
void on_generate_citation_clicked(GtkButton *button, gpointer user_data); // For reports.c
void on_upload_clicked(GtkButton *button, gpointer user_data);          // For dashboard.c
GtkWidget *create_paper_tree_view();                                        // For search.c
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data); // For login.c

#endif // RESEARCH_ORGANIZER_H