#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "shell-check.h"

int main(int argc, char* argv[])
{
    printf("Starting the guesser...\n");
    bool result = CheckIfRunningFromShell();

    if (result) {
        fprintf(
            stderr,
            "Looks like I'm running in a terminal.\n"
            "Did I guess right?\n"
        );
    } else {
        if (!gtk_init_check(0, NULL)) {
            return 1;
        }
        GtkWidget* parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);

        GtkWidget* dialog = gtk_message_dialog_new(
            GTK_WINDOW(parent),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s",
            "Looks like I'm running from a desktop.\nDid I guess right?"
        );
        gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);

        gtk_window_set_title(GTK_WINDOW(dialog), "Just a guess.");
        gtk_dialog_run(GTK_DIALOG(dialog));

        gtk_widget_destroy(GTK_WIDGET(dialog));
        gtk_widget_destroy(GTK_WIDGET(parent));
        while (g_main_context_iteration(NULL, false));
    }
    
    return 0;
}
