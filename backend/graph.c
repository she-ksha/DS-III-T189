#include "all_in_one.h"
#include <stdbool.h> // For bool type

// --- Helper: Find Paper by ID ---
// Since the Hash Map uses title, we need a slow linear search for graph traversal
// In a real system, you'd have a Hash Map keyed by ID.




// ==========================================================
// 4.1: BUILD REFERENCE GRAPH (Manual Edge Creation)
// ==========================================================

void add_reference_edge() {
    char source_title[MAX_TITLE_LEN];
    char target_title[MAX_TITLE_LEN];
    
    printf("\n--- Add Reference Edge ---\n");
    printf("Enter Title of paper that CITES another (Source): ");
    scanf(" %255[^\n]", source_title);
    
    printf("Enter Title of paper that IS CITED (Target): ");
    scanf(" %255[^\n]", target_title);

    Paper* source_paper = retrieve_paper_by_title(source_title);
    Paper* target_paper = retrieve_paper_by_title(target_title);
    
    if (source_paper == NULL || target_paper == NULL) {
        printf("❌ Error: One or both papers not found. Cannot create reference.\n");
        return;
    }
    
    // Check for capacity and duplicates
    if (source_paper->num_cited < MAX_REFERENCES) {
        // Add the target's ID to the source paper's adjacency list
        source_paper->cited_paper_ids[source_paper->num_cited++] = target_paper->id;
        printf("✅ Reference created: '%s' cites '%s'.\n", source_paper->title, target_paper->title);
    } else {
        printf("Warning: Source paper already has maximum references (%d).\n", MAX_REFERENCES);
    }
}

// ==========================================================
// 4.2: GRAPH TRAVERSAL ALGORITHMS
// ==========================================================

// --- Depth-First Search (DFS) Implementation ---
// Explores as far as possible along each branch before backtracking.
void dfs_traversal_recursive(int paper_id, bool visited[], int visited_count) {
    // 1. Mark the current node as visited
    visited[paper_id] = true;
    Paper* current = find_paper_by_id(paper_id);
    
    if (current) {
        // Print the paper details
        printf("%*s> [%d] %s\n", visited_count * 2, "", current->id, current->title);
    } else {
        return; // Should not happen if ID is valid
    }

    // 2. Recur for all adjacent nodes (cited papers)
    for (int i = 0; i < current->num_cited; i++) {
        int neighbor_id = current->cited_paper_ids[i];
        if (neighbor_id < HASH_MAP_SIZE && !visited[neighbor_id]) { // Assuming Paper IDs < HASH_MAP_SIZE for array indexing
            dfs_traversal_recursive(neighbor_id, visited, visited_count + 1);
        }
    }
}

void run_dfs_traversal(char* start_title) {
    Paper* start_paper = retrieve_paper_by_title(start_title);
    if (!start_paper) {
        printf("❌ Start paper not found.\n");
        return;
    }
    
    // The visited array tracks which IDs have been seen
    // NOTE: This assumes paper IDs are small and contiguous.
    bool visited[HASH_MAP_SIZE] = {false};
    
    printf("\n--- Depth-First Traversal (Citation Chain) ---\n");
    dfs_traversal_recursive(start_paper->id, visited, 0);
}


// --- Breadth-First Search (BFS) Implementation ---
// Explores neighbor nodes first, before moving to the next level (shortest path).
void run_bfs_traversal(char* start_title) {
    Paper* start_paper = retrieve_paper_by_title(start_title);
    if (!start_paper) {
        printf("❌ Start paper not found.\n");
        return;
    }
    
    // 1. Initialize Queue and Visited Array
    int queue[HASH_MAP_SIZE]; // Simple array simulating a Queue
    int front = 0, rear = 0;
    bool visited[HASH_MAP_SIZE] = {false};

    // 2. Enqueue the starting node
    queue[rear++] = start_paper->id;
    visited[start_paper->id] = true;
    
    printf("\n--- Breadth-First Traversal (Citation Layers) ---\n");

    // 3. Loop until the queue is empty
    while (front < rear) {
        int current_id = queue[front++]; // Dequeue
        Paper* current = find_paper_by_id(current_id);
        
        if (current) {
            printf("Layer %d: [%d] %s\n", front, current->id, current->title);
        }

        // 4. Enqueue unvisited neighbors (cited papers)
        if (current) {
            for (int i = 0; i < current->num_cited; i++) {
                int neighbor_id = current->cited_paper_ids[i];
                if (neighbor_id < HASH_MAP_SIZE && !visited[neighbor_id]) {
                    visited[neighbor_id] = true;
                    queue[rear++] = neighbor_id; // Enqueue
                }
            }
        }
    }
}


// --- Menu for Graph Traversal ---
void traverse_graph_menu() {
    char start_title[MAX_TITLE_LEN];
    int choice;

    printf("\nEnter Title to start traversal from: ");
    scanf(" %255[^\n]", start_title);

    printf("Choose Traversal Type:\n");
    printf("1. Depth-First Search (DFS) - Longest chain\n");
    printf("2. Breadth-First Search (BFS) - Layered connection (shortest path)\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1) return;

    if (choice == 1) {
        run_dfs_traversal(start_title);
    } else if (choice == 2) {
        run_bfs_traversal(start_title);
    } else {
        printf("Invalid choice.\n");
    }
}


// --- Main Handler for Knowledge Graph Module ---
void handle_knowledge_graph() {
    int choice = 0;

    while (choice != 3) {
        printf("\n--- 🌐 KNOWLEDGE GRAPH MENU ---\n");
        printf("1. Add Reference (Build Edge)\n");
        printf("2. Traverse Graph (DFS/BFS)\n");
        printf("3. Exit to Main Menu\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            choice = 0; 
            continue;
        }

        switch (choice) {
            case 1: add_reference_edge(); break;
            case 2: traverse_graph_menu(); break;
            case 3: printf("[MODULE]: Exiting Knowledge Graph.\n"); break;
            default: printf("Invalid choice. Please select 1-3.\n");
        }
    }
}