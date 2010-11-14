/*
 *  Copyright 2000-2001: Sergio Rua <srua@debian.org>
 *  Copyright 2008 Joop Stakenborg <pg4i@amsat.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define BUFFER_SIZE 131072

static GList *pixmaps_directories = NULL;

GtkWidget*
lookup_widget(GtkWidget *widget, const gchar *widget_name)
{
    GtkWidget *parent, *found_widget;

    for (;;) {
        if (GTK_IS_MENU (widget))
            parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
        else
            parent = widget->parent;
        if (parent == NULL)
            break;
        widget = parent;
    }

    found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget),
                   widget_name);
    if (!found_widget)
        g_warning ("Widget not found: %s", widget_name);
    return found_widget;
}

/* Use this function to set the directory containing installed pixmaps. */
void
add_pixmap_directory(const gchar *directory)
{
    pixmaps_directories = g_list_prepend (pixmaps_directories, g_strdup (directory));
}

/* This is an internally used function to find pixmap files. */
static gchar*
find_pixmap_file(const gchar *filename)
{
    GList *elem;

    /* We step through each of the pixmaps directory to find it. */
    elem = pixmaps_directories;
    while (elem) {
        gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data, G_DIR_SEPARATOR_S, filename);
        if (g_file_test (pathname, G_FILE_TEST_EXISTS))
            return pathname;
        g_free (pathname);
        elem = elem->next;
    }
    return NULL;
}

GdkPixbuf*
create_image(const gchar *filename)
{
    gchar *pathname = NULL;
    GdkPixbuf *pixbuf;
    GError *error = NULL;

    if (!filename || !filename[0]) return NULL;

    pathname = find_pixmap_file (filename);

    if (!pathname) {
        g_warning (_("Couldn't find pixmap file: %s"), filename);
        return NULL;
    }

    pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
    if (!pixbuf) {
        fprintf (stderr, _("Failed to load pixbuf file: %s: %s\n"),
                 pathname, error->message);
        g_error_free (error);
    }
    g_free (pathname);
    return pixbuf;
}

void open_man_file(const gchar *manfile)
{
    GtkWidget *statusbar, *text;
    GtkSourceBuffer *tb;
    gzFile *f;
    gint bytes_read;
    gchar *buffer = g_malloc(BUFFER_SIZE);

    text = lookup_widget(GTK_WIDGET(wprincipal),"text");
    tb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer (GTK_TEXT_VIEW (text)));

    /* status bar */
    statusbar = lookup_widget(GTK_WIDGET(wprincipal), "statusbar1");
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 1);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, _("File opened."));

    /* now open the file */
    if ((f = gzopen(manfile, "rb")) != NULL) {
        gtk_source_buffer_begin_not_undoable_action(tb);
        gtk_text_buffer_set_text (GTK_TEXT_BUFFER(tb), "", 0);

        while(!gzeof(f)) {
            bytes_read = gzread(f, buffer, BUFFER_SIZE);
            if (bytes_read > 0) {
                gchar *utf8 = NULL;

                if (g_utf8_validate(buffer, -1, NULL) == FALSE) {
                    utf8 = g_locale_to_utf8(buffer, -1, NULL, NULL, NULL);
                }

                if (utf8 != NULL) {
                    strncpy(buffer,utf8, BUFFER_SIZE - 1);
                    buffer[BUFFER_SIZE - 1] = 0;
                }
                gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(tb),
                                                 buffer, bytes_read);
            }
        }
        gtk_source_buffer_end_not_undoable_action(tb);
        gzclose(f);
    } else {
        dialog_message(strerror(errno),GTK_MESSAGE_ERROR);
    }

    /* return the buffer */
    g_free(buffer);

    /* mark the text buffer as unaltered */
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(tb), FALSE);
}

const gchar *ReadConfFromFile(const gchar *variable)
{
    FILE *f;
    gchar *rcname;
    gchar *tok;
    gchar buf[1024];

    rcname = g_build_filename (g_get_home_dir(), G_DIR_SEPARATOR_S, ".gmaneditrc", NULL );
    f = fopen(rcname,"r");
    g_free (rcname);

    if (f == NULL) return((gchar *)NULL);

    while (fgets(buf,80,f) != NULL) {
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';

        if ((buf[0] != '#') && (!strncmp(variable,buf,strlen(variable)))) {
            tok = strtok(buf,"=");
            tok = strtok(NULL,"=");
            fclose(f);
            return(tok);
        }
    }
    fclose(f);
    return((gchar *)NULL);
}

gboolean OpenWebsite(const gchar *url)
{
    gchar *cmdline;
    gboolean success;

    /* assemble command line for xdg-open */
    cmdline = g_strdup_printf("/usr/bin/env xdg-open %s", url);

    /* spawn xdg-open */
    success = g_spawn_command_line_async(cmdline, NULL);

    g_free(cmdline);

    return success;
}

gboolean document_modified(GtkWindow *win, const gchar *tname)
{
    GtkWidget *text;
    GtkTextBuffer *tb;

    text = lookup_widget(GTK_WIDGET(win), tname);
    tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));

    return gtk_text_buffer_get_modified(tb);
}

gboolean
tree_list_add_unique(GtkListStore *slist, const gchar *string)
{
    GtkTreeIter iter;
    gboolean found = FALSE;

    /* look for the search term in the search history */
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(slist), &iter)) {
        do {
            const gchar *sval;

            gtk_tree_model_get(GTK_TREE_MODEL(slist), &iter, 0, &sval, -1);

            /* compare the current item's value with the search term*/
            if (g_strcmp0(sval, string) == 0) {
                found = TRUE;
            }
        } while (!found && gtk_tree_model_iter_next(GTK_TREE_MODEL(slist), &iter));
    }

    if (!found) {
        /* prepend a row to the search term list */
        gtk_list_store_prepend(slist, &iter);
        /* attach a copy of the entry field content to the list */
        gtk_list_store_set(slist, &iter, 0, g_strdup(string), -1);
    }

    return found;
}
