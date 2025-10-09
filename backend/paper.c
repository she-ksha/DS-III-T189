#include "all_in_one.h" // Needed for Paper struct and Hash Map functions

// --- Global counter for unique paper IDs ---
static int next_paper_id = 1;

// --- Helper: Get Metadata from User Input ---
void input_paper_metadata(Paper* p, char* operation) {
    // Note: In a real system, you'd handle input buffer cleaning better.
    // For now, assume simple string inputs.
    
    printf("\n--- %s Paper Metadata ---\n", operation);
    
    // For Add operation, we need a new ID
    if (strcmp(operation, "ADD") == 0) {
        p->id = next_paper_id++;
    } else if (strcmp(operation, "EDIT") == 0) {
        // When editing, the ID is already set.
        printf("Editing Paper ID: %d\n", p->id);
    }

    printf("Enter Title (required for unique key): ");
    scanf(" %255[^\n]", p->title); // Reads string including spaces
    
    printf("Enter Author(s): ");
    scanf(" %127[^\n]", p->author);
    
    printf("Enter Keywords (comma-separated): ");
    scanf(" %255[^\n]", p->keywords);
    
    printf("Enter Year: ");
    if (scanf("%d", &p->year) != 1) {
        printf("Invalid year input, setting to 0.\n");
        // Clear input buffer
        while (getchar() != '\n'); 
        p->year = 0;
    }
    
    // Initialize citation counts to zero
    p->num_cited = 0;
}

// --- 2.1: ADD Paper ---
void add_paper() {
    Paper* new_paper = (Paper*)malloc(sizeof(Paper));
    if (new_paper == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new paper.\n");
        return;
    }
    // Set default next pointer for Hash Map list
    new_paper->next = NULL; 
    
    input_paper_metadata(new_paper, "ADD");
    
    // Use the Hash Map function from data_structures.c
    insert_paper(new_paper);
}

// --- 2.2: EDIT Paper ---
void edit_paper() {
    char title_to_edit[MAX_TITLE_LEN];
    printf("\nEnter Title of paper to EDIT: ");
    scanf(" %255[^\n]", title_to_edit);
    
    Paper* paper_to_edit = retrieve_paper_by_title(title_to_edit);
    
    if (paper_to_edit) {
        // Found the paper. Now collect new data.
        input_paper_metadata(paper_to_edit, "EDIT");
        printf("✅ Paper '%s' successfully updated.\n", paper_to_edit->title);
    } else {
        printf("❌ Paper with title '%s' not found.\n", title_to_edit);
    }
}

// --- 2.3: DELETE Paper ---
void delete_paper_menu() {
    char title_to_delete[MAX_TITLE_LEN];
    printf("\nEnter Title of paper to DELETE: ");
    scanf(" %255[^\n]", title_to_delete);
    
    // Use the Hash Map deletion function from data_structures.c
    delete_paper(title_to_delete);
}

// --- 2.4: Upload and Extract Metadata (Simplified Simulation) ---
// Note: Actual extraction is complex (requires parsing PDF/text structure).
// Here, we simulate by reading a sample file's contents.
void upload_and_extract_metadata() {
    printf("\n--- 📤 Upload and Extract Metadata (Simulation) ---\n");
    printf("Simulating upload and extraction from a sample file...\n");
    
    // Create a simulated paper structure
    Paper* extracted_paper = (Paper*)malloc(sizeof(Paper));
    if (extracted_paper == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return;
    }
    
    // SIMULATED EXTRACTION:
    extracted_paper->id = next_paper_id++;
    strcpy(extracted_paper->title, "Simulated Paper on NLP and C");
    strcpy(extracted_paper->author, "Team DS-III-T189");
    strcpy(extracted_paper->keywords, "NLP, C, Summarizer");
    extracted_paper->year = 2025;
    extracted_paper->num_cited = 0;
    extracted_paper->next = NULL;
    
    printf("✅ Extracted metadata:\n");
    printf("   Title: %s\n", extracted_paper->title);
    printf("   Author: %s\n", extracted_paper->author);
    
    // Insert the extracted paper into the Hash Map
    insert_paper(extracted_paper);
}

// --- Main Handler for Paper Management Module ---
void handle_paper_management() {
    int choice = 0;
    
    while (choice != 5) {
        printf("\n--- 📑 PAPER MANAGEMENT MENU ---\n");
        printf("1. Add New Paper (Manual Input)\n");
        printf("2. Edit Paper Metadata\n");
        printf("3. Delete Paper\n");
        printf("4. Upload & Extract Metadata (Simulated)\n");
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
            case 1: add_paper(); break;
            case 2: edit_paper(); break;
            case 3: delete_paper_menu(); break;
            case 4: upload_and_extract_metadata(); break;
            case 5: printf("[MODULE]: Exiting Paper Management.\n"); break;
            default: printf("Invalid choice. Please select 1-5.\n");
        }
    }
}