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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <zlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <argz.h>
#include <sys/wait.h>

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define BUFFER_SIZE 8192
#define GTK_ENABLE_BROKEN

extern GtkWidget *wprincipal;
GtkWidget *open_file=NULL;
GtkWidget *save_file=NULL;
GtkWidget *buscar=NULL;
GtkWidget *about=NULL;
GtkWidget *exit_dialog=NULL;
GtkWidget *prefs=NULL;
GtkWidget *wizard=NULL;

static const gchar *MainWindowKey = "MainWindowKey";
gchar *filename=NULL;
gint in_gzip=0;

/* Funciones */
static void save_as(GtkWidget *main_window);
static void mensaje (gchar *msg,gint tipo);
static gchar *ReadConfFromFile(gchar *variable);
static void insert_label(const gchar *base,const gchar *text_info, GtkWidget *item);
static void help_without_gnome(GtkWidget *wid);
static void help_with_gnome(GtkWidget *wid);
static void open_man_file(GtkWidget *wid);


/* Eventos */
void
on_wprincipal_delete                  (GtkObject       *object,
                                        gpointer         user_data)
{
	if (exit_dialog==NULL)
		exit_dialog=create_exit_dialog();

	gtk_widget_show(exit_dialog);
}

void
on_cortar1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;
	
	text = lookup_widget (GTK_WIDGET (menuitem), "text");
	gtk_editable_cut_clipboard (GTK_EDITABLE (text));
	    
	statusbar = lookup_widget (GTK_WIDGET (menuitem), "statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text cutted."));
}


void
on_copiar1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;
	
	text = lookup_widget (GTK_WIDGET (menuitem), "text");
	gtk_editable_copy_clipboard (GTK_EDITABLE (text));
	    
	statusbar = lookup_widget (GTK_WIDGET (menuitem), "statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text copied."));
	          
}


void
on_pegar1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;
	
	text = lookup_widget (GTK_WIDGET (menuitem), "text");
	gtk_editable_paste_clipboard (GTK_EDITABLE (text));
	    
	statusbar = lookup_widget (GTK_WIDGET (menuitem), "statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text pasted."));
}


void
on_pagina_base1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    const gchar *base=_(".\\\"Created with GNOME Manpages Editor\n.\\\"\
http://sourceforge.net/projects/gmanedit2\n.\\\"Joop Stakenborg <pg4i@amsat.org>\n.\\\"\n\
.TH program <section number> \"<date>\" \"<title>\"\
\n\n.SH NAME\n\
<program> \\-program for...\n\n.SH SYNOPSIS\n.B <program>\n\
.RI [ options ]\n.br\n\n.SH DESCRIPTION\nThis manual page explains \
the\n.B <program>\nprogram. This program...\n.PP\n\\fB<program>\\fP is for...\n\
\n.SH OPTIONS\n.B\n.IP OPTION\nThis option...\n\n.SH LICENCE\n\n\
.SH BUGS\n\n.SH AUTHOR\n");

	insert_label(base,"Page created.",GTK_WIDGET(menuitem));
}


void
on_sair_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_novo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;

	text=lookup_widget(GTK_WIDGET(wprincipal),"text");
	gtk_editable_delete_text(GTK_EDITABLE(text),0,-1);
	
	statusbar = lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("New file."));

	if (filename) g_free(filename);
    in_gzip=0;
}


void
on_abrir1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
     open_file = create_fileselection ();
     gtk_widget_show (open_file);
}


void
on_gardar1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (save_file == NULL)
		save_file=create_save_file();
		
	gtk_object_set_data(GTK_OBJECT(save_file),MainWindowKey,menuitem);

	if (filename!=NULL)  /* Si ya tiene nombre asignado */
		save_as(wprincipal);
	else			
		gtk_widget_show(save_file);
		
	gdk_window_raise(save_file->window);

}


void
on_gardar_como1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (save_file == NULL)
		save_file=create_save_file();
		
	gtk_object_set_data(GTK_OBJECT(save_file),MainWindowKey,menuitem);

        gtk_widget_show(save_file);		
	gdk_window_raise(save_file->window);
}


void
on_sair4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}

void
on_statusbar1_destroy                  (GtkObject       *object,
                                        gpointer         user_data)
{

}

void
on_cancel_button1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
    gtk_widget_destroy (filesel);
}

void
on_ok_button1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *filesel;
    const gchar *temp;

    filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_hide(filesel);
    temp=gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    filename = g_strdup(temp);

    open_man_file(GTK_WIDGET(filesel));
}


static void save_as(GtkWidget *main_window)
{
	GtkWidget *text,*statusbar;
	FILE *f;
	gchar *datos;
	gint bytes_written;
	char extension[5];
	int n;

/* Esta comprabación se hace para permitir guardar un fichero comprimido */
/* sin comprimir */
	n = strlen(filename)-3;
        strncpy(extension,filename+n,3);
        if (!strncmp(extension,".gz",3))
            in_gzip=1;
        else
            in_gzip=0;


	
	gtk_object_set_data(GTK_OBJECT(save_file),MainWindowKey,main_window);
	text=lookup_widget(GTK_WIDGET(main_window),"text");
	
/* Barra de estado */
	statusbar=lookup_widget(GTK_WIDGET(main_window),"statusbar1");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);

	datos=gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

	if (in_gzip)
		f=gzopen(filename,"wb");
	else
		f=fopen(filename,"w");
	
	if ((f!=NULL) && (f!=Z_NULL))
	{
	  if (!in_gzip)
		bytes_written=fwrite(datos,sizeof(gchar),strlen(datos),f);
	  else
	  	bytes_written=gzwrite(f,datos,strlen(datos));
  	  gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("File saved."));		
	}
	else
	{
  	  gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("File NOT saved."));		
  	  mensaje(strerror(errno),GTK_MESSAGE_ERROR);
	}
	if (in_gzip)
	    gzclose(f);
        else
	    fclose(f);
}

void
on_titulo_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".TH program <section number> \"<date>\" \"<title>\"\
\n\n");

	insert_label(base,"Title writed.",GTK_WIDGET(menuitem));
}


void
on_nome1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH NAME program\n<program> \\- program for... \n");

        insert_label(base,"Section name writed.",GTK_WIDGET(menuitem));
}


void
on_sinopsis1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH SYNOPSIS\n");

 	insert_label(base,"Section synopsis writed.",GTK_WIDGET(menuitem));
}


void
on_descripcion1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH DESCRIPTION\n");

	insert_label(base,"Section description writed.",GTK_WIDGET(menuitem));
}


void
on_opcions1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH OPTIONS\n");

 	insert_label(base,"Section options writed.",GTK_WIDGET(menuitem));
}


void
on_valores_retornados1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH RETURN VALUES\n");

 	insert_label(base,"Section return values writed.",GTK_WIDGET(menuitem));
}


void
on_estados_de_saida1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH EXIT STATUS\n");
 	insert_label(base,"Section exit status.",GTK_WIDGET(menuitem));
}


void
on_uso1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH USAGE\n");

 	insert_label(base,"Section usage writed.",GTK_WIDGET(menuitem));
}


void
on_ficheiro1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH FILES\n");
 	insert_label(base,"Section files writed.",GTK_WIDGET(menuitem));
}


void
on_entorno1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH ENVIRONMENT\n");
 	insert_label(base,"Section environment writed.",GTK_WIDGET(menuitem));
}


void
on_diagnostico1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH DIAGNOSTICS\n");
 	insert_label(base,"Section diagnostics writed.",GTK_WIDGET(menuitem));
}


void
on_seguridade1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH SECURITY\n");
 	insert_label(base,"Section security writed.",GTK_WIDGET(menuitem));
}


void
on_notas1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH NOTES\n");
 	insert_label(base,"Section notes writed.",GTK_WIDGET(menuitem));
}


void
on_bugs1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH BUGS\n");
 	insert_label(base,"Section bugs writed.",GTK_WIDGET(menuitem));
}


void
on_autor1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH AUTHOR\n");
 	insert_label(base,"Section author writed.",GTK_WIDGET(menuitem));
}


void
on_exemplos1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH EXAMPLES\n");

 	insert_label(base,"Section examples writed.",GTK_WIDGET(menuitem));
}


void
on_ver_tamen1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH SEE ALSO\n");
 	insert_label(base,"Section see also writed.",GTK_WIDGET(menuitem));
}


void
on_conforme_a1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH CONFORMING TO\n");
 	insert_label(base,"Section conforming to writed.",GTK_WIDGET(menuitem));
}


void
on__1_activate                         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_otro1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SH <SECTION NAME>\n");
 	insert_label(base,"Section <other> writed.",GTK_WIDGET(menuitem));
}


void
on_ambas1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".AD B\n";
 	insert_label(base,"Right and left justify.",GTK_WIDGET(menuitem));
}


void
on_activar1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".FI\n";
 	insert_label(base,"Filling enabled.",GTK_WIDGET(menuitem));
}


void
on_desactivar1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".NF\n";
 	insert_label(base,"Filling disabled.",GTK_WIDGET(menuitem));    
}


void
on_bold1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".B\n";
 	insert_label(base,"Font Bold selected.",GTK_WIDGET(menuitem));    
}


void
on_bold_roman1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".BR\n";
 	insert_label(base,"Font Bold-Roman selected.",GTK_WIDGET(menuitem)); 
}


void
on_italica1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".I\n";
 	insert_label(base,"Font Italic selected.",GTK_WIDGET(menuitem)); 
}


void
on_italica_bold1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".IB\n";
 	insert_label(base,"Font Italic-Bold selected.",GTK_WIDGET(menuitem)); 
}


void
on_italica_roman1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".IR\n";
 	insert_label(base,"Font Italic-Roman selected.",GTK_WIDGET(menuitem)); 
}


void
on_roman_bold1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".RB\n";
 	insert_label(base,"Font Roman-Bold selected.",GTK_WIDGET(menuitem)); 
}


void
on_roman_italic1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".RI\n";
 	insert_label(base,"Font Roman-Italic selected.",GTK_WIDGET(menuitem)); 
}


void
on_small_bold1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".SB\n";
 	insert_label(base,"Font Small-Bold selected.",GTK_WIDGET(menuitem)); 
}


void
on_small1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".SM\n";
 	insert_label(base,"Font Small selected.",GTK_WIDGET(menuitem)); 
}


void
on_novo_parrafo1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".LP\n";
 	insert_label(base,"New paragraph.",GTK_WIDGET(menuitem)); 
}


void
on_comezo_parrafo1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".PP\n";
 	insert_label(base,"Paragraph start",GTK_WIDGET(menuitem)); 
}


void
on_comezo_marxen1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".RS <margin position>\n");
 	insert_label(base,"Margin position",GTK_WIDGET(menuitem)); 
}


void
on_finaliza_marxen1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".RE\n";
 	insert_label(base,"Relative margin end",GTK_WIDGET(menuitem)); 
}


void
on_tabulador_por_defecto1_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".DT\n";
 	insert_label(base,"Default tab",GTK_WIDGET(menuitem));
 }


void
on_subcabeceira1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".SS <NAME>\n");
 	insert_label(base,"Subheader inserted",GTK_WIDGET(menuitem));
}

void
on_a_href1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".UR <address>\n.UE\n");
 	insert_label(base,"Link inserted",GTK_WIDGET(menuitem));
}


void
on_a_name1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=_(".UN <address>\n");

 	insert_label(base,"A NAME section inserted",GTK_WIDGET(menuitem));
}


void
on_xustificacion_a_esquerda1_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".AD L\n";
 	insert_label(base,"Left justifier",GTK_WIDGET(menuitem));
 	
}

void
on_lineas_en_branco1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *base=".SP <n>\n";
	insert_label(base,"Insert \"n\"+1 empty lines.",GTK_WIDGET(menuitem));
}



void
on_save_file_destroy                   (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_widget_hide(save_file);
}


void
on_ok_button2_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *filesel,*main_window;
	const gchar *temp;
	
	filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
	main_window = gtk_object_get_data (GTK_OBJECT (filesel), MainWindowKey);
	gtk_widget_hide(save_file);
	temp=gtk_file_selection_get_filename(GTK_FILE_SELECTION(save_file));
	strcpy (filename, temp);

	save_as(main_window);
}


void
on_cancel_button2_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(save_file);
}

void
on_paxina_creada1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;
	FILE *f;
	gint bytes_written=0;
	gchar temp[30];
	gchar command[50],*datos;
	gint exitstatus;

/* I read conf file ~/.gmaneditrc */
	
	strcpy(temp, "/tmp/gmanedit.XXXXXX");
	mkstemp (temp);;
	datos=ReadConfFromFile("COMMAND");

	if (datos==NULL)
	{
		strcpy(command,"xterm -e man -l ");
		strcat(command,temp);
	}
	else
	{
		strcpy(command,datos);
		strcat(command," -l ");
		strcat(command,temp);
	}	

	text=lookup_widget(GTK_WIDGET(menuitem),"text");
	
/* Barra de estado */
	statusbar=lookup_widget(GTK_WIDGET(menuitem),"statusbar1");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("Page preview."));	

	datos=gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);
	if ((f=fopen(temp,"w"))!=NULL)
	{
 	     bytes_written=fwrite(datos,sizeof(gchar),strlen(datos),f);
	     fclose(f);
	}
	else
	     mensaje(strerror(errno),GTK_MESSAGE_ERROR);


	if (bytes_written>0)
		g_spawn_command_line_sync(command, NULL, NULL, &exitstatus, NULL);
	unlink(temp);
}

void
on_buscar_e_reemprazar1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
     if (buscar == NULL)
            buscar = create_wbuscar();
             
     gtk_widget_show(buscar);
}

void
on_opcions_programa1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *aux;
	GtkWidget *obj;
	gchar datos[255];
	
	if (prefs == NULL)
		prefs=create_wpreferences();
	
/* Window Options with last options */
/* First: command to view man pages */
	aux=ReadConfFromFile("COMMAND");
	if (aux != NULL)
	{
		strcpy(datos,aux);
		obj=lookup_widget(GTK_WIDGET(prefs),"entry_command");
		gtk_entry_set_text(GTK_ENTRY(obj),(gchar *)datos);
	}
/* Second: Internet Browser selector */
	aux=ReadConfFromFile("INTERNET_BROWSER");
	if (aux != NULL)
	{
		strcpy(datos,aux);
		obj=lookup_widget(GTK_WIDGET(prefs),"cbinet");
		gtk_entry_set_text(GTK_ENTRY(obj),(gchar *)datos);
	}

/* Third: if it use yelp */
	aux=ReadConfFromFile("USE_GNOME_HELP_BROWSER");
	if (aux != NULL)
	{
		strcpy(datos,aux);
		obj=lookup_widget(GTK_WIDGET(prefs),"chgnome_help");
		if (!strcmp(datos,"no"))
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(obj),FALSE);
		else	
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(obj),TRUE);
	}

	gtk_widget_show(prefs);
}


void
on_bbuscar_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_breemprazar_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_bpechar_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(buscar);
}


void
on_data1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text,*statusbar;
     time_t fech;
     struct tm *fecha;
     gchar cad[30];
     gchar *meses[]={_("January"),_("February"),_("March"),_("April"),
        		_("May"),_("June"),_("July"),_("August"),
        		_("September"),_("October"),_("November"),_("December")};

	
     time(&fech);
     fecha=localtime(&fech);
	
	if (fecha!=NULL)
	{
		sprintf(cad,"%d %s %d",fecha->tm_mday,meses[fecha->tm_mon],fecha->tm_year+1900);
		text=lookup_widget(GTK_WIDGET(menuitem),"text");	
		gtk_text_insert(text,NULL,NULL,NULL,cad,strlen(cad));
	
		statusbar = lookup_widget(GTK_WIDGET(menuitem),"statusbar1");
		gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
		gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Date inserted."));    
	}
	else
		mensaje(_("Cannot get system date!"),GTK_MESSAGE_ERROR);		
}

void
on_about2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
     create_about();
}

static void
mensaje (gchar *msg,gint tipo)
{
	GtkWidget *msgbox;
	
	msgbox=gtk_message_dialog_new(GTK_WINDOW(wprincipal),GTK_DIALOG_MODAL,tipo, GTK_BUTTONS_OK, msg,NULL);
	gtk_dialog_run (GTK_DIALOG (msgbox));
    gtk_widget_destroy (msgbox);
}

void
on_bdialog_yes_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_bdialog_no_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(exit_dialog);
}

void
on_bpok_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
	FILE *p;
	GtkWidget *entry,*ch;
	const gchar *entry_text=NULL;
	gchar cad[256],cad2[50];
	gchar *home,*browser;


/* I get home directory */
	home=(gchar *)getenv("HOME");
	strcpy(cad2,home);
	strcat(cad2,"/.gmaneditrc");

	entry=lookup_widget(GTK_WIDGET(button),"entry_command");
	entry_text=gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(cad,"# File created by gmanedit preferences option\n\nCOMMAND=");
	strcat(cad,entry_text);
	
	ch = lookup_widget(GTK_WIDGET(button),"chgnome_help");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	        strcat(cad,"\nUSE_GNOME_HELP_BROWSER=yes\n");
	else
        	strcat(cad,"\nUSE_GNOME_HELP_BROWSER=no\n");

	ch = lookup_widget(GTK_WIDGET(button), "cbinet");
	browser = gtk_editable_get_chars(GTK_EDITABLE(ch),0,-1);
     
	strcat(cad,"INTERNET_BROWSER=");
	strcat(cad,browser);
	
	if ((p=fopen(cad2,"w"))!=NULL)
	{
		fprintf(p,"%s\n",cad);
		fclose(p);	
	}
	else
		mensaje(strerror(errno),GTK_MESSAGE_ERROR);
	gtk_widget_hide(prefs);
}


void
on_bpcancel_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(prefs);
}

static gchar *ReadConfFromFile(gchar *variable)
{
  FILE *f;
  gchar readed[100];
  gchar *home;
  gchar *tok;
  
// Intento de abrir el fichero con la configuración personalizada  
  home = getenv("HOME");
  strcpy(readed,home);
  strcat(readed,"/.gmaneditrc");
  
  f = fopen(readed,"r");
  if (f == NULL)
  {
  	return((gchar *)NULL);
  }  
    
  while (fgets(readed,80,f) != NULL)
  {
// Lo siguiente quita los retornos de carro de las líneas leidas
     if (readed[strlen(readed)-1] == '\n')
     	readed[strlen(readed)-1] = '\0';
       
     if ((readed[0] != '#') && (!strncmp(variable,readed,strlen(variable))))
     {
        tok = strtok(readed,"=");
        tok = strtok(NULL,"=");
        fclose(f);
        return(tok);
     }    
  }
  fclose(f);
  return((gchar *)NULL);
}

void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *datos;
	
	datos=ReadConfFromFile("USE_GNOME_HELP_BROWSER");
	if (datos==NULL)
	{
		help_without_gnome(GTK_WIDGET(menuitem));
		return;
	}
	if (!strcmp(datos,"yes"))
		help_with_gnome(GTK_WIDGET(menuitem));
	else
		help_without_gnome(GTK_WIDGET(menuitem));		
}


void
on_select_all1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *text,*statusbar;
    gchar *datos;
	
	text = lookup_widget (GTK_WIDGET (menuitem), "text");

	datos=gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

	if (strlen(datos)>0)
	{
	gtk_editable_select_region(GTK_EDITABLE(text),0,strlen(datos));

	statusbar = lookup_widget (GTK_WIDGET (menuitem), "statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("Text selected."));
	}
}


void
on_new_wizard_page1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (wizard == NULL)
		wizard=create_wizard();
	gtk_widget_show(wizard);
	gtk_object_set_data(GTK_OBJECT(wizard),MainWindowKey,menuitem);
}

void
on_assistant_cancel                    (GtkAssistant *assistant,
                                        gpointer         user_data)
{
	gtk_widget_destroy (GTK_WIDGET(assistant));
}


void
on_dthe_end_finish                     (GtkAssistant *assistant,
                                        gpointer         user_data)
{
   GtkWidget *ch,*text,*druid,*main_window;
   gchar *nombre,*snumber,*date,*title,*author;
   gchar cadena[500];


/* Init for main_window */
   druid = gtk_widget_get_toplevel (GTK_WIDGET (assistant));
   main_window = gtk_object_get_data (GTK_OBJECT (druid), MainWindowKey);


/* First, I get man page name from step 1 */
   text = lookup_widget(GTK_WIDGET(assistant), "mname");
   nombre = gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

/* Date from step 1 */
   text = lookup_widget(GTK_WIDGET(assistant), "mdate");
   date = gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

/* Title from step 1 */
   text = lookup_widget(GTK_WIDGET(assistant), "mtitle");
   title = gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

/* Author from step 1 */
   text = lookup_widget(GTK_WIDGET(assistant), "mauthor");
   author = gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

/* Section number from combo */
   text = lookup_widget(GTK_WIDGET(assistant), "snumber");
   snumber = gtk_editable_get_chars(GTK_EDITABLE(text),0,-1);

/* Page Start */
   strcpy(cadena,".\\\"Created with GNOME Manpages Editor Wizard\n.\\\"\
http://sourceforge.net/projects/gmanedit2\n.\\\"Joop Stakenborg <pg4i@amsat.org>\n.\\\"\n");
   strcat(cadena,".TH ");
   strcat(cadena,nombre);
   strcat(cadena," ");
   strcat(cadena,snumber);
   strcat(cadena," \"");
   strcat(cadena,date);   
   strcat(cadena,"\" ");
   strcat(cadena,"\"");
   strcat(cadena,title);   
   strcat(cadena,"\"\n\n");
   
/* Section NAME */
   ch = lookup_widget (GTK_WIDGET (assistant), "chname");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
   {
	strcat(cadena,_(".SH NAME\n"));   
	strcat(cadena,nombre);
	strcat(cadena,_(" \\-program for...\n\n"));
   }

/* Section SYNOPSIS */
   ch = lookup_widget (GTK_WIDGET (assistant), "chsynopsis");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
   {
	strcat(cadena,_(".SH SYNOPSIS\n.B "));   
	strcat(cadena,nombre);
	strcat(cadena,_("\n.RI [ options ]\n.br\n\n"));
   }

/* Section DESCRIPTION */
   ch = lookup_widget (GTK_WIDGET (assistant), "chdescription");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
   {
	strcat(cadena,_(".SH DESCRIPTION\nThis manual page explains the\n.B "));   
	strcat(cadena,nombre);
	strcat(cadena,_("\nprogram. This program...\n.PP\n\\fB"));
	strcat(cadena,nombre);
	strcat(cadena,_("\\fP is for...\n\n"));
   }

/* Section OPTIONS */
   ch = lookup_widget (GTK_WIDGET (assistant), "choptions");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH OPTIONS\n.B\n.IP -OPTION\nThis option...\n\n"));   

/* Section USAGE */
   ch = lookup_widget (GTK_WIDGET (assistant), "chusage");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH USAGE\n\n"));   

/* Section FILES */
   ch = lookup_widget (GTK_WIDGET (assistant), "chfiles");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH FILES\n\n"));   

/* Section ENVIRONMENT */
   ch = lookup_widget (GTK_WIDGET (assistant), "chenvironment");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH ENVIRONMENT\n\n"));   

/* Section DIAGNOSTICS */
   ch = lookup_widget (GTK_WIDGET (assistant), "chdiagnostics");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH DIAGNOSTICS\n\n"));   

/* Section SECURITY */
   ch = lookup_widget (GTK_WIDGET (assistant), "chsecurity");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH SECURITY\n\n"));   
	
/* Section RETURN VALUES */
   ch = lookup_widget (GTK_WIDGET (assistant), "chreturnvalues");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,".SH RETURN VALUES\n\n");   

/* Section ERROR HANDLING */
   ch = lookup_widget (GTK_WIDGET (assistant), "chreturnvalues");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH ERROR HANDLING\n\n"));   

/* Section ERRORS */
   ch = lookup_widget (GTK_WIDGET (assistant), "cherrors");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH ERRORS\n\n"));   

/* Section CONFORMING TO */
   ch = lookup_widget (GTK_WIDGET (assistant), "chconformingto");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH CONFORMING TO\n\n"));   

/* Section NOTES */
   ch = lookup_widget (GTK_WIDGET (assistant), "chnotes");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH NOTES\n\n"));   

/* Section BUGS */
   ch = lookup_widget (GTK_WIDGET (assistant), "chbugs");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH BUGS\n\n"));   


/* Section SEE ALSO */
   ch = lookup_widget (GTK_WIDGET (assistant), "chseealso");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
	strcat(cadena,_(".SH SEE ALSO\n\n"));   
	
	
/* Section AUTHOR */
   ch = lookup_widget (GTK_WIDGET (assistant), "chauthor");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
   {
	strcat(cadena,_(".SH AUTHOR\n"));
	strcat(cadena,author);
   }

/* Hide wizard */
   gtk_widget_hide(wizard);
   
/* Insert all into gmanedit */
   insert_label(cadena,_("Wizard page created"),main_window);

/* Wizard closed */
   gtk_widget_destroy(wizard);
   wizard=NULL;
}

static
void insert_label(const gchar *base,const gchar *text_info, GtkWidget *item)
{
	GtkWidget *text,*statusbar;

	text=lookup_widget(GTK_WIDGET(item),"text");
	
	gtk_text_insert(text,NULL,NULL,NULL,
				_(base),strlen(base));
	
	statusbar = lookup_widget(GTK_WIDGET(item),"statusbar1");
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _(text_info));
}

void
on_home_page1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	pid_t p;
	gchar *browser;
	gchar cad[50];
	
	browser=ReadConfFromFile("INTERNET_BROWSER");
	if (browser==NULL)
		browser="netscape";
	strcpy(cad,browser);

	p=fork();
	if (p==0)
	if (execlp(cad,cad,"http://sourceforge.net/projects/gmanedit2",NULL) == -1)
			mensaje(_("Can not open Internet Browser"),GTK_MESSAGE_ERROR);	
}


void
on_chgnome_help_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
/*	GtkWidget *ch,*text;

	text = lookup_widget(GTK_WIDGET(togglebutton),"entry_command");
	ch = lookup_widget(GTK_WIDGET(togglebutton),"chgnome_help");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch))==TRUE)
		gtk_entry_set_editable(GTK_ENTRY(text),FALSE);
	else
		gtk_entry_set_editable(GTK_ENTRY(text),TRUE);
*/
}

static void help_with_gnome(GtkWidget *wid)
{
	GtkWidget *statusbar;
	pid_t p;

	p=fork();
	if (p==0)
	     if (execlp("yelp","yelp","man:man.7",NULL) == -1)
	     {	     
		printf("Warning, \"yelp\" not executed\n");
		exit(0);
	     }
/* Barra de estado */
	statusbar=lookup_widget(GTK_WIDGET(wid),"statusbar1");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("Man help."));		
}

static void help_without_gnome(GtkWidget *wid)
{
	GtkWidget *statusbar;
	gchar *datos;
	gchar temp[10],command[75];
	gint exitstatus;

// I read conf file ~/.gmaneditrc
	strcpy(temp," 7 man");
	datos=ReadConfFromFile("COMMAND");

	if (datos==NULL)
	{
		strcpy(command,"xterm -e man");
		strcat(command,temp);
	}
	else
	{
		strcpy(command,datos);
		strcat(command,temp);
	}

	g_spawn_command_line_sync(command, NULL, NULL, &exitstatus, NULL);

/* Barra de estado */
	statusbar=lookup_widget(GTK_WIDGET(wid),"statusbar1");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("Man help."));	
}

static void open_man_file(GtkWidget *widget)
{
    GtkWidget *statusbar,*text;
	gzFile *f;
	gchar buffer[BUFFER_SIZE];
	gint bytes_read;
	int n;
	char extension[10];


	text=lookup_widget(GTK_WIDGET(wprincipal),"text");
	gtk_editable_delete_text(GTK_EDITABLE(text),0,-1);
	
/* Barra de estado */
	statusbar=lookup_widget(GTK_WIDGET(wprincipal),"statusbar1");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),1);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),1,_("File opening."));	

/* Ahora abro el fichero */

	n = strlen(filename)-3;
	strncpy(extension,filename+n,3);
	if (!strncmp(extension,".gz",3))
	    in_gzip=1;
	else
	    in_gzip=0;


	if ((f=gzopen((gchar *)filename,"rb"))!=NULL)
	{
	  while(!gzeof(f))
	  {
		bytes_read=gzread(f,buffer,BUFFER_SIZE);
		if (bytes_read>0)
		gtk_text_insert(text,NULL,NULL,NULL,
				buffer,bytes_read);
	  }
  	  gzclose(f);
	}
	else
		mensaje(strerror(errno),GTK_MESSAGE_ERROR);
	gtk_widget_destroy (widget);
}

void
on_text_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *data,
                                        guint            info,
                                        guint            time,
                                        gpointer         user_data)
{
	if (data->length >= 0)
	{
	    if (!strncmp((gchar *)data->data,"file:",4))
	    {
		filename=(gchar *)data->data+5;
		filename[strlen(filename)-2]='\0';
		open_man_file(GTK_WIDGET(widget));
	    }

	}
	gtk_drag_finish(drag_context, TRUE, FALSE, time);
}


void
on_comments1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	const gchar *comm=".\\\" ";

	insert_label(comm,"Label Comments Inserted",GTK_WIDGET(menuitem));
}
