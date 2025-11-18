#include "research_organizer.h"

void on_switch_to_signup(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "signup_page");
}

void on_switch_to_login(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "login_page");
}

// REPLACE the old on_login_btn_clicked with this:
static void on_login_btn_clicked(GtkButton *button, GtkStack *stack) {
    // --- This is the code I forgot to include ---
    GtkWidget *parent_grid = gtk_widget_get_parent(GTK_WIDGET(button));
    GtkWidget *username_entry = g_object_get_data(G_OBJECT(parent_grid), "username-entry");
    GtkWidget *password_entry = g_object_get_data(G_OBJECT(parent_grid), "password-entry");
    GtkWidget *status_label = g_object_get_data(G_OBJECT(parent_grid), "status-label");

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_entry));
    // --- End of missing code ---

    if (authenticate_user(username, password)) {
        GtkWidget *login_window = gtk_widget_get_toplevel(GTK_WIDGET(stack));

        // 1. Create the main window
        create_main_app_window();

        // 3. Refresh the UI
        update_paper_list(NULL);
        refresh_dashboard_metrics();

        // 4. Hide login and show main app
        gtk_widget_hide(login_window);
        gtk_widget_show_all(main_window);

    } else {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Invalid Username or Password.</span>");
    }
}

static void on_create_btn_clicked(GtkButton *button, GtkStack *stack) {
    GtkWidget *parent_grid = gtk_widget_get_parent(GTK_WIDGET(button));
    GtkWidget *username_entry = g_object_get_data(G_OBJECT(parent_grid), "username-entry");
    GtkWidget *password_entry = g_object_get_data(G_OBJECT(parent_grid), "password-entry");
    GtkWidget *status_label = g_object_get_data(G_OBJECT(parent_grid), "status-label");

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (strlen(username) < 3 || strlen(password) < 6) {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Username must be 3+ and password 6+ characters.</span>");
        return;
    }

    if (create_user(username, password)) {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='green'>Account Created! Please log in.</span>");
        gtk_stack_set_visible_child_name(stack, "login_page");
    } else {
        gtk_label_set_markup(GTK_LABEL(status_label), "<span foreground='red'>Account Creation Failed (Username 'taken' or other mock error).</span>");
    }
}



static GtkWidget *create_login_ui(GtkStack *stack, gboolean is_login) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    
    gtk_widget_set_name(grid, "login-card");

    GtkWidget *title_label = gtk_label_new(is_login ? "User Login" : "Create Account");
    gtk_label_set_markup(GTK_LABEL(title_label), is_login ? "<span size='xx-large' weight='bold' foreground='#3f51b5'>User Login</span>" : "<span size='xx-large' weight='bold' foreground='#3f51b5'>Create Account</span>");
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 2, 1);

    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username ");
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Username:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), username_entry, 1, 1, 1, 1);

    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Password:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_entry, 1, 2, 1, 1);

    GtkWidget *status_label = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), status_label, 0, 3, 2, 1);

    GtkWidget *action_button = gtk_button_new_with_label(is_login ? "Login" : "Create Account");
    gtk_style_context_add_class(gtk_widget_get_style_context(action_button), "suggested-action");
    
    GtkWidget *switch_button = gtk_button_new_with_label(is_login ? "Need an Account? Sign Up" : "Already have an account? Log In");
    
    gtk_grid_attach(GTK_GRID(grid), action_button, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), switch_button, 0, 5, 2, 1);
    
    if (is_login) {
        g_signal_connect(action_button, "clicked", G_CALLBACK(on_login_btn_clicked), stack);
        g_signal_connect(switch_button, "clicked", G_CALLBACK(on_switch_to_signup), stack);
    } else {
        g_signal_connect(action_button, "clicked", G_CALLBACK(on_create_btn_clicked), stack);
        g_signal_connect(switch_button, "clicked", G_CALLBACK(on_switch_to_login), stack);
    }

    g_object_set_data(G_OBJECT(grid), "username-entry", username_entry);
    g_object_set_data(G_OBJECT(grid), "password-entry", password_entry);
    g_object_set_data(G_OBJECT(grid), "status-label", status_label);
    
    return grid;
}

void create_login_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "USER LOGIN");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 450);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    g_signal_connect(window, "delete-event", G_CALLBACK(on_window_delete_event), NULL);


    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    GtkWidget *login_page = create_login_ui(GTK_STACK(stack), TRUE);
    GtkWidget *signup_page = create_login_ui(GTK_STACK(stack), FALSE);
    
    GtkWidget *center_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *center_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(center_vbox), stack, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(center_frame), center_vbox, TRUE, FALSE, 0);

    gtk_stack_add_named(GTK_STACK(stack), login_page, "login_page");
    gtk_stack_add_named(GTK_STACK(stack), signup_page, "signup_page");

    gtk_box_pack_start(GTK_BOX(main_box), center_frame, TRUE, TRUE, 50);

    gtk_widget_show_all(window);
}