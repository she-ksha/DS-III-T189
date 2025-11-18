#include "research_organizer.h"
#include <math.h> // For M_PI and math functions

// At the top of graph.c

// --- DELETE THE OLD CONSTANTS ---

// --- ADD THESE NEW CONSTANTS ---

#define IDEAL_EDGE_LENGTH 250.0  // Desired distance between connected nodes (longer)
#define TIME_STEP 1.0            // How much to advance simulation
#define LAYOUT_ITERATIONS 500  
#define MIN_NODE_Y_SPACING 30  // Number of iterations
// --- END ---

// --- Static global variable ---
static GtkWidget *global_drawing_area = NULL; // To trigger redraws

// --- NEW Helper Function: Find a node by its ID ---
// This is needed by the drawing function
static GraphNode* find_node_by_id(int id) {
    if (!global_graph) return NULL;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        if (global_graph->nodes[i]->id == id) {
            return global_graph->nodes[i];
        }
    }
    return NULL;
}

// NEW FUNCTION 1: Lays out the Author-Paper graph
static void layout_author_graph(GtkDrawingArea *drawing_area) {
    if (!global_graph || global_graph->num_nodes == 0) return;

    gint width = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area));
    gint height = gtk_widget_get_allocated_height(GTK_WIDGET(drawing_area));
    double cx = width / 2.0;
    double cy = height / 2.0;

    // 1. Reset layout helper
    for (int i = 0; i < global_graph->num_nodes; i++) {
        global_graph->nodes[i]->papers_positioned = 0;
    }

    // 2. Count and position authors
    int author_count = 0;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        if (strcmp(global_graph->nodes[i]->type, "Author") == 0) {
            author_count++;
        }
    }
    
    double author_radius = (width < height ? width / 3.0 : height / 3.0);
    if (author_radius < 100) author_radius = 100;
    
    double angle_step = (author_count > 0) ? (2 * M_PI / author_count) : 0;
    
    int author_index = 0;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *node = global_graph->nodes[i];
        if (strcmp(node->type, "Author") == 0) {
            node->x = cx + author_radius * cos(angle_step * author_index);
            node->y = cy + author_radius * sin(angle_step * author_index);
            author_index++;
        }
    }

    // 3. Position papers in a "fan" around their author
    double paper_distance = 100.0; // Increased distance
    double paper_spread_angle = 0.35;
    
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *paper_node = global_graph->nodes[i];
        if (strcmp(paper_node->type, "Paper") == 0) {
            
            GraphNode *author_node = NULL;
            for (int e = 0; e < global_graph->num_edges; e++) {
                if (global_graph->edges[e]->from_node_id == paper_node->id) { // Paper -> Author
                    author_node = find_node_by_id(global_graph->edges[e]->to_node_id);
                    break;
                }
            }
            if (author_node) {
                // (This is the "fan out" logic)
                double dx = author_node->x - cx;
                double dy = author_node->y - cy;
                double author_angle = atan2(dy, dx);
                int paper_count_for_author = 0;
                for (int e = 0; e < global_graph->num_edges; e++) {
                    if (global_graph->edges[e]->to_node_id == author_node->id) {
                        paper_count_for_author++; // Mistake here, but let's fix it next
                    }
                }
                int k = author_node->papers_positioned;
                double fan_angle_start = author_angle - (paper_spread_angle * (paper_count_for_author - 1) / 2.0);
                double paper_angle = fan_angle_start + k * paper_spread_angle;
                double paper_radius = author_radius + paper_distance;
                paper_node->x = cx + paper_radius * cos(paper_angle);
                paper_node->y = cy + paper_radius * sin(paper_angle);
                author_node->papers_positioned++;
            } else {
                paper_node->x = cx;
                paper_node->y = cy;
            }
        }
    }
}


// REPLACE your old layout_keyword_graph function with this new one
static void layout_keyword_graph(GtkDrawingArea *drawing_area) {
    if (!global_graph || global_graph->num_nodes == 0) return;

    gint width = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area));
    gint height = gtk_widget_get_allocated_height(GTK_WIDGET(drawing_area));

    // 1. Count the two types of nodes
    int paper_count = 0;
    int keyword_count = 0;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        if (strcmp(global_graph->nodes[i]->type, "Paper") == 0) paper_count++;
        else if (strcmp(global_graph->nodes[i]->type, "Keyword") == 0) keyword_count++;
    }

    // --- THIS IS THE FIX (PART 1) ---
    // 2. Calculate total height needed based on the tallest column
    int nodes_in_tallest_column = (paper_count > keyword_count) ? paper_count : keyword_count;
    int min_height = (nodes_in_tallest_column * MIN_NODE_Y_SPACING) + 100; // 50px top/bottom margin
    
    // Don't shrink smaller than the viewport
    if (min_height < height) min_height = height;

    // 3. SET THE DRAWING AREA'S SIZE
    // This tells the scrolled window how big the content is
    gtk_widget_set_size_request(GTK_WIDGET(drawing_area), width, min_height);
    // --- END FIX (PART 1) ---


    // --- Vertical Bipartite Layout ---
    double paper_x = width * 0.25;
    double keyword_x = width * 0.75;
    
    // 4. Position all Paper nodes
    // We use min_height so they spread out across the entire scrollable area
    double paper_y_spacing = (paper_count > 1) ? (min_height - 100) / (paper_count - 1) : 0;
    double paper_y_start = 50;
    if (paper_count == 1) paper_y_start = min_height / 2.0;
    
    int current_paper = 0;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *node = global_graph->nodes[i];
        if (strcmp(node->type, "Paper") == 0) {
            node->x = paper_x;
            node->y = (paper_count == 1) ? paper_y_start : (paper_y_start + (current_paper * paper_y_spacing));
            current_paper++;
        }
    }

    // 5. Position all Keyword nodes
    double keyword_y_spacing = (keyword_count > 1) ? (min_height - 100) / (keyword_count - 1) : 0;
    double keyword_y_start = 50;
    if (keyword_count == 1) keyword_y_start = min_height / 2.0;
    
    int current_keyword = 0;
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *node = global_graph->nodes[i];
        if (strcmp(node->type, "Keyword") == 0) {
            node->x = keyword_x;
            node->y = (keyword_count == 1) ? keyword_y_start : (keyword_y_start + (current_keyword * keyword_y_spacing));
            current_keyword++;
        }
    }
}
static gboolean on_graph_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    // Get the center of the drawing area
    gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);
    double cx = width / 2.0;

    // --- Placeholder text if no graph ---
    if (!global_graph) {
        cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
        cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 16);
        cairo_text_extents_t extents;
        cairo_text_extents(cr, "Click 'Build Graph' to start", &extents);
        cairo_move_to(cr, (width / 2.0) - (extents.width / 2.0), height / 2.0);
        cairo_show_text(cr, "Click 'Build Graph' to start");
        return FALSE;
    }
    // --- End placeholder ---


   cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.7); // Semi-transparent gray
    cairo_set_line_width(cr, 2.0);
    
    for (int i = 0; i < global_graph->num_edges; i++) {
        GraphEdge *edge = global_graph->edges[i];
        GraphNode *from = find_node_by_id(edge->from_node_id);
        GraphNode *to = find_node_by_id(edge->to_node_id);

        if (from && to) {
            cairo_move_to(cr, from->x, from->y);
            cairo_line_to(cr, to->x, to->y);
            cairo_stroke(cr);
        }
    }

    // 2. --- Draw Nodes (Circles and Text) ---
    cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    
    for (int i = 0; i < global_graph->num_nodes; i++) {
        GraphNode *node = global_graph->nodes[i];

        // --- A. Draw the circle (use node's stored color) ---
        cairo_set_source_rgb(cr, node->r, node->g, node->b);
        cairo_arc(cr, node->x, node->y, 10.0, 0, 2 * M_PI);
        cairo_fill(cr);

        // --- B. Draw the text (THIS IS THE FIX) ---
        cairo_set_source_rgb(cr, 0, 0, 0); // Black text
        
        // Get the size of the text
        cairo_text_extents_t extents;
        cairo_text_extents(cr, node->label, &extents);

        double text_x;
        double text_y = node->y + 4; // Center text vertically
        double text_padding = 15.0;  // 15px padding from the node

        if (node->x < cx) {
            // Node is on the LEFT, draw text to the RIGHT
            text_x = node->x + text_padding;
        } else {
            // Node is on the RIGHT, draw text to the LEFT
            text_x = node->x - text_padding - extents.width;
        }
        
        cairo_move_to(cr, text_x, text_y);
        cairo_show_text(cr, node->label);
    }
    
    return FALSE; // We have finished drawing
}

// --- NEW Click Handler for the Button ---
static void on_build_graph_clicked(GtkButton *button, gpointer user_data) {
    build_knowledge_graph(); // 
    if (global_drawing_area) {
        layout_author_graph(GTK_DRAWING_AREA(global_drawing_area)); // <-- NEW
    }
    if (global_drawing_area) {
        gtk_widget_queue_draw(global_drawing_area);
    }
}


// --- ADD this new click handler ---
static void on_build_citation_graph_clicked(GtkButton *button, gpointer user_data) {
    build_citation_graph();
    if (global_drawing_area) {
        layout_keyword_graph(GTK_DRAWING_AREA(global_drawing_area));(GTK_DRAWING_AREA(global_drawing_area)); // <-- NEW
    }
    if (global_drawing_area) {
        gtk_widget_queue_draw(global_drawing_area);
    }
}


// --- This is the main function for this file ---
// It replaces the placeholder in your old search.c
// REPLACE your old create_knowledge_graph_page function with this new one
GtkWidget *create_knowledge_graph_page() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Knowledge Graph</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    // --- Button 1: Author Graph ---
    GtkWidget *ref_graph_btn = gtk_button_new_with_label("Build Author Graph");
    g_signal_connect(ref_graph_btn, "clicked", G_CALLBACK(on_build_graph_clicked), NULL);
    
    // --- Button 2: Citation Graph ---
    GtkWidget *collab_graph_btn = gtk_button_new_with_label("Build Citation Graph");
    g_signal_connect(collab_graph_btn, "clicked", G_CALLBACK(on_build_citation_graph_clicked), NULL);
    
    gtk_style_context_add_class(gtk_widget_get_style_context(ref_graph_btn), "suggested-action");
    gtk_style_context_add_class(gtk_widget_get_style_context(collab_graph_btn), "suggested-action");

    gtk_box_pack_start(GTK_BOX(control_box), ref_graph_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), collab_graph_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), control_box, FALSE, FALSE, 0);

    
    // --- THIS IS THE FIX (PART 2) ---

    // 1. Create a scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,  // Auto horizontal scroll
                                   GTK_POLICY_AUTOMATIC); // Auto vertical scroll
    gtk_widget_set_vexpand(scrolled_window, TRUE); // Allow it to fill space

    // 2. Create the drawing area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    // We set a minimum *width*, but let the height be dynamic
    gtk_widget_set_size_request(drawing_area, 600, -1); 
    
    gtk_style_context_add_class(gtk_widget_get_style_context(drawing_area), "graph-area");
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_graph_draw), NULL);
    global_drawing_area = drawing_area; // Save global reference

    // 3. Add the drawing area *inside* the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), drawing_area);

    // 4. Pack the *scrolled window* (not the drawing area) into the UI
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // --- END FIX (PART 2) ---

    return vbox;
}