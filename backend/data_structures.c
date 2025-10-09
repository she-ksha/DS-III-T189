#include "all_in_one.h"


Paper* paper_storage[HASH_MAP_SIZE];

// Trie storage: The root node for title indexing
TrieNode* title_trie_root = NULL;


// ==========================================================
// B. HASH MAP IMPLEMENTATION
// ==========================================================

// --- 1. Hash Function ---
// Converts a string key (like a paper title) into a Hash Map index.
int hash(char* key) {
    if (!key) return 0;
    unsigned long hash_value = 0;
    int c;
    // Simple hashing algorithm
    while ((c = *key++)) {
        hash_value = c + (hash_value << 6) + (hash_value << 16) - hash_value;
    }
    return hash_value % HASH_MAP_SIZE;
}

// --- 2. Storage Initialization ---
void initialize_storage() {
    // Initialize Hash Map array pointers to NULL
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        paper_storage[i] = NULL;
    }
    
    // Initialize Trie root (Creation logic moved to separate function)
    title_trie_root = create_trie_node();
}

// --- 3. Retrieve Paper (Quick Retrieval) ---
Paper* retrieve_paper_by_title(char* title) {
    if (!title || strlen(title) == 0) return NULL;
    int index = hash(title);
    Paper* current = paper_storage[index];

    // Traverse the linked list (handles collisions)
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            return current; // Found the paper!
        }
        current = current->next;
    }
    return NULL; // Not found
}

// --- 4. Delete Paper ---
void delete_paper(char* title) {
    if (!title || strlen(title) == 0) return;
    int index = hash(title);
    Paper* current = paper_storage[index];
    Paper* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            if (prev == NULL) {
                // Deleting the head node
                paper_storage[index] = current->next;
            } else {
                // Deleting a middle/tail node
                prev->next = current->next;
            }
            free(current);
            printf("[BACKEND]: Paper '%s' deleted from storage.\n", title);
            // NOTE: For a complete solution, you would also remove the title from the Trie here.
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("[BACKEND]: Paper '%s' not found for deletion.\n", title);
}


// ==========================================================
// C. TRIE IMPLEMENTATION (For Search/Autocomplete)
// ==========================================================

// --- 5. Create Trie Node ---
TrieNode* create_trie_node() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node) {
        node->is_end_of_word = 0;
        node->paper_ptr = NULL;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// --- 6. Insert Word into Trie ---
void insert_trie_word(TrieNode* root, char* word, Paper* paper_ptr) {
    if (!root) return;
    int length = strlen(word);
    TrieNode* current = root;

    for (int i = 0; i < length; i++) {
        // Only index letters and convert to lowercase for case-insensitive search
        char c = tolower(word[i]);
        if (c < 'a' || c > 'z') continue; 

        int index = c - 'a';
        if (!current->children[index]) {
            current->children[index] = create_trie_node();
        }
        current = current->children[index];
    }

    // Mark the end of the word and link to the actual paper data
    current->is_end_of_word = 1;
    current->paper_ptr = paper_ptr; 
}


void autocomplete_suggest_papers(TrieNode* node) {
    if (!node) return;

   
    if (node->is_end_of_word && node->paper_ptr) {
        printf("    -> %s (Author: %s, Year: %d)\n", 
               node->paper_ptr->title, 
               node->paper_ptr->author, 
               node->paper_ptr->year);
    }

    
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            autocomplete_suggest_papers(node->children[i]);
        }
    }
}


void handle_autocomplete(TrieNode* root, char* prefix) {
    if (!root || strlen(prefix) == 0) {
        printf("Please enter a non-empty prefix.\n");
        return;
    }
    
    int length = strlen(prefix);
    TrieNode* current = root;

    
    for (int i = 0; i < length; i++) {
        char c = tolower(prefix[i]);
        if (c < 'a' || c > 'z') continue;

        int index = c - 'a';
        if (!current->children[index]) {
            printf("No suggestions found for prefix '%s'.\n", prefix);
            return;
        }
        current = current->children[index];
    }

    printf("\n--- Autocomplete Suggestions for '%s' ---\n", prefix);
    
    autocomplete_suggest_papers(current);
}


void insert_paper(Paper* new_paper) {
    if (title_trie_root == NULL) {
        title_trie_root = create_trie_node();
    }
    
    
    int index = hash(new_paper->title);
    new_paper->next = paper_storage[index];
    paper_storage[index] = new_paper;

    
    insert_trie_word(title_trie_root, new_paper->title, new_paper);

    printf("\n[BACKEND]: Paper '%s' indexed successfully.\n", new_paper->title);
}


Paper* find_paper_by_id(int id) {
    
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            if (current->id == id) {
                return current;
            }
            current = current->next;
        }
    }
    return NULL;
}