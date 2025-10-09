#include "all_in_one.h" // Needed for Paper struct, Hash Map, and Trie functions

// --- Helper: Display Paper Details ---
void display_paper_details(Paper* p) {
    if (!p) return;
    printf("\n--- Found Paper (ID: %d) ---\n", p->id);
    printf("  Title: %s\n", p->title);
    printf("  Author: %s\n", p->author);
    printf("  Keywords: %s\n", p->keywords);
    printf("  Year: %d\n", p->year);
    printf("---------------------------\n");
}

// --- 3.1: Quick Retrieval (Hash Map) ---
void quick_retrieval_by_title() {
    char search_title[MAX_TITLE_LEN];
    printf("\nEnter EXACT Title for Quick Retrieval: ");
    scanf(" %255[^\n]", search_title);

    // This uses the O(1) average time Hash Map lookup
    Paper* found_paper = retrieve_paper_by_title(search_title); 

    if (found_paper) {
        printf("✅ Quick Retrieval Success!\n");
        display_paper_details(found_paper);
    } else {
        printf("❌ Paper with title '%s' not found in Hash Map.\n", search_title);
    }
}

// --- 3.2: Autocomplete (Trie) ---
void autocomplete_search_menu() {
    char prefix[MAX_TITLE_LEN];
    printf("\nEnter prefix to search (e.g., 'Rese'): ");
    scanf(" %255s", prefix);

    // This uses the O(L) Trie lookup (L=length of prefix)
    handle_autocomplete(title_trie_root, prefix);
}

// --- 3.3: Search by Title/Author/Keyword (Simplified Full Search) ---
void full_search_menu() {
    char search_term[MAX_TITLE_LEN];
    printf("\nEnter search term (Title/Author/Keyword): ");
    scanf(" %255[^\n]", search_term);
    
    printf("\n--- Full Search Results for '%s' ---\n", search_term);
    int count = 0;
    
    // We iterate through the entire Hash Map (slower than Trie, but comprehensive)
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            // Check if the search term is present in title, author, or keywords
            if (strstr(current->title, search_term) || 
                strstr(current->author, search_term) ||
                strstr(current->keywords, search_term)) 
            {
                display_paper_details(current);
                count++;
            }
            current = current->next;
        }
    }
    
    if (count == 0) {
        printf("No papers found matching the search term.\n");
    } else {
        printf("\nTotal results found: %d\n", count);
    }
}

// --- 3.4: Filters (Year, Tag) (Simplified Year Filter) ---
void filter_by_year() {
    int filter_year;
    printf("\nEnter Year to filter by: ");
    if (scanf("%d", &filter_year) != 1) {
        printf("Invalid year input.\n");
        while (getchar() != '\n');
        return;
    }
    
    printf("\n--- Filtered Results for Year %d ---\n", filter_year);
    int count = 0;
    
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            if (current->year == filter_year) {
                display_paper_details(current);
                count++;
            }
            current = current->next;
        }
    }
    
    if (count == 0) {
        printf("No papers found from year %d.\n", filter_year);
    }
}


// --- Main Handler for Search and Filter Module ---
void handle_search_and_filter() {
    int choice = 0;
    
    while (choice != 5) {
        printf("\n--- 🔍 SEARCH AND FILTER MENU ---\n");
        printf("1. Quick Retrieval (by EXACT Title - Hash Map)\n");
        printf("2. Autocomplete Suggestions (by Title Prefix - Trie)\n");
        printf("3. Full Search (Title/Author/Keyword)\n");
        printf("4. Filter Papers (by Year)\n");
        printf("5. Exit to Main Menu\n");
        printf("Choose an option: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Clear input buffer
            while (getchar() != '\n'); 
            choice = 0; 
            continue;
        }

        switch (choice) {
            case 1: quick_retrieval_by_title(); break;
            case 2: autocomplete_search_menu(); break;
            case 3: full_search_menu(); break;
            case 4: filter_by_year(); break;
            case 5: printf("[MODULE]: Exiting Search and Filter.\n"); break;
            default: printf("Invalid choice. Please select 1-5.\n");
        }
    }
}