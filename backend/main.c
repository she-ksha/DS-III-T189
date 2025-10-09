#include "all_in_one.h"
// This single header file should now include:
// - login.h (for handle_login_signup)
// - dashboard.h (for handle_dashboard)
// - paper.h (for handle_paper_management)
// - search_filter.h (for handle_search_and_filter)
// - knowledge_graph.h (for handle_knowledge_graph)
// - collaboration_tools.h (for handle_collaboration_tools)
// - reports_export.h (for handle_reports_and_export)


void display_main_menu() {
    printf("\n--- 📋 RESEARCH PAPER ORGANIZER  ---\n");
    printf("1. DASHBOARD\n");
    printf("2. PAPER MANAGEMENT\n");
    printf("3. SEARCH AND FILTER\n");
    printf("4. KNOWLEDGE GRAPH\n");
    printf("5. COLLABORATION TOOLS\n");
    printf("6. REPORTS AND EXPORT\n");
    printf("7. EXIT PROGRAM\n");
    printf("Choose an option (1-7): ");
}


int main() {
    initialize_storage(); 

    if (!handle_login_signup()) {
        return 1; 
    }

    
    int choice = 0;
    while (choice != 7) {
        display_main_menu();
        
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            
            while (getchar() != '\n');
            choice = 0; 
            continue;
        }

        
        switch (choice) {
            case 1: 
                handle_dashboard(); 
                break;
            case 2: 
                handle_paper_management(); 
                break;
            case 3: 
                handle_search_and_filter(); 
                break;
            case 4: 
                handle_knowledge_graph(); 
                break;
            case 5: 
                handle_collaboration_tools(); 
                break;
            case 6: 
                handle_reports_and_export(); 
                break;
            case 7:
                printf("\n👋 EXIT PROGRAM: Exiting the Research Paper Organizer.\n");
                
                break;
            default:
                printf("Invalid choice. Please select 1-7.\n");
        }
    }
    
    return 0;
}