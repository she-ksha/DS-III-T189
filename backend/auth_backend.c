#include "research_organizer.h"

#define USERS_FILE "users.csv"
#define MAX_USERS 50
#define MAX_NAME_LEN 100
#define MAX_PASS_LEN 100

// --- In-Memory User Database ---
typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_PASS_LEN];
} User;

static User user_database[MAX_USERS];
static int num_users = 0;

// --- Global Variable Definitions ---
// The currently logged-in user
gchar *current_user_id = NULL; 


/**
 * @brief Saves the in-memory user list to 'users.csv'
 * This is called by create_user()
 */
static void save_users() {
    FILE *file = fopen(USERS_FILE, "w");
    if (!file) {
        g_print("FATAL ERROR: Could not open %s for writing.\n", USERS_FILE);
        return;
    }
    // Write header
    fprintf(file, "Username,Password\n");
    for (int i = 0; i < num_users; i++) {
        fprintf(file, "%s,%s\n", user_database[i].username, user_database[i].password);
    }
    fclose(file);
}

/**
 * @brief Loads all users from 'users.csv' into memory
 * This is called once when the application starts.
 */
void load_users() {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        g_print("[Auth]: %s not found. Will be created on new user registration.\n", USERS_FILE);
        return;
    }

    char line[MAX_NAME_LEN + MAX_PASS_LEN + 2];
    fgets(line, sizeof(line), file); // Skip header row

    while (fgets(line, sizeof(line), file) && num_users < MAX_USERS) {
        line[strcspn(line, "\n")] = 0; // Remove newline

        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        if (username && password) {
            strcpy(user_database[num_users].username, username);
            strcpy(user_database[num_users].password, password);
            num_users++;
        }
    }
    fclose(file);
    g_print("[Auth]: Loaded %d users from %s.\n", num_users, USERS_FILE);
}

/**
 * @brief Creates a new user and saves to 'users.csv'
 */
gboolean create_user(const gchar *username, const gchar *password) {
    if (num_users >= MAX_USERS) {
        g_print("Auth Error: Max users reached.\n");
        return FALSE; // Database full
    }

    // Check if user is already taken
    for (int i = 0; i < num_users; i++) {
        if (strcmp(user_database[i].username, username) == 0) {
            g_print("Auth Error: Username '%s' is taken.\n", username);
            return FALSE; // Username taken
        }
    }

    // Add new user to in-memory list
    strcpy(user_database[num_users].username, username);
    strcpy(user_database[num_users].password, password);
    num_users++;

    // Save the entire list back to the file
    save_users();
    
    g_print("Auth Success: User '%s' created.\n", username);
    return TRUE;
}

/**
 * @brief Authenticates a user against the in-memory database
 */
gboolean authenticate_user(const gchar *username, const gchar *password) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(user_database[i].username, username) == 0 && 
            strcmp(user_database[i].password, password) == 0) 
        {
            if (current_user_id) g_free(current_user_id);
            current_user_id = g_strdup(username);
            
            g_print("Auth Success: User '%s' logged in.\n", username);
            //g_print("[System]: Database file set to '%s'\n", database_filename);
            return TRUE;
        }
    }

    g_print("Auth Error: Invalid username or password for '%s'.\n", username);
    return FALSE;
}