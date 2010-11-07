/*
 *  Copyright 2000-2001: Sergio Rua
 *  Copyright 2008 Joop Stakenborg <pg4i@amsat.org>
 *  Copyright 2008 Anibal Avelar <aavelar@cofradia.org>
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <zlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern GtkWidget *wprincipal;
GtkWidget *buscar = NULL;
GtkWidget *about  = NULL;
GtkWidget *prefs  = NULL;
GtkWidget *wizard = NULL;

gchar *filename = NULL;

/* Funciones */
static void save_as(gchar *name);
static void insert_label(const gchar *base,const gchar *text_info);
static void help_without_gnome(GtkWidget *wid);

/* Eventos */
void
on_cortar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text,*statusbar;

    text = lookup_widget (GTK_WIDGET (wprincipal), "text");
    GtkTextBuffer *b = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));
    gtk_text_buffer_cut_clipboard (b, gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), TRUE);
    statusbar = lookup_widget (GTK_WIDGET (wprincipal), "statusbar1");
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text cut."));
}


void
on_copiar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text,*statusbar;

    text = lookup_widget (GTK_WIDGET (wprincipal), "text");
    GtkTextBuffer *b = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));
    gtk_text_buffer_copy_clipboard (b,  gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    statusbar = lookup_widget (GTK_WIDGET (wprincipal), "statusbar1");
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text copied."));
}


void
on_pegar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text,*statusbar;

    text = lookup_widget (GTK_WIDGET (wprincipal), "text");
    GtkTextBuffer *b = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));
    gtk_text_buffer_paste_clipboard (b, gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), NULL, TRUE);

    statusbar = lookup_widget (GTK_WIDGET (wprincipal), "statusbar1");
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text pasted."));
}


void
on_pagina_base1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(
                          ".\\\"Created with GNOME Manpages Editor\n"
                          ".\\\"http://sourceforge.net/projects/gmanedit2\n\n"
                          ".\\\"Replace <program> with the program name, x with the Section Number\n"
                          ".TH <program> x \"<date>\" \"\" \"Linux User\'s Manual\"\n\n"
                          ".SH NAME\n<program> \\- program for...\n\n."
                          ".SH SYNOPSIS\n.B <program>\n.RI [ options ]\n.br\n\n"
                          ".SH DESCRIPTION\nThis manual page explains the \\fB<program>\\fP program."
                          " The \\fB<program>\\fP program is for...\n\n"
                          ".SH OPTIONS\n.IP \\fB\\-OPTION\\fP\nThis option...\n\n"
                          ".SH NOTES\n\n"
                          ".SH \"SEE ALSO\"\n");

    insert_label(base,"Page created.");
}


void
on_new_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text, *statusbar;
    GtkTextBuffer *buf;

    text = lookup_widget(GTK_WIDGET(wprincipal),"text");
    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));

    /* check if the text buffer contains unsaved changes */
    if (gtk_text_buffer_get_modified(buf)) {
        /* buffer has been modified, ask for permission to reset */
        int res = dialog_question(_("Start new document?"),
                                  _("<big><b>The document has been modified.</b></big>\n\n" \
                                    "Are you sure you want to start a new document?"),
                                  GTK_STOCK_DIALOG_QUESTION);

        if (res != GTK_RESPONSE_ACCEPT) {
            /* abort */
            return;
        }
    }

    /* set status bar */
    gtk_text_buffer_set_text (buf, "", -1);
    statusbar = lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("New file."));

    /* reset filename */
    if (filename) {
        g_free(filename);
        filename = NULL;
    }

    /* mark the text buffer as unaltered */
    gtk_text_buffer_set_modified(buf, FALSE);
}


void
on_open_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *open_file;
    gint res;

    /* check if the text buffer has been modified */
    if (document_modified(GTK_WINDOW(wprincipal), "text")) {
        /* ask if the changes shall be abandoned */
        res = dialog_question(_("Continue loading?"),
                              _("<big><b>The document has been modified.</b></big>\n\n" \
                                "Do you want to continue loading a file?"),
                              GTK_STOCK_DIALOG_QUESTION);

        if (res == GTK_RESPONSE_REJECT) {
            /* loading aborted */
            return;
        }
    }

    open_file = create_fileselection (GTK_WIDGET(wprincipal));
    gtk_widget_show (open_file);

    if (gtk_dialog_run (GTK_DIALOG (open_file)) == GTK_RESPONSE_ACCEPT) {
        if (filename != NULL) {
            g_free(filename);
        }

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (open_file));
        open_man_file(filename);
    }

    gtk_widget_destroy(open_file);
}


void
on_save_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *save_file = NULL;

    if (filename != NULL) {
        save_as(filename);
    }
    else {
        save_file = create_save_file(GTK_WIDGET(wprincipal), GTK_STOCK_SAVE);
        gtk_widget_show(save_file);
        if (gtk_dialog_run (GTK_DIALOG (save_file)) == GTK_RESPONSE_ACCEPT) {
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (save_file));
            gtk_widget_hide(save_file);
            save_as(filename);
        }

        gtk_widget_destroy (save_file);
    }
}


void
on_save_as_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *save_file;

    save_file = create_save_file(GTK_WIDGET(wprincipal), GTK_STOCK_SAVE_AS);
    gtk_widget_show(save_file);

    if (gtk_dialog_run (GTK_DIALOG (save_file)) == GTK_RESPONSE_ACCEPT) {
        if (filename != NULL) {
            g_free(filename);
        }

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (save_file));
        gtk_widget_hide(save_file);
        save_as(filename);
    }

    gtk_widget_destroy (save_file);
}


void
on_quit_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    /* check if the text buffer has been modified */
    if (document_modified(GTK_WINDOW(wprincipal), "text")) {
        int res;

        /* ask if the changes shall be abandoned */
        res = dialog_question(_("Continue quitting?"),
                              _("<big><b>The document has been modified.</b></big>\n\n" \
                                "Do you want to continue to quit?"),
                              GTK_STOCK_DIALOG_QUESTION);

        if (res == GTK_RESPONSE_REJECT) {
            /* exiting aborted */
            return;
        }
    }

    /* the answer was yes - terminate the program */
    gtk_main_quit();
}

static void save_as(gchar *name)
{
    GtkWidget *text, *statusbar;
    GtkTextBuffer *buf;
    GtkTextIter startiter, enditer;

    FILE *f;
    gchar *datos;
    gint bytes_written;
    gint in_gzip;

    /* this is done to allow saving a compressed file uncompressed */
    if (g_str_has_suffix(name, ".gz")) {
        in_gzip = 1;
    } else {
        in_gzip = 0;
    }

    /* get the text view */
    text = lookup_widget(GTK_WIDGET(wprincipal),"text");

    /* get the status bar */
    statusbar = lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 1);

    /* get the content of the text buffer */
    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));
    gtk_text_buffer_get_start_iter (buf, &startiter);
    gtk_text_buffer_get_end_iter (buf, &enditer);
    datos = gtk_text_buffer_get_text (buf, &startiter, &enditer, FALSE);

    /* open the file */
    if (in_gzip) {
        f = gzopen(name, "wb");
    } else {
        f = fopen(name, "w");
    }

    if ((f == NULL) || (f == Z_NULL)) {
        /* opening the file failed */
        gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, _("File NOT saved."));

        datos = g_strdup_printf(_("<big><b>An error occured while opening "
                                  "\"%s\" for writing:</b></big>\n\n%s"),
                                name, strerror(errno));

        dialog_message(datos, GTK_MESSAGE_ERROR);
        g_free(datos);

        return;
    }

    /* try to write to the file */
    if (in_gzip) {
        bytes_written = gzwrite(f, datos, strlen(datos));
    } else {
        bytes_written = fwrite(datos, sizeof(gchar), strlen(datos), f);
    }
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1 ,_("File saved."));

    /* close the file */
    if (in_gzip) {
        gzclose(f);
    } else {
        fclose(f);
    }

    /* free the memory returned by gtk_text_buffer_get_text() */
    g_free(datos);

    /* mark the text buffer as unmodified */
    gtk_text_buffer_set_modified(buf, FALSE);
}

void
on_titulo_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".TH <program> <section number> \"<date>\" \"\" \"Linux User\'s Manual\"\n\n");

    insert_label(base,"Title written.");
}


void
on_nome1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH NAME\n<program> \\- program for... \n");

    insert_label(base,"Section name written.");
}


void
on_sinopsis1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH SYNOPSIS\n");

    insert_label(base,"Section synopsis written.");
}


void
on_configuration_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH CONFIGURATION\n");

    insert_label(base,"Section configuration written.");
}

void
on_descripcion1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH DESCRIPTION\n");

    insert_label(base,"Section description written.");
}


void
on_opcions1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH OPTIONS\n");

    insert_label(base,"Section options written.");
}


void
on_valores_retornados1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH \"RETURN VALUE\"\n");

    insert_label(base,"Section return value written.");
}


void
on_estados_de_saida1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH \"EXIT STATUS\"\n");
    insert_label(base,"Section exit status written.");
}


void
on_errors_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH ERRORS\n");

    insert_label(base,"Section errors written.");
}


void
on_ficheiro1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH FILES\n");
    insert_label(base,"Section files written.");
}


void
on_entorno1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH ENVIRONMENT\n");
    insert_label(base,"Section environment written.");
}


void
on_versions_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH VERSIONS\n");
    insert_label(base,"Section versions written.");
}


void
on_notas1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH NOTES\n");
    insert_label(base,"Section notes written.");
}


void
on_bugs1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH BUGS\n");
    insert_label(base,"Section bugs written.");
}


void
on_exemplos1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH EXAMPLE\n");

    insert_label(base,"Section examples written.");
}


void
on_ver_tamen1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH \"SEE ALSO\"\n");
    insert_label(base,"Section see also written.");
}


void
on_conforme_a1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH \"CONFORMING TO\"\n");
    insert_label(base,"Section conforming to written.");
}


void
on_otro1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SH \"<SECTION NAME>\"\n");
    insert_label(base,"Section other written.");
}


void
on_ambas1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".AD B\n";
    insert_label(base,"Right and left justify.");
}


void
on_activar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".FI\n";
    insert_label(base,"Filling enabled.");
}


void
on_desactivar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".NF\n";
    insert_label(base,"Filling disabled.");
}


void
on_bold1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".B\n";
    insert_label(base,"Font Bold selected.");
}


void
on_bold_roman1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".BR\n";
    insert_label(base,"Font Bold-Roman selected.");
}


void
on_italica1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".I\n";
    insert_label(base,"Font Italic selected.");
}


void
on_italica_bold1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".IB\n";
    insert_label(base,"Font Italic-Bold selected.");
}


void
on_italica_roman1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".IR\n";
    insert_label(base,"Font Italic-Roman selected.");
}


void
on_roman_bold1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".RB\n";
    insert_label(base,"Font Roman-Bold selected.");
}


void
on_roman_italic1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".RI\n";
    insert_label(base,"Font Roman-Italic selected.");
}


void
on_small_bold1_activate(GtkMenuItem *menuitem, gpointer nuser_data)
{
    const gchar *base=".SB\n";
    insert_label(base,"Font Small-Bold selected.");
}


void
on_small1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".SM\n";
    insert_label(base,"Font Small selected.");
}


void
on_novo_parrafo1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".LP\n";
    insert_label(base,"New paragraph.");
}


void
on_comezo_parrafo1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".PP\n";
    insert_label(base,"Paragraph start.");
}


void
on_comezo_marxen1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".RS <margin position>\n");
    insert_label(base,"Margin position.");
}


void
on_finaliza_marxen1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".RE\n";
    insert_label(base,"Relative margin end.");
}


void
on_tabulador_por_defecto1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".DT\n";
    insert_label(base,"Default tab.");
}


void
on_subcabeceira1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=_(".SS \"<NAME>\"\n");
    insert_label(base,"Subheader inserted.");
}

void
on_xustificacion_a_esquerda1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *base=".AD L\n";
    insert_label(base,"Left justifier.");
}

void
on_view_created_page(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text, *statusbar;
    GtkTextIter startiter, enditer;
    gchar filename[31];
    const gchar *config = NULL;
    gchar *command = NULL;
    gchar *page = NULL;

    /* create temporary file */
    g_snprintf(filename, sizeof(filename) - 1, "/tmp/gmanedit.XXXXXX");
    mkstemp(filename);

    /* get the configured preview command from ~/.gmaneditrc */
    config = ReadConfFromFile("COMMAND");

    if (config == NULL) {
        /* no seting found - use default */
        command = g_strdup_printf("xterm -e man %s", filename);
    } else {
        /* use program specified by user */
        command = g_strdup_printf("%s %s", config, filename);
    }

    /* update the status bar */
    statusbar = lookup_widget(wprincipal, "statusbar1");
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 1);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1 ,_("Page preview."));

    /* get the current content of the text view */
    text = lookup_widget(wprincipal, "text");

    GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    gtk_text_buffer_get_start_iter(b, &startiter);
    gtk_text_buffer_get_end_iter(b, &enditer);
    page = gtk_text_buffer_get_text (b, &startiter, &enditer, FALSE);

    /* write the content of the text view to the temporary file */
    if (!g_file_set_contents(filename, page, -1, NULL)) {
        /* writing failed */
        dialog_message(strerror(errno), GTK_MESSAGE_ERROR);
    } else {
        /* show preview */
        g_spawn_command_line_async(command, NULL);

        /* wait one second to make sure the program has had enough time
         * to read the temporary file
         * FIXME: there must be a better way to ensure this! */
        sleep(1);
    }

    g_free(command);

    /* free the memory allocated by gtk_text_buffer_get_text() */
    g_free(page);

    /* remove the temporary file */
    unlink(filename);
}

void
on_buscar_e_reemprazar1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    if (buscar == NULL)
        buscar = create_wbuscar();

    gtk_widget_show(buscar);
}

void
on_opcions_programa1_activate(GtkMenuItem *menuitem, gpointer  user_data)
{
    const gchar *aux;
    GtkWidget *obj;
    gchar datos[255];

    prefs = create_wpreferences();
    aux = ReadConfFromFile("COMMAND");

    if (aux != NULL) {
        strncpy(datos,aux, sizeof datos - 1);
        datos[sizeof(datos) - 1] = 0;
        obj = lookup_widget(GTK_WIDGET(prefs),"entry_command");
        gtk_entry_set_text(GTK_ENTRY(obj),datos);
    }

    aux = ReadConfFromFile("FONT");

    if (aux != NULL) {
        obj = lookup_widget(GTK_WIDGET(prefs),"font");
        gtk_font_button_set_font_name(GTK_FONT_BUTTON(obj), aux);
    }

    gtk_widget_show_all(prefs);
}


void
on_bsearch_clicked(GtkButton *button, gpointer user_data)
{
    GtkTextIter titer, mstart, mend;
    gboolean found = FALSE;
    GtkWidget *text;
    GtkTextBuffer *buffer;
    GtkTextMark *pos;

    /* get search term combo box */
    GtkBin *scombo = GTK_BIN(lookup_widget(GTK_WIDGET(user_data), "search"));

    /* get the entry field of the combo box */
    GtkEntry *entry = GTK_ENTRY(gtk_bin_get_child(scombo));

    /* get the value of the entry field = the search term */
    const gchar *etext = gtk_entry_get_text(entry);

    /* get the search term list */
    GtkListStore *slist = GTK_LIST_STORE(lookup_widget(GTK_WIDGET(user_data),
                                         "slist"));

    if (strlen(etext) == 0) {
        /* empty entry field */
        return;
    }

    /* add the search term to the list */
    found = tree_list_add_unique(slist, etext);

    /* get the text buffer */
    text = lookup_widget(GTK_WIDGET(wprincipal), "text");
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));

    /* get the last position mark */
    pos = gtk_text_buffer_get_mark(buffer, "found");

    if (found && pos) {
        /* this is a previously used search term and the position  of
         * the last search result has been stored, thus start searching
         * at the position of the last finding */
        gtk_text_buffer_get_iter_at_mark(buffer, &titer, pos);
    } else {
        /* start from the beginning */
        gtk_text_buffer_get_start_iter(buffer, &titer);
    }

    if (pos) {
        /* delete the mark from a previous search */
        gtk_text_buffer_delete_mark(buffer, pos);
    }

    /* look for the entry field value in the text buffer */
    found = gtk_text_iter_forward_search (&titer, etext, 0,
                                          &mstart, &mend, NULL);

    if (found) {
        /* If found, hilight the text. */
        gtk_text_buffer_select_range (buffer, &mstart, &mend);

        /* set a mark in the text */
        pos = gtk_text_buffer_create_mark(buffer, "found", &mend, TRUE);

        /* scroll the text view to the found position */
        gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(text), pos);
    }
}


void
on_breplace_clicked(GtkButton *button, gpointer user_data)
{

}


void
on_bclose_clicked(GtkButton *button, gpointer user_data)
{
    gtk_widget_hide(buscar);
}


void
on_data1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text,*statusbar;
    GtkTextBuffer*  buffer;
    time_t fech;
    struct tm *fecha;
    gchar cad[30];
    gchar *meses[]= {_("January"),_("February"),_("March"),_("April"),
                     _("May"),_("June"),_("July"),_("August"),
                     _("September"),_("October"),_("November"),_("December")
                    };

    time(&fech);
    fecha=localtime(&fech);

    if (fecha!=NULL) {
        sprintf(cad,"%d %s %d",fecha->tm_mday,meses[fecha->tm_mon],fecha->tm_year+1900);
        text=lookup_widget(wprincipal,"text");
        if (GTK_IS_TEXT_VIEW (text)) {
            buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
            gtk_text_buffer_insert_at_cursor( buffer, cad ,strlen(cad));
        }
        statusbar = lookup_widget(wprincipal,"statusbar1");
        gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
        gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Date inserted."));
    } else
        dialog_message(_("Cannot get system date!"),GTK_MESSAGE_ERROR);
}

void
on_about2_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    create_about();
}

void
on_bpok_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *widget;
    FILE *p;
    gchar *buf;
    gchar *rcname;
    const gchar *entry_command = NULL;
    const gchar *font;
    PangoFontDescription *fdesc;

    rcname = g_build_filename (g_get_home_dir(), G_DIR_SEPARATOR_S,
                               ".gmaneditrc", NULL );

    widget = lookup_widget(prefs,"entry_command");
    entry_command = gtk_entry_get_text(GTK_ENTRY(widget));

    /* get the selected font */
    widget = lookup_widget(prefs, "font");
    font = gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget));

    /* set the text area's font */
    widget = lookup_widget(GTK_WIDGET(wprincipal),"text");
    fdesc = pango_font_description_from_string(font);
    gtk_widget_modify_font (widget, fdesc);
    pango_font_description_free(fdesc);

    /* store the settings */
    buf = g_strdup_printf("# File created by gmanedit preferences option\n\n" \
                          "COMMAND=%s\nFONT=%s",
                          entry_command, font);

    if ((p = fopen(rcname, "w")) != NULL) {
        fprintf(p, "%s\n", buf);
        fclose(p);
    } else {
        dialog_message(strerror(errno), GTK_MESSAGE_ERROR);
    }

    /* free allocated ressources */
    g_free(buf);
    g_free (rcname);
    gtk_widget_destroy(prefs);
}


void
on_bpcancel_clicked(GtkButton *button, gpointer user_data)
{
    gtk_widget_destroy(prefs);
}


void
on_help1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    help_without_gnome(wprincipal);
}


void
on_howto_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    OpenWebsite("http://www.schweikhardt.net/Man-Page.html");
}


void
on_select_all1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *text,*statusbar;
    gchar *datos;

    text = lookup_widget (GTK_WIDGET (wprincipal), "text");

    GtkTextBuffer *b = gtk_text_view_get_buffer (GTK_TEXT_VIEW(text));
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_start_iter (b, &startiter);
    gtk_text_buffer_get_end_iter (b, &enditer);
    datos=gtk_text_buffer_get_text (b, &startiter, &enditer, FALSE);

    if (strlen(datos)>0) {
        gtk_text_buffer_select_range (b, &startiter, &enditer);
        statusbar = lookup_widget (GTK_WIDGET (wprincipal), "statusbar1");
        gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
        gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text selected."));
    }
}


void
on_new_wizard_page1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    if (wizard == NULL)
        wizard=create_wizard();
    gtk_widget_show(wizard);
}

void
on_assistant_cancel(GtkAssistant *assistant, gpointer user_data)
{
    gtk_widget_destroy (GTK_WIDGET(assistant));
    wizard = NULL;
}


void
on_dthe_end_finish(GtkAssistant *assistant, gpointer user_data)
{
    GtkWidget *ch, *text, *druid;
    gchar *name, *date, *title;
    GString *page;
    gint number;

    /* Init for main_window */
    druid = gtk_widget_get_toplevel (GTK_WIDGET (assistant));

    /* First, I get man page name from step 1 */
    text = lookup_widget(GTK_WIDGET(assistant), "mname");
    name = gtk_editable_get_chars(GTK_EDITABLE(text), 0, -1);

    /* Date from step 1 */
    text = lookup_widget(GTK_WIDGET(assistant), "mdate");
    date = gtk_editable_get_chars(GTK_EDITABLE(text), 0, -1);

    /* Title from step 1 */
    text = lookup_widget(GTK_WIDGET(assistant), "mtitle");
    title = gtk_editable_get_chars(GTK_EDITABLE(text), 0, -1);

    /* Section number from combo */
    ch = lookup_widget(GTK_WIDGET(assistant), "combo1");
    number = gtk_combo_box_get_active (GTK_COMBO_BOX(ch));

    /* Page Start */
    page = g_string_new(".\\\"Created with GNOME Manpages Editor Wizard\n"
           ".\\\"http://sourceforge.net/projects/gmanedit2\n");

    g_string_append_printf(page, ".TH %s %d \"%s\" \"\" \"%s\"\n\n",
                           name, number + 1, date, title);

    /* Section NAME */
    ch = lookup_widget (GTK_WIDGET (assistant), "chname");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE) {
        g_string_append_printf(page, _(".SH NAME\n"));
        g_string_append_printf(page, _("%s \\- program for...\n\n"), name);
    }

    /* Section SYNOPSIS */
    ch = lookup_widget (GTK_WIDGET (assistant), "chsynopsis");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE) {
        g_string_append_printf(page, _(".SH SYNOPSIS\n.B %s"), name);
        g_string_append_printf(page, _("\n.RI [ options ]\n.br\n\n"));
    }

    /* Section CONFIGURATION */
    ch = lookup_widget (GTK_WIDGET (assistant), "chconfiguration");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page,  _(".SH CONFIGURATION\n\n"));

    /* Section DESCRIPTION */
    ch = lookup_widget (GTK_WIDGET (assistant), "chdescription");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE) {
        g_string_append_printf(page, _(".SH DESCRIPTION\n"));
        g_string_append_printf(page, _("This manual page explains the\n.B %s"), name);
        g_string_append_printf(page, _("\nprogram. This program...\n.PP\n"));
        g_string_append_printf(page, _("\\fB%s\\fP is for...\n\n"), name);
    }

    /* Section OPTIONS */
    ch = lookup_widget (GTK_WIDGET (assistant), "choptions");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH OPTIONS\n.B\n.IP -OPTION\nThis option...\n\n"));

    /* Section EXIT STATUS */
    ch = lookup_widget (GTK_WIDGET (assistant), "chexitstatus");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH \"EXIT STATUS\"\n\n"));

    /* Section RETURN VALUE */
    ch = lookup_widget (GTK_WIDGET (assistant), "chreturnvalues");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, ".SH \"RETURN VALUE\"\n\n");

    /* Section ERRORS */
    ch = lookup_widget (GTK_WIDGET (assistant), "cherrors");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH ERRORS\n\n"));

    /* Section ENVIRONMENT */
    ch = lookup_widget (GTK_WIDGET (assistant), "chenvironment");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH ENVIRONMENT\n\n"));

    /* Section FILES */
    ch = lookup_widget (GTK_WIDGET (assistant), "chfiles");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH FILES\n\n"));

    /* Section VERSIONS */
    ch = lookup_widget (GTK_WIDGET (assistant), "chversions");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH VERSIONS\n\n"));

    /* Section CONFORMING TO */
    ch = lookup_widget (GTK_WIDGET (assistant), "chconformingto");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH \"CONFORMING TO\"\n\n"));

    /* Section NOTES */
    ch = lookup_widget (GTK_WIDGET (assistant), "chnotes");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH NOTES\n\n"));

    /* Section BUGS */
    ch = lookup_widget (GTK_WIDGET (assistant), "chbugs");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH BUGS\n\n"));

    /* Section EXAMPLE */
    ch = lookup_widget (GTK_WIDGET (assistant), "chexample");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH EXAMPLE\n\n"));

    /* Section SEE ALSO */
    ch = lookup_widget (GTK_WIDGET (assistant), "chseealso");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch)) == TRUE)
        g_string_append_printf(page, _(".SH \"SEE ALSO\"\n\n"));

    /* Hide wizard */
    gtk_widget_hide(wizard);

    /* Insert the generated page into gmanedit */
    insert_label(page->str, _("Wizard page created."));

    /* free allocated memory */
    g_free(name);
    g_free(date);
    g_free(title);
    g_string_free(page, TRUE);

    /* Wizard closed */
    gtk_widget_destroy(wizard);
    wizard = NULL;
}

static
void insert_label(const gchar *base, const gchar *text_info)
{
    GtkWidget *text, *statusbar;
    GtkTextBuffer *buffer;

    text = lookup_widget(GTK_WIDGET(wprincipal),"text");
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
    gtk_text_buffer_insert_at_cursor(buffer, base, -1);

    statusbar = lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, text_info);
}

static void help_without_gnome(GtkWidget *wid)
{
    GtkWidget *statusbar;
    const gchar *datos;
    gchar temp[10],command[1024];

    strcpy(temp," 7 man");
    datos = ReadConfFromFile("COMMAND");

    if (datos==NULL)
        snprintf(command, sizeof command, "xterm -e man %s", temp);
    else
        snprintf(command, sizeof command, "%s %s", datos, temp);

    g_spawn_command_line_async(command, NULL);

    /* Barra de estado */
    statusbar=lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("Man help."));
}

void
on_text_drag_data_received(GtkWidget *widget, GdkDragContext  *drag_context,
                           gint x, gint y, GtkSelectionData *data,
                           guint info, guint time, gpointer user_data)
{
    if (data->length >= 0) {

        if (g_str_has_prefix ((gchar *)data->data, "file://")) {
            if (filename != NULL) {
                g_free(filename);
            }
            filename = g_strdup((gchar *)data->data + 7);
            filename[strlen(filename) - 2] = '\0';
            open_man_file(filename);
        }
    }

    gtk_drag_finish(drag_context, TRUE, FALSE, time);
}


void
on_comments1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    const gchar *comm=".\\\" ";

    insert_label(comm,"Label Comments Inserted.");
}
