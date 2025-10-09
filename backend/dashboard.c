#include "all_in_one.h" // Needed for the Paper struct and paper_storage array

// --- Helper: Count Total Papers ---
// This function traverses the Hash Map linked lists to get the total count.
int get_total_papers() {
    int count = 0;
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            count++;
            current = current->next;
        }
    }
    return count;
}

// --- Main Handler for Dashboard Module ---
void handle_dashboard() {
    printf("\n--- 🖥️ DASHBOARD ---\n");
    int total_papers = get_total_papers();
    
    printf("Total Papers in Organizer: %d\n", total_papers);
    
    if (total_papers > 0) {
        printf("\nQuick Stats:\n");
        
        // Calculate the Hash Map load percentage (simple gauge of storage use)
        int map_load_percent = (total_papers * 100) / HASH_MAP_SIZE;
        printf(" - Hash Map Load: %d%% (%d papers / %d slots)\n", 
               map_load_percent, total_papers, HASH_MAP_SIZE);
               
        printf(" - Suggested Action: Run Keyword Frequency Report (Option 6 -> 1) to analyze topics.\n");
    } else {
        printf("\nTip: Start by adding papers using the PAPER MANAGEMENT menu (Option 2) to unlock features.\n");
    }
}