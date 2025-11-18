#include "research_organizer.h"
// --- ADD THIS LINE AT THE TOP ---
#define DATABASE_FILE "all_papers.csv"

// --- Global Backend Variables ---
Paper* paper_storage[HASH_MAP_SIZE];
int g_paper_id_counter = 1;
KnowledgeGraph *global_graph = NULL;
int g_graph_node_id_counter = 1;
// === CORE HASH MAP FUNCTIONS ===

void init_paper_storage() {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        paper_storage[i] = NULL;
    }
}

unsigned int hash(const char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HASH_MAP_SIZE;
}

void insert_paper(Paper* p) {
    unsigned int index = hash(p->title);
    p->next = paper_storage[index];
    paper_storage[index] = p;
}
// (You'll need a simple way to create nodes/edges)
// Returns the ID of the new node
static int add_graph_node(const gchar *label, const gchar *type) {
    if (global_graph->num_nodes >= MAX_GRAPH_NODES) return -1; // Graph is full

    GraphNode *node = (GraphNode*)malloc(sizeof(GraphNode));
    node->id = g_graph_node_id_counter++;
    strcpy(node->label, label);
    strcpy(node->type, type);
    node->x = 0; // Will be set by layout algorithm
    node->y = 0;
    node->papers_positioned = 0;

    // --- ADD THIS CODE ---
    if (strcmp(type, "Paper") == 0) {
        node->r = 0.2; node->g = 0.4; node->b = 0.8; // Blue for Papers
    } else if (strcmp(type, "Author") == 0) {
        node->r = 0.1; node->g = 0.7; node->b = 0.2; // Green for Authors
    } else if (strcmp(type, "Keyword") == 0) {
        node->r = 0.8; node->g = 0.5; node->b = 0.1; // Orange for Keywords
    } else {
        node->r = 0.5; node->g = 0.5; node->b = 0.5; // Grey by default
    }
    // --- END ADD ---
    global_graph->nodes[global_graph->num_nodes++] = node;
    return node->id;
}

static void add_graph_edge(int from_id, int to_id, const gchar *label) {
    if (global_graph->num_edges >= MAX_GRAPH_EDGES) return; // Graph is full

    GraphEdge *edge = (GraphEdge*)malloc(sizeof(GraphEdge));
    edge->from_node_id = from_id;
    edge->to_node_id = to_id;
    strcpy(edge->label, label);

    global_graph->edges[global_graph->num_edges++] = edge;
}

Paper* retrieve_paper_by_title(const char* title) {
    unsigned int index = hash(title);
    Paper* current = paper_storage[index];
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Paper* find_paper_by_id(int id_to_find) {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            if (current->id == id_to_find) {
                return current;
            }
            current = current->next;
        }
    }
    return NULL;
}

bool delete_paper(const char* title) {
    unsigned int index = hash(title);
    Paper* current = paper_storage[index];
    Paper* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            if (prev == NULL) {
                paper_storage[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

// === GUI-FRIENDLY FUNCTIONS ===

void add_paper_from_gui(const char* title, const char* author, int year, const char* keywords,const char* filepath) {
    if (retrieve_paper_by_title(title)) {
        // In a real app, show an error
        g_print("Error: Paper with this title already exists.\n");
        return;
    }
    
    Paper* new_paper = (Paper*)malloc(sizeof(Paper));
    if (new_paper == NULL) {
        g_print("Error: Memory allocation failed.\n");
        return;
    }

    new_paper->id = g_paper_id_counter++;
    strncpy(new_paper->title, title, MAX_TITLE_LEN - 1);
    strncpy(new_paper->author, author, MAX_AUTHOR_LEN - 1);
    new_paper->year = year;
    strncpy(new_paper->keywords, keywords, MAX_KEYWORDS_LEN - 1);

    // --- ADD THIS LINE ---
    if (filepath) {
        strncpy(new_paper->filepath, filepath, 511);
    } else {
        new_paper->filepath[0] = '\0'; // Ensure it's an empty string
    }
    // --- END ADD ---

    // --- ADD THIS LINE (current_user_id is global from auth_backend.c) ---
    if (current_user_id) {
        strncpy(new_paper->owner, current_user_id, MAX_AUTHOR_LEN - 1);
    } else {
        strcpy(new_paper->owner, "unknown"); // Fallback
    }
    // --- END ADD ---

    new_paper->num_annotations = 0;
    new_paper->num_tags = 0;
    new_paper->num_citations = 0;
    new_paper->next = NULL;

    insert_paper(new_paper);
}

gchar* load_notes_for_paper(int paper_id) {
    Paper* p = find_paper_by_id(paper_id);
    if (!p) return g_strdup(""); // Return empty string

    // Use GString to build the text block
    GString* notes_text = g_string_new("");
    for (int i = 0; i < p->num_annotations; i++) {
        g_string_append(notes_text, p->annotations[i]);
        g_string_append_c(notes_text, '\n');
    }
    return g_string_free(notes_text, FALSE); // FALSE = don't free buffer
}

void save_notes_for_paper(int paper_id, const char* all_notes) {
    Paper* p = find_paper_by_id(paper_id);
    if (!p) return;

    p->num_annotations = 0; // Clear old annotations
    char* notes_copy = g_strdup(all_notes); // Make a copy
    char* line = strtok(notes_copy, "\n");

    while (line != NULL && p->num_annotations < MAX_ANNOTATIONS) {
        if (strlen(line) > 0) {
            strncpy(p->annotations[p->num_annotations], line, MAX_ANNOTATION_LEN - 1);
            p->annotations[p->num_annotations][MAX_ANNOTATION_LEN - 1] = '\0';
            p->num_annotations++;
        }
        line = strtok(NULL, "\n");
    }
    g_free(notes_copy);
}

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

int get_hash_map_load_percent() {
    int total_papers = get_total_papers();
    if (HASH_MAP_SIZE == 0) return 0;
    return (total_papers * 100) / HASH_MAP_SIZE;
}

// Refactored to return a string for a dialog
gchar* get_keyword_frequency_report_string() {
    // This uses a simple array for demo; a hash map is better
    #define KEYWORD_MAP_SIZE 50
    typedef struct { char word[50]; int count; } KeywordCount;
    KeywordCount keyword_map[KEYWORD_MAP_SIZE] = {0};
    int num_keywords = 0;

    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            char temp_keywords[MAX_KEYWORDS_LEN];
            strcpy(temp_keywords, current->keywords);
            char *token = strtok(temp_keywords, ", ");
            while (token != NULL) {
                if (strlen(token) > 1) {
                    bool found = false;
                    for (int k = 0; k < num_keywords; k++) {
                        if (strcmp(keyword_map[k].word, token) == 0) {
                            keyword_map[k].count++;
                            found = true;
                            break;
                        }
                    }
                    if (!found && num_keywords < KEYWORD_MAP_SIZE) {
                        strcpy(keyword_map[num_keywords].word, token);
                        keyword_map[num_keywords].count = 1;
                        num_keywords++;
                    }
                }
                token = strtok(NULL, ", ");
            }
            current = current->next;
        }
    }

    GString* report = g_string_new("Keyword Frequency Report:\n\n");
    g_string_append_printf(report, "%-20s %s\n", "KEYWORD", "COUNT");
    g_string_append(report, "-------------------------------\n");
    for (int i = 0; i < num_keywords; i++) {
        g_string_append_printf(report, "%-20s %d\n", keyword_map[i].word, keyword_map[i].count);
    }
    return g_string_free(report, FALSE);
}

bool add_reference_edge_from_gui(const char* source_title, const char* target_title) {
    Paper* source_paper = retrieve_paper_by_title(source_title);
    Paper* target_paper = retrieve_paper_by_title(target_title);
    
    if (source_paper == NULL || target_paper == NULL) return false;
    
    if (source_paper->num_citations < MAX_CITATIONS) {
        source_paper->cited_paper_ids[source_paper->num_citations++] = target_paper->id;
        return true;
    }
    return false; // Max references reached
}


// --- PERSISTENCE (CSV) ---

#define MAX_CSV_LINE_LEN 2048

// Helper to parse CSV fields
char* get_csv_field(char** line_ptr) {
    char* token = (char*)malloc(MAX_CSV_LINE_LEN);
    if (!token) return NULL;
    
    char* start = *line_ptr;
    char* end;
    
    if (start == NULL || *start == '\0') { free(token); return NULL; }

    if (start[0] == '\"') { // Quoted
        start++; 
        end = strchr(start, '\"');
        if (!end) { strcpy(token, start); *line_ptr = NULL; return token; }
        strncpy(token, start, end - start);
        token[end - start] = '\0';
        *line_ptr = (*(end + 1) == ',') ? end + 2 : end + 1;
    } else { // Unquoted
        end = strchr(start, ',');
        if (!end) { strcpy(token, start); *line_ptr = NULL; }
        else {
            strncpy(token, start, end - start);
            token[end - start] = '\0';
            *line_ptr = end + 1;
        }
    }
    return token;
}
// Helper to remove surrounding quotes from a string
static void remove_quotes(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    if (len < 2) return;

    // Check if it starts and ends with quotes
    if ((str[0] == '"' && str[len-1] == '"') || 
        (str[0] == '\'' && str[len-1] == '\'')) {
        
        // Shift everything left by one
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0'; // Null terminate
    }
}

void import_from_csv() {
   FILE *file = fopen(DATABASE_FILE, "r");
    if (!file) {
        g_print("[SYSTEM]: No database file found. Starting fresh.\n");
        return;
    }

    char line[MAX_CSV_LINE_LEN];
    int count = 0;
    int highest_id = 0;

    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        
        Paper* new_paper = (Paper*)malloc(sizeof(Paper));
        if (!new_paper) break;
        memset(new_paper, 0, sizeof(Paper));

        char* line_ptr = line;
        char* token;

        token = get_csv_field(&line_ptr); new_paper->id = token ? atoi(token) : 0; free(token);
        token = get_csv_field(&line_ptr); if(token) strcpy(new_paper->title, token); free(token);
        token = get_csv_field(&line_ptr); if(token) strcpy(new_paper->author, token); free(token);
        token = get_csv_field(&line_ptr); new_paper->year = token ? atoi(token) : 0; free(token);
        token = get_csv_field(&line_ptr); if(token) strcpy(new_paper->keywords, token); free(token);
        
        token = get_csv_field(&line_ptr); new_paper->num_tags = token ? atoi(token) : 0; free(token);
        for (int i = 0; i < new_paper->num_tags; i++) {
            token = get_csv_field(&line_ptr); if(token) strcpy(new_paper->tags[i], token); free(token);
        }

        token = get_csv_field(&line_ptr); new_paper->num_annotations = token ? atoi(token) : 0; free(token);
        for (int i = 0; i < new_paper->num_annotations; i++) {
            token = get_csv_field(&line_ptr); if(token) strcpy(new_paper->annotations[i], token); free(token);
        }
        // --- ADD THIS LINE before insert_paper() ---
        token = get_csv_field(&line_ptr); 
        if(token) {
            remove_quotes(token);
            strcpy(new_paper->filepath, token); 
        }
        free(token);
        // --- ADD THIS LINE ---
        token = get_csv_field(&line_ptr); 
        if(token) {
            remove_quotes(token);
            strcpy(new_paper->owner, token); 
        }
        free(token);
        // --- END ADD --

        
        insert_paper(new_paper);
        if (new_paper->id > highest_id) highest_id = new_paper->id;
        count++;
    }
    fclose(file);
    g_paper_id_counter = highest_id + 1;
    //g_print("[SYSTEM]: Successfully loaded %d papers.\n", count);
    g_print("[SYSTEM]: Successfully loaded %d papers from %s.\n", count, DATABASE_FILE);
}

void export_to_csv() {
   FILE *file = fopen(DATABASE_FILE, "w");
    if (file == NULL) {
        g_print("Error: Could not open %s for writing.\n",DATABASE_FILE);
        return;
    }

    // --- CHANGE THIS LINE ---
   fprintf(file, "ID,Title,Author,Year,Keywords,NumTags,Tags,NumAnnotations,Annotations,Filepath,Owner\n");
    // ...
    int count = 0;
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current = paper_storage[i];
        while (current != NULL) {
            fprintf(file, "%d,\"%s\",\"%s\",%d,\"%s\"", 
                current->id, current->title, current->author, current->year, current->keywords);
            
            fprintf(file, ",%d", current->num_tags);
            for(int j=0; j < current->num_tags; j++) 
                fprintf(file, ",\"%s\"", current->tags[j]);
            
            fprintf(file, ",%d", current->num_annotations);
            for(int j=0; j < current->num_annotations; j++) 
                fprintf(file, ",\"%s\"", current->annotations[j]);
            
                fprintf(file, ",\"%s\"", current->filepath);
                // --- ADD THIS LINE ---
        fprintf(file, ",\"%s\"", current->owner);
        // ---

            fprintf(file, "\n");
            count++;
            current = current->next;
        }
    }
    fclose(file);
    g_print("[SYSTEM]: Saved %d papers to %s\n", count, DATABASE_FILE);
}

// Add this new function anywhere in backend.c
void update_paper_details(int id, const gchar *title, const gchar *author, int year, const gchar *keywords) {
    Paper* p = find_paper_by_id(id);
    if (!p) {
        g_print("ERROR: [Backend] Could not find paper with ID %d to update.\n", id);
        return;
    }

    // Update the struct in memory
    strcpy(p->title, title);
    strcpy(p->author, author);
    p->year = year;
    strcpy(p->keywords, keywords);
    
    g_print("[Backend]: Updated paper ID %d.\n", id);
}
/**/
static int get_node_id_for_paper_title(const gchar *title) {
    if (!global_graph) return -1;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *node = global_graph->nodes[i];
        if (strcmp(node->type, "Paper") == 0 && strcmp(node->label, title) == 0) {
            return node->id;
        }
    }
    return -1; // Not found
}

// Add this new helper function in backend.c
static int get_or_create_author_node(const gchar *author_name) {
    // 1. Check if an author node with this name already exists
    if (global_graph) {
        for (int i = 0; i < global_graph->num_nodes; i++) {
            GraphNode *node = global_graph->nodes[i];
            if (strcmp(node->type, "Author") == 0 && strcmp(node->label, author_name) == 0) {
                return node->id; // Found it! Return its ID.
            }
        }
    }
    
    // 2. Not found. Create a new one.
    return add_graph_node(author_name, "Author");
}

// NEW HELPER: Finds an existing keyword node or creates a new one
static int get_or_create_keyword_node(const gchar *keyword) {
    // 1. Check if a keyword node with this name already exists
    if (global_graph) {
        for (int i = 0; i < global_graph->num_nodes; i++) {
            GraphNode *node = global_graph->nodes[i];
            if (strcmp(node->type, "Keyword") == 0 && strcmp(node->label, keyword) == 0) {
                return node->id; // Found it! Return its ID.
            }
        }
    }
    // 2. Not found. Create a new one.
    return add_graph_node(keyword, "Keyword");
}

// In backend.c
// Replace your old function with this
void build_knowledge_graph() { // or build_knowledge_graph
    g_print("[Graph]: Building AUTHOR graph...\n");

    // 1. Initialize the global graph structure
    if (global_graph) {
        for (int i = 0; i < global_graph->num_nodes; i++) free(global_graph->nodes[i]);
        for (int i = 0; i < global_graph->num_edges; i++) free(global_graph->edges[i]);
        free(global_graph);
    }
    global_graph = (KnowledgeGraph*)malloc(sizeof(KnowledgeGraph));
    global_graph->num_nodes = 0;
    global_graph->num_edges = 0;
    g_graph_node_id_counter = 1;

    // 2. Loop through all papers in the hash map
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current_paper = paper_storage[i];
        while (current_paper != NULL) {
            
            // --- THIS IS THE FIX ---

            // 1. Create a node for the paper itself (this is always unique)
            int paper_node_id = add_graph_node(current_paper->title, "Paper");

            // 2. Get or create the author node
            // This new function handles the "merge" logic
            int author_node_id = get_or_create_author_node(current_paper->author);

            // 3. Create an edge connecting them
            if (paper_node_id != -1 && author_node_id != -1) {
                add_graph_edge(paper_node_id, author_node_id, "WROTE_BY");
            }
            
            // --- END FIX ---
            
            current_paper = current_paper->next;
        }
    }

    g_print("[Graph]: Built author graph with %d nodes and %d edges.\n",
            global_graph->num_nodes, global_graph->num_edges);
}

// In backend.c
// REPLACE your old build_citation_graph function with this one

void build_citation_graph() {
    g_print("[Graph]: Building KEYWORD graph...\n");

    // 1. Initialize the global graph structure
    if (global_graph) {
        for (int i = 0; i < global_graph->num_nodes; i++) free(global_graph->nodes[i]);
        for (int i = 0; i < global_graph->num_edges; i++) free(global_graph->edges[i]);
        free(global_graph);
    }
    global_graph = (KnowledgeGraph*)malloc(sizeof(KnowledgeGraph));
    global_graph->num_nodes = 0;
    global_graph->num_edges = 0;
    g_graph_node_id_counter = 1;

    // --- This is a 3-pass process ---

    // 2. FIRST PASS: Create "Keyword" nodes.
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current_paper = paper_storage[i];
        while (current_paper != NULL) {
            if (strlen(current_paper->keywords) > 0) {
                char keywords_copy[MAX_KEYWORDS_LEN];
                strcpy(keywords_copy, current_paper->keywords);
                char *token = strtok(keywords_copy, ",");
                while (token != NULL) {
                    get_or_create_keyword_node(token); // Helper from previous step
                    token = strtok(NULL, ",");
                }
            }
            current_paper = current_paper->next;
        }
    }

    // 3. SECOND PASS: Create a "Paper" node for EVERY paper
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current_paper = paper_storage[i];
        while (current_paper != NULL) {
            add_graph_node(current_paper->title, "Paper");
            current_paper = current_paper->next;
        }
    }

    // 4. THIRD PASS: Create edges
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Paper* current_paper = paper_storage[i];
        while (current_paper != NULL) {
            int paper_node_id = get_node_id_for_paper_title(current_paper->title);
            if (paper_node_id == -1) continue;

            if (strlen(current_paper->keywords) > 0) {
                char keywords_copy[MAX_KEYWORDS_LEN];
                strcpy(keywords_copy, current_paper->keywords);
                char *token = strtok(keywords_copy, ",");
                while (token != NULL) {
                    int keyword_node_id = get_or_create_keyword_node(token);
                    if (keyword_node_id != -1) {
                        add_graph_edge(paper_node_id, keyword_node_id, "HAS_KEYWORD");
                    }
                    token = strtok(NULL, ",");
                }
            }
            current_paper = current_paper->next;
        }
    }
    g_print("[Graph]: Built keyword graph with %d nodes and %d edges.\n",
            global_graph->num_nodes, global_graph->num_edges);
}