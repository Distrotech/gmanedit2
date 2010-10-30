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

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define BUFFER_SIZE 131072

GtkWidget*
lookup_widget                          (GtkWidget       *widget,
                                        const gchar     *widget_name)
{
  GtkWidget *parent, *found_widget;

  for (;;)
    {
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

static GList *pixmaps_directories = NULL;
/* Use this function to set the directory containing installed pixmaps. */
void
add_pixmap_directory(const gchar        *directory)
{
        pixmaps_directories = g_list_prepend (pixmaps_directories, g_strdup (directory));
}

/* This is an internally used function to find pixmap files. */
static gchar*
find_pixmap_file(const gchar    *filename)
{
        GList *elem;

        /* We step through each of the pixmaps directory to find it. */
        elem = pixmaps_directories;
        while (elem)
        {
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

        if (!pathname)
        {
                g_warning (_("Couldn't find pixmap file: %s"), filename);
                return NULL;
        }

        pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
        if (!pixbuf)
        {
                fprintf (stderr, _("Failed to load pixbuf file: %s: %s\n"),
                        pathname, error->message);
                g_error_free (error);
        }
        g_free (pathname);
        return pixbuf;
}

void open_man_file(gchar *manfile)
{
    GtkWidget *statusbar,*text;
    GtkTextBuffer *tb;
    gzFile *f;
    gint bytes_read;
    gint in_gzip;
    int n;
    char extension[10];
    gchar *utf8;
    gchar * buffer = (gchar*)malloc(BUFFER_SIZE);

    text=lookup_widget(GTK_WIDGET(wprincipal),"text");
    tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
    gtk_text_buffer_set_text (tb, "", 0);

/* Barra de estado */
    statusbar=lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("File opened."));

/* Ahora abro el fichero */

    n = strlen(manfile)-3;
    strncpy(extension,manfile+n,3);
    if (!strncmp(extension,".gz",3))
        in_gzip=1;
    else
        in_gzip=0;


    if ((f=gzopen((gchar *)manfile,"rb"))!=NULL)
    {
      while(!gzeof(f))
      {
        bytes_read=gzread(f,buffer,BUFFER_SIZE);
        if (bytes_read>0)
        {
            utf8 = NULL;
            if (g_utf8_validate(buffer, -1, NULL) == FALSE)
            {
                utf8 = g_locale_to_utf8(buffer, -1, NULL, NULL, NULL);
            }
            if (utf8 != NULL){
                strncpy(buffer,utf8, BUFFER_SIZE - 1);
                buffer[BUFFER_SIZE - 1] = 0;
            }
            gtk_text_buffer_insert_at_cursor(tb, buffer ,bytes_read);
        }
      }
      gzclose(f);
    }
    else
        mensaje(strerror(errno),GTK_MESSAGE_ERROR);
    if (open_file) gtk_widget_destroy (open_file);
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

  while (fgets(buf,80,f) != NULL)
  {
     if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

     if ((buf[0] != '#') && (!strncmp(variable,buf,strlen(variable))))
     {
        tok = strtok(buf,"=");
        tok = strtok(NULL,"=");
        fclose(f);
        return(tok);
     }
  }
  fclose(f);
  return((gchar *)NULL);
}
