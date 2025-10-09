#include "all_in_one.h"
#include <stdio.h>
#include <string.h>

#define CREDENTIALS_FILE "credentials.txt"

// --- Helper function to load credentials from file ---
int load_credentials(char* username, char* password) {
    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL) {
        return 0; // File does not exist
    }
    // Read the stored username and password
    if (fscanf(file, "%49s %49s", username, password) != 2) {
        fclose(file);
        return 0; // Error reading file format
    }
    fclose(file);
    return 1; // Credentials loaded successfully
}

// --- Implementation of the SIGNUP process ---
void handle_signup() {
    FILE *file = fopen(CREDENTIALS_FILE, "w");
    char new_username[50];
    char new_password[50];

    printf("\n--- ✍️ SIGN UP ---\n");
    printf("No existing user found. Creating a new account.\n");
    
    // Get new credentials from the user
    printf("Choose a Username (max 49 chars): ");
    if (scanf("%49s", new_username) != 1) return;
    printf("Choose a Password (max 49 chars): ");
    if (scanf("%49s", new_password) != 1) return;

    // Save credentials to the file
    fprintf(file, "%s %s", new_username, new_password);
    fclose(file);
    
    printf("✅ Account successfully created! Proceeding to Main Menu.\n");
}

// --- Implementation of the LOGIN process ---
int handle_login(const char* stored_username, const char* stored_password) {
    char input_username[50];
    char input_password[50];
    int attempts = 3;

    printf("\n--- 🔑 LOG IN ---\n");

    while (attempts > 0) {
        printf("Enter Username: ");
        if (scanf("%49s", input_username) != 1) return 0; 
        
        printf("Enter Password: ");
        if (scanf("%49s", input_password) != 1) return 0;

        // Compare input against stored credentials
        if (strcmp(input_username, stored_username) == 0 && strcmp(input_password, stored_password) == 0) {
            printf("\n✅ Login successful! Welcome.\n");
            return 1; // Success
        } else {
            attempts--;
            printf("❌ Invalid credentials. Attempts left: %d\n", attempts);
        }
    }
    printf("🚫 Max attempts reached. Exiting program.\n");
    return 0; // Failure
}


// --- Main LOGIN/SIGNUP handler function ---
int handle_login_signup() {
    char stored_username[50];
    char stored_password[50];

    // Try to load existing credentials
    if (load_credentials(stored_username, stored_password)) {
        // Credentials found, proceed to LOGIN
        return handle_login(stored_username, stored_password);
    } else {
        // No credentials found, proceed to SIGNUP
        handle_signup();
        // After successful signup, the user is automatically authenticated
        return 1; 
    }
}