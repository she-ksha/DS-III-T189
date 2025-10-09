#include "all_in_one.h"

// --- Helper: Find Paper for Action ---
Paper* find_paper_for_action() {
    char title_to_find[MAX_TITLE_LEN];
    printf("Enter Title of the paper: ");
    scanf(" %255[^\n]", title_to_find);
    
    Paper* paper = retrieve_paper_by_title(title_to_find);
    if (!paper) {
        printf("❌ Paper '%s' not found.\n", title_to_find);
    }
    return paper;
}

// --- 5.1: Add Annotation (Note/Highlight) ---
void add_annotation() {
    Paper* paper = find_paper_for_action();
    if (!paper) return;

    if (paper->num_annotations >= MAX_ANNOTATIONS) {
        printf("Warning: Maximum annotations (%d) reached for this paper.\n", MAX_ANNOTATIONS);
        return;
    }

    printf("Enter Annotation text: ");
    // Write directly into the next available annotation slot
    scanf(" %255[^\n]", paper->annotations[paper->num_annotations]);
    paper->num_annotations++;
    printf("✅ Annotation added to '%s'.\n", paper->title);
}

// --- 5.2: Add Tag ---
void add_tag() {
    Paper* paper = find_paper_for_action();
    if (!paper) return;

    if (paper->num_tags >= MAX_TAGS) {
        printf("Warning: Maximum tags (%d) reached for this paper.\n", MAX_TAGS);
        return;
    }

    printf("Enter Tag (e.g., 'AI', 'Review', 'Team-A'): ");
    // Write directly into the next available tag slot
    scanf(" %49s", paper->tags[paper->num_tags]);
    paper->num_tags++;
    printf("✅ Tag added to '%s'.\n", paper->title);
}

// --- 5.3: Display Collaboration Data ---
void view_collaboration_data() {
    Paper* paper = find_paper_for_action();
    if (!paper) return;

    printf("\n--- Collaboration Data for %s ---\n", paper->title);
    
    printf("Tags (%d/%d): ", paper->num_tags, MAX_TAGS);
    for (int i = 0; i < paper->num_tags; i++) {
        printf("[%s]%s", paper->tags[i], (i == paper->num_tags - 1) ? "" : ", ");
    }
    printf("\n\nAnnotations (%d/%d):\n", paper->num_annotations, MAX_ANNOTATIONS);
    for (int i = 0; i < paper->num_annotations; i++) {
        printf(" %d. %s\n", i + 1, paper->annotations[i]);
    }
}

// --- Main Handler for Collaboration Module ---
void handle_collaboration_tools() {
    int choice = 0;

    while (choice != 4) {
        printf("\n--- 🤝 COLLABORATION TOOLS MENU ---\n");
        printf("1. Add Annotation/Note\n");
        printf("2. Add Tag\n");
        printf("3. View Collaboration Data\n");
        printf("4. Exit to Main Menu\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            choice = 0; 
            continue;
        }

        switch (choice) {
            case 1: add_annotation(); break;
            case 2: add_tag(); break;
            case 3: view_collaboration_data(); break;
            case 4: printf("[MODULE]: Exiting Collaboration Tools.\n"); break;
            default: printf("Invalid choice. Please select 1-4.\n");
        }
    }
}