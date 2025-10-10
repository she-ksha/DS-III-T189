#include "research_organizer.h"

// --- Mock Backend Functions  ---

gboolean create_user(const gchar *username, const gchar *password) {
    g_print("Frontend Mock: Attempting to create user: %s\n", username);
    return strcmp(username, "taken") != 0;
}

gboolean authenticate_user(const gchar *username, const gchar *password) {
    g_print("Frontend Mock: Attempting to authenticate user: %s\n", username);
    if (strcmp(username, "testuser") == 0 && strcmp(password, "password") == 0) {
        if (current_user_id) g_free(current_user_id);
        current_user_id = g_strdup("1"); 
        return TRUE;
    }
    return FALSE;
}

gboolean insert_paper(const gchar *title, const gchar *author, const gchar *topic, const gchar *keywords, const gchar *file_path) {
    g_print("Frontend Mock: Paper upload simulated.\n Title: %s\n Path: %s\n", title, file_path);
    return TRUE;
}

gboolean save_paper_notes(gint paper_id, const gchar *notes) {
    g_print("Frontend Mock: Notes saved for paper ID %d. Notes length: %zu\n", paper_id, strlen(notes));
    return TRUE;
}

gchar *load_paper_notes(gint paper_id) {
    g_print("Frontend Mock: Loading mock notes for paper ID %d.\n", paper_id);
    return g_strdup("This is a mock note for the selected paper. Annotations and highlights would appear here!");
}