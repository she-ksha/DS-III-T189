#include "all_in_one.h"
// --- Helper: Count all keywords in the system using another Hash Map ---
void generate_keyword_frequency_report() {
    // We will use a temporary Hash Map for counting keywords
    // For simplicity, we'll use a fixed-size array of keyword structures
    #define KEYWORD_MAP_SIZE 50 
    
    typedef struct KeywordCount {
        char word[50];
        int count;
        struct KeywordCount *next;
    } KeywordCount;

    KeywordCount* keyword_map[KEYWORD_MAP_SIZE] = {NULL};

    // Use the existing hash function for keyword counting
    
    // 1. Iterate through all papers
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            // Tokenize the keywords string by comma
            char temp_keywords[MAX_AUTHOR_LEN];
            strcpy(temp_keywords, current->keywords);
            
            char *token = strtok(temp_keywords, ", "); // Split by comma or space
            while (token != NULL) {
                // Ignore empty tokens
                if (strlen(token) > 1) { 
                    int index = hash(token) % KEYWORD_MAP_SIZE;

                    // 2. Look up/insert into Keyword Map
                    KeywordCount* kc_current = keyword_map[index];
                    bool found = false;
                    while (kc_current != NULL) {
                        if (strcmp(kc_current->word, token) == 0) {
                            kc_current->count++; // Found, increment count
                            found = true;
                            break;
                        }
                        kc_current = kc_current->next;
                    }

                    if (!found) {
                        // Not found, create new node and insert at head
                        KeywordCount* new_kc = (KeywordCount*)malloc(sizeof(KeywordCount));
                        if (new_kc) {
                            strcpy(new_kc->word, token);
                            new_kc->count = 1;
                            new_kc->next = keyword_map[index];
                            keyword_map[index] = new_kc;
                        }
                    }
                }
                token = strtok(NULL, ", ");
            }
            current = current->next;
        }
    }

    // 3. Print Report and Cleanup
    printf("\n--- 📈 Keyword Frequency Report ---\n");
    printf("%-20s %s\n", "KEYWORD", "COUNT");
    printf("-------------------------------\n");
    
    for (int i = 0; i < KEYWORD_MAP_SIZE; i++) {
        KeywordCount* current = keyword_map[i];
        while (current != NULL) {
            printf("%-20s %d\n", current->word, current->count);
            KeywordCount* temp = current;
            current = current->next;
            free(temp); // Free memory
        }
    }
}


// --- 6.2: Rule-Based NLP Summarization (Simulation) ---
void generate_summaries() {
    Paper* paper = find_paper_for_action();
    if (!paper) return;
    
    printf("\n--- 🤖 Rule-Based Summarization ---\n");
    printf("Paper: %s\n", paper->title);
    
    // SIMULATED SUMMARIZER: 
    // Rule 1: The title is always the first sentence.
    printf("\nSUMMARY:\n");
    printf("1. The primary topic of this research is: %s.\n", paper->title);
    
    // Rule 2: Extract key information (like author/year).
    printf("2. This work was authored by %s and published in %d.\n", paper->author, paper->year);

    // Rule 3: Mention the paper's tags and keywords.
    if (paper->num_tags > 0 || strlen(paper->keywords) > 0) {
        printf("3. It is categorized under the keywords '%s'.", paper->keywords);
        if (paper->num_tags > 0) {
            printf(" It has also been tagged with: [%s].\n", paper->tags[0]);
        } else {
            printf("\n");
        }
    }
    printf("\n(NOTE: A real extractive NLP summarizer would require advanced text processing.)\n");
}

// --- 6.3: Export to CSV (Simple) ---
void export_to_csv() {
    FILE *file = fopen("export_data.csv", "w");
    if (!file) {
        perror("Error creating export_data.csv");
        return;
    }

    // Write CSV Header
    fprintf(file, "ID,Title,Author,Year,Keywords,Citations,Tags\n");

    // Iterate through all papers and write to file
    int count = 0;
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            fprintf(file, "%d,\"%s\",\"%s\",%d,\"%s\",%d,", 
                current->id, 
                current->title, 
                current->author, 
                current->year, 
                current->keywords,
                current->num_cited);
            
            // Add tags
            for(int j=0; j < current->num_tags; j++) {
                fprintf(file, "%s%s", current->tags[j], (j < current->num_tags - 1) ? ";" : "");
            }
            fprintf(file, "\n");
            
            count++;
            current = current->next;
        }
    }

    fclose(file);
    printf("✅ Export successful! Total %d papers written to export_data.csv\n", count);
}


// --- Main Handler for Reports and Export Module ---
void handle_reports_and_export() {
    int choice = 0;

    while (choice != 4) {
        printf("\n--- 📊 REPORTS AND EXPORT MENU ---\n");
        printf("1. Generate Keyword Frequency Report\n");
        printf("2. Generate Rule-Based Summary\n");
        printf("3. Export All Data to CSV\n");
        printf("4. Exit to Main Menu\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            choice = 0; 
            continue;
        }

        switch (choice) {
            case 1: generate_keyword_frequency_report(); break;
            case 2: generate_summaries(); break;
            case 3: export_to_csv(); break;
            case 4: printf("[MODULE]: Exiting Reports and Export.\n"); break;
            default: printf("Invalid choice. Please select 1-4.\n");
        }
    }
}