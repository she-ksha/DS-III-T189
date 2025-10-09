#ifndef ALL_IN_ONE_H
#define ALL_IN_ONE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 


#define MAX_TITLE_LEN 256
#define MAX_AUTHOR_LEN 128
#define MAX_TEXT_LEN 256


#define HASH_MAP_SIZE 100 


#define ALPHABET_SIZE 26 


#define MAX_REFERENCES 20
#define MAX_COLLABORATORS 5
#define MAX_ANNOTATIONS 10
#define MAX_TAGS 5


typedef struct Paper {
    int id;                                 
    char title[MAX_TITLE_LEN];              
    char author[MAX_AUTHOR_LEN];
    char keywords[MAX_AUTHOR_LEN];
    int year;                               
    
    
    int cited_paper_ids[MAX_REFERENCES];    
    int num_cited;
    char collaborator_names[MAX_COLLABORATORS][MAX_AUTHOR_LEN]; 
    int num_collaborators;
    
    
    char annotations[MAX_ANNOTATIONS][MAX_TEXT_LEN];
    int num_annotations;
    char tags[MAX_TAGS][50];
    int num_tags;

    struct Paper* next; 
} Paper;



typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    Paper* paper_ptr; 
    int is_end_of_word;
} TrieNode;


extern Paper* paper_storage[HASH_MAP_SIZE]; 


extern TrieNode* title_trie_root;


Paper* find_paper_for_action();

int hash(char* key);
void initialize_storage();
void insert_paper(Paper* new_paper);
Paper* retrieve_paper_by_title(char* title);
void delete_paper(char* title);
TrieNode* create_trie_node();
void insert_trie_word(TrieNode* root, char* word, Paper* paper_ptr);
void handle_autocomplete(TrieNode* root, char* prefix);
Paper* find_paper_by_id(int id); 



int handle_login_signup();

void handle_dashboard();



void handle_paper_management();
void upload_and_extract_metadata();



void handle_search_and_filter();



void handle_knowledge_graph();

void handle_collaboration_tools();



void handle_reports_and_export();


#endif 