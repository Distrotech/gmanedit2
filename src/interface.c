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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GTK_ENABLE_BROKEN 1

/* Normal items */
static const GtkActionEntry entries[] = {
  { "FileMenu", NULL, "_File" },
  { "New", GTK_STOCK_NEW, N_("_New"), NULL, "", on_novo1_activate },
  { "Wizard", NULL, N_("New _Wizard page"), NULL, "", on_new_wizard_page1_activate },
  { "Open", GTK_STOCK_OPEN, N_("_Open..."), NULL, "", on_abrir1_activate },
  { "Save", GTK_STOCK_SAVE, N_("_Save"), NULL, "", on_gardar1_activate },
  { "SaveAs", GTK_STOCK_SAVE_AS, N_("Save _As..."), NULL, "", on_gardar_como1_activate },
  { "Quit", GTK_STOCK_QUIT, N_("_Quit"), NULL, "", on_sair4_activate },
  { "EditMenu", NULL, "_Edit" },
  { "Cut", GTK_STOCK_CUT, N_("Cu_t"), NULL, "", on_cortar1_activate },
  { "Copy", GTK_STOCK_COPY, N_("_Copy"), NULL, "", on_copiar1_activate },
  { "Paste", GTK_STOCK_PASTE, N_("_Paste"), NULL, "", on_pegar1_activate },
  { "SelectAll", GTK_STOCK_SELECT_ALL, N_("_Select All"), NULL, "", on_select_all1_activate },
};

static const char *ui_description =
"<ui>"
"  <menubar name='MainMenu'>"
"        <menu action='FileMenu'>"
"               <menuitem action='New'/>"
"               <menuitem action='Wizard'/>"
"               <menuitem action='Open'/>"
"               <menuitem action='Save'/>"
"               <menuitem action='SaveAs'/>"
"               <separator name='sep1'/>"
"               <menuitem action='Quit'/>"
"        </menu>"
"        <menu action='EditMenu'>"
"               <menuitem action='Cut'/>"
"               <menuitem action='Copy'/>"
"               <menuitem action='Paste'/>"
"               <menuitem action='SelectAll'/>"
"        </menu>"
"  </menubar>"
"  <toolbar name='ToolBar'>"
"        <toolitem action='New'/>"
"        <toolitem action='Open'/>"
"        <toolitem action='Save'/>"
"        <separator name='septool1'/>"
"        <toolitem action='Cut'/>"
"        <toolitem action='Copy'/>"
"        <toolitem action='Paste'/>"
"        <separator name='septool2'/>"
"        <toolitem action='Quit'/>"
"  </toolbar>"
"</ui>";

static GnomeUIInfo mficheiro1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_NEW_ITEM (N_("_New"), NULL, on_novo1_activate, NULL),
  {
    GNOME_APP_UI_ITEM, N_("New _Wizard page"),
    NULL,
    (gpointer) on_new_wizard_page1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    GDK_W, (GdkModifierType) GDK_CONTROL_MASK, NULL
  },
  GNOMEUIINFO_MENU_OPEN_ITEM (on_abrir1_activate, NULL),
  GNOMEUIINFO_MENU_SAVE_ITEM (on_gardar1_activate, NULL),
  GNOMEUIINFO_MENU_SAVE_AS_ITEM (on_gardar_como1_activate, NULL),
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_MENU_EXIT_ITEM (on_sair4_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo edicion1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_CUT_ITEM (on_cortar1_activate, NULL),
  GNOMEUIINFO_MENU_COPY_ITEM (on_copiar1_activate, NULL),
  GNOMEUIINFO_MENU_PASTE_ITEM (on_pegar1_activate, NULL),
  {
    GNOME_APP_UI_ITEM, N_("_Select All"),
    NULL,
    (gpointer) on_select_all1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    GDK_E, (GdkModifierType) GDK_CONTROL_MASK, NULL
  },
//  GNOMEUIINFO_MENU_FIND_ITEM (on_buscar_e_reemprazar1_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo seccion1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Name"),
    NULL,
    (gpointer) on_nome1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Synopsis"),
    NULL,
    (gpointer) on_sinopsis1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Description"),
    NULL,
    (gpointer) on_descripcion1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Options"),
    NULL,
    (gpointer) on_opcions1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Return values"),
    NULL,
    (gpointer) on_valores_retornados1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Exit status"),
    NULL,
    (gpointer) on_estados_de_saida1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Usage"),
    NULL,
    (gpointer) on_uso1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("File"),
    NULL,
    (gpointer) on_ficheiro1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Environment"),
    NULL,
    (gpointer) on_entorno1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Diagnostics"),
    NULL,
    (gpointer) on_diagnostico1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Security"),
    NULL,
    (gpointer) on_seguridade1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Notes"),
    NULL,
    (gpointer) on_notas1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Bugs"),
    NULL,
    (gpointer) on_bugs1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Author"),
    NULL,
    (gpointer) on_autor1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Examples"),
    NULL,
    (gpointer) on_exemplos1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("See also"),
    NULL,
    (gpointer) on_ver_tamen1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Conforming to"),
    NULL,
    (gpointer) on_conforme_a1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_SEPARATOR,
  {
    GNOME_APP_UI_ITEM, N_("Other"),
    NULL,
    (gpointer) on_otro1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo justificacion_esquerda1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Left"),
    NULL,
    (gpointer) on_xustificacion_a_esquerda1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    GDK_L, (GdkModifierType) GDK_CONTROL_MASK, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Both"),
    NULL,
    (gpointer) on_ambas1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo recheo1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Enable"),
    NULL,
    (gpointer) on_activar1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Disable"),
    NULL,
    (gpointer) on_desactivar1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo fonte1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Bold"),
    NULL,
    (gpointer) on_bold1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Bold - Roman"),
    NULL,
    (gpointer) on_bold_roman1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Italic"),
    NULL,
    (gpointer) on_italica1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Italic - Bold"),
    NULL,
    (gpointer) on_italica_bold1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Italic - Roman"),
    NULL,
    (gpointer) on_italica_roman1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Roman - Bold"),
    NULL,
    (gpointer) on_roman_bold1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Roman - Italic"),
    NULL,
    (gpointer) on_roman_italic1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Small - Bold"),
    NULL,
    (gpointer) on_small_bold1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Small"),
    NULL,
    (gpointer) on_small1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo parrafos1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("New paragraph"),
    NULL,
    (gpointer) on_novo_parrafo1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Paragraph start"),
    NULL,
    (gpointer) on_comezo_parrafo1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo marxen1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Margin start"),
    N_("Start a margin"),
    (gpointer) on_comezo_marxen1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Margin end"),
    NULL,
    (gpointer) on_finaliza_marxen1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo hypertext1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("A HREF"),
    NULL,
    (gpointer) on_a_href1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("A NAME"),
    NULL,
    (gpointer) on_a_name1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo outros1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Default tabs"),
    NULL,
    (gpointer) on_tabulador_por_defecto1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Subheader"),
    N_("Like .SH"),
    (gpointer) on_subcabeceira1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Empty lines"),
    NULL,
    (gpointer) on_lineas_en_branco1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Comments"),
    NULL,
    (gpointer) on_comments1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo insertar1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Basic page"),
    NULL,
    (gpointer) on_pagina_base1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Title"),
    NULL,
    (gpointer) on_titulo_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Date"),
    NULL,
    (gpointer) on_data1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Section"),
    NULL,
    seccion1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Justify"),
    NULL,
    justificacion_esquerda1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Filling"),
    NULL,
    recheo1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Font"),
    NULL,
    fonte1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Paragraphs"),
    NULL,
    parrafos1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Margin"),
    NULL,
    marxen1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Hypertext"),
    N_("Not available"),
    hypertext1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("Others"),
    NULL,
    outros1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo ver1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("View created page"),
    NULL,
    (gpointer) on_paxina_creada1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_opcions_programa1_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo separator2_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("H_elp"),
    NULL,
    (gpointer) on_help1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("_Home Page"),
    NULL,
    (gpointer) on_home_page1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_MENU_ABOUT_ITEM (on_about2_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_FILE_TREE (mficheiro1_menu_uiinfo),
  GNOMEUIINFO_MENU_EDIT_TREE (edicion1_menu_uiinfo),
  {
    GNOME_APP_UI_SUBTREE, N_("_Insert"),
    NULL,
    insertar1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_MENU_VIEW_TREE (ver1_menu_uiinfo),
  GNOMEUIINFO_MENU_HELP_TREE (separator2_menu_uiinfo),
  GNOMEUIINFO_END
};

GtkWidget*
create_wprincipal (void)
{
  GtkWidget *wprincipal;
  GtkWidget *vbox1;
  GtkWidget *menu;
  GtkWidget *toolbar1;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *bnew;
  GtkWidget *bopen;
  GtkWidget *bsave;
  GtkWidget *vseparator1;
  GtkWidget *bcut;
  GtkWidget *bcopy;
  GtkWidget *bpaste;
  GtkWidget *vseparator2;
  GtkWidget *bexit;
  GtkWidget *scrolledwindow1;
  GtkWidget *text;
  GtkWidget *statusbar1;

  wprincipal = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (wprincipal), "wprincipal", wprincipal);
  gtk_widget_set_usize (wprincipal, 640, 500);
  gtk_window_set_title (GTK_WINDOW (wprincipal), _("Gmanedit - Gnome manpages editor"));
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (wprincipal), icon_pixbuf);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (wprincipal), vbox1);
  
  GtkActionGroup *action_group = gtk_action_group_new ("MenuActions");
  gtk_action_group_set_translation_domain (action_group, PACKAGE);
  gtk_action_group_add_actions (action_group, entries,
                G_N_ELEMENTS (entries), wprincipal);
  GtkUIManager *ui_manager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
  GtkAccelGroup *accel_group = gtk_ui_manager_get_accel_group (ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (wprincipal), accel_group);
  
  gtk_ui_manager_add_ui_from_string (ui_manager, ui_description, -1, NULL);
  GtkWidget *menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);
  
  GtkWidget *handlebox = gtk_handle_box_new ();
  gtk_box_pack_start (GTK_BOX (vbox1), handlebox, FALSE, FALSE, 0);
  GtkWidget *toolbar = gtk_ui_manager_get_widget (ui_manager, "/ToolBar");
  gtk_container_add (GTK_CONTAINER (handlebox), toolbar);
  gtk_widget_show_all (handlebox);
/*
  menu = gtk_menu_bar_new ();
  gtk_widget_ref (menu);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "menu", menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menu);
  gtk_box_pack_start (GTK_BOX (vbox1), menu, FALSE, FALSE, 0);
  gnome_app_fill_menu (GTK_MENU_SHELL (menu), menu_uiinfo,
                       NULL, FALSE, 0);

  gtk_widget_ref (menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "mficheiro1",
                            menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "novo1",
                            mficheiro1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "new_wizard_page1",
                            mficheiro1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "abrir1",
                            mficheiro1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "gardar1",
                            mficheiro1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "gardar_como1",
                            mficheiro1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "separator1",
                            mficheiro1_menu_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (mficheiro1_menu_uiinfo[6].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "sair",
                            mficheiro1_menu_uiinfo[6].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "edicion1",
                            menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (edicion1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "cortar1",
                            edicion1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (edicion1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "copiar1",
                            edicion1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (edicion1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "pegar1",
                            edicion1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (edicion1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "select_all1",
                            edicion1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (edicion1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "buscar_e_reemprazar1",
                            edicion1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "insertar1",
                            menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "pagina_base1",
                            insertar1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "titulo",
                            insertar1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "data1",
                            insertar1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "seccion1",
                            insertar1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "nome1",
                            seccion1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "sinopsis1",
                            seccion1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "descripcion1",
                            seccion1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "opcions1",
                            seccion1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "valores_retornados1",
                            seccion1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "estados_de_saida1",
                            seccion1_menu_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[6].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "uso1",
                            seccion1_menu_uiinfo[6].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[7].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "ficheiro1",
                            seccion1_menu_uiinfo[7].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[8].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "entorno1",
                            seccion1_menu_uiinfo[8].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[9].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "diagnostico1",
                            seccion1_menu_uiinfo[9].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[10].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "seguridade1",
                            seccion1_menu_uiinfo[10].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[11].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "notas1",
                            seccion1_menu_uiinfo[11].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[12].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bugs1",
                            seccion1_menu_uiinfo[12].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[13].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "autor1",
                            seccion1_menu_uiinfo[13].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[14].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "exemplos1",
                            seccion1_menu_uiinfo[14].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[15].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "ver_tamen1",
                            seccion1_menu_uiinfo[15].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[16].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "conforme_a1",
                            seccion1_menu_uiinfo[16].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[17].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "_1",
                            seccion1_menu_uiinfo[17].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (seccion1_menu_uiinfo[18].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "outro1",
                            seccion1_menu_uiinfo[18].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "justificacion_esquerda1",
                            insertar1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (justificacion_esquerda1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "xustificacion_a_esquerda1",
                            justificacion_esquerda1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (justificacion_esquerda1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "xustificacion_ambas1",
                            justificacion_esquerda1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "recheo1",
                            insertar1_menu_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (recheo1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "activar1",
                            recheo1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (recheo1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "desactivar1",
                            recheo1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[6].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "fonte1",
                            insertar1_menu_uiinfo[6].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bold1",
                            fonte1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bold_roman1",
                            fonte1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "italica1",
                            fonte1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "italica_bold1",
                            fonte1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "italica_roman1",
                            fonte1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "roman_bold1",
                            fonte1_menu_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[6].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "roman_italic1",
                            fonte1_menu_uiinfo[6].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[7].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "small_bold1",
                            fonte1_menu_uiinfo[7].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (fonte1_menu_uiinfo[8].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "small1",
                            fonte1_menu_uiinfo[8].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[7].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "parrafos1",
                            insertar1_menu_uiinfo[7].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (parrafos1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "novo_parrafo1",
                            parrafos1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (parrafos1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "comezo_parrafo1",
                            parrafos1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[8].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "marxen1",
                            insertar1_menu_uiinfo[8].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (marxen1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "comezo_marxen1",
                            marxen1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (marxen1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "finaliza_marxen1",
                            marxen1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[9].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "hypertext1",
                            insertar1_menu_uiinfo[9].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (hypertext1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "a_href1",
                            hypertext1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (hypertext1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "a_name1",
                            hypertext1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (insertar1_menu_uiinfo[10].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "outros1",
                            insertar1_menu_uiinfo[10].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (outros1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "tabulador_por_defecto1",
                            outros1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (outros1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "subcabeceira1",
                            outros1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (outros1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "lineas_en_branco1",
                            outros1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (outros1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "comments1",
                            outros1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "ver1",
                            menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (ver1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "paxina_creada1",
                            ver1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (ver1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "opcions_programa1",
                            ver1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "separator2",
                            menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (separator2_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "help1",
                            separator2_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (separator2_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "home_page1",
                            separator2_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (separator2_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "about2",
                            separator2_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_ref (toolbar1);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "toolbar1", toolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_NEW, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bnew = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("New"),
                                _("New page"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bnew);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bnew", bnew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bnew);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_OPEN, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bopen = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Open"),
                                _("Open page"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bopen);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bopen", bopen,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bopen);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_SAVE, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bsave = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Save"),
                                _("Save current page"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bsave);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bsave", bsave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bsave);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator1);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "vseparator1", vseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar1), vseparator1, NULL, NULL);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_CUT, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bcut = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Cut"),
                                _("Cut selected text"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bcut);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bcut", bcut,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bcut);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_COPY, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bcopy = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Copy"),
                                _("Copy selected text"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bcopy);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bcopy", bcopy,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bcopy);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_PASTE, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bpaste = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Paste"),
                                _("Paste from clipboard"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bpaste);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bpaste", bpaste,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bpaste);

  vseparator2 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator2);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "vseparator2", vseparator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator2);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar1), vseparator2, NULL, NULL);

  tmp_toolbar_icon = gtk_image_new_from_stock (GNOME_STOCK_PIXMAP_EXIT, GTK_ICON_SIZE_SMALL_TOOLBAR);
  bexit = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Exit"),
                                _("Exit from gmanedit"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (bexit);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "bexit", bexit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bexit);
*/
  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text = GTK_WIDGET(gtk_text_new (NULL, NULL));
  gtk_widget_ref (text);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "text", text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text);
  gtk_text_set_editable (GTK_EDITABLE (text), TRUE);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_ref (statusbar1);
  gtk_object_set_data_full (GTK_OBJECT (wprincipal), "statusbar1", statusbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (wprincipal), "delete_event",
                      GTK_SIGNAL_FUNC (on_wprincipal_delete),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bnew), "clicked",
                      GTK_SIGNAL_FUNC (on_bnew_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bopen), "clicked",
                      GTK_SIGNAL_FUNC (on_bopen_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bsave), "clicked",
                      GTK_SIGNAL_FUNC (on_bsave_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bcut), "clicked",
                      GTK_SIGNAL_FUNC (on_bcut_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bcopy), "clicked",
                      GTK_SIGNAL_FUNC (on_bcopy_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bpaste), "clicked",
                      GTK_SIGNAL_FUNC (on_bpaste_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bexit), "clicked",
                      GTK_SIGNAL_FUNC (on_bexit_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (text), "drag_data_received",
                      GTK_SIGNAL_FUNC (on_text_drag_data_received),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (statusbar1), "destroy",
                      GTK_SIGNAL_FUNC (on_statusbar1_destroy),
                      NULL);

  gtk_widget_grab_focus (text);
  return wprincipal;
}

GtkWidget*
create_fileselection (void)
{
  GtkWidget *fileselection;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;

  fileselection = gtk_file_selection_new (_("Gmanedit - Select file"));
  gtk_object_set_data (GTK_OBJECT (fileselection), "fileselection", fileselection);
  gtk_container_set_border_width (GTK_CONTAINER (fileselection), 10);
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (fileselection), icon_pixbuf);

  ok_button1 = GTK_FILE_SELECTION (fileselection)->ok_button;
  gtk_object_set_data (GTK_OBJECT (fileselection), "ok_button1", ok_button1);
  gtk_widget_show (ok_button1);

  cancel_button1 = GTK_FILE_SELECTION (fileselection)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (fileselection), "cancel_button1", cancel_button1);
  gtk_widget_show (cancel_button1);

  gtk_signal_connect (GTK_OBJECT (fileselection), "destroy",
                      GTK_SIGNAL_FUNC (on_fileselection_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_ok_button1_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button1_clicked),
                      NULL);

  return fileselection;
}

GtkWidget*
create_save_file (void)
{
  GtkWidget *save_file;
  GtkWidget *ok_button2;
  GtkWidget *cancel_button2;

  save_file = gtk_file_selection_new (_("Gmanedit - Saving file"));
  gtk_object_set_data (GTK_OBJECT (save_file), "save_file", save_file);
  gtk_container_set_border_width (GTK_CONTAINER (save_file), 10);
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (save_file), icon_pixbuf);

  ok_button2 = GTK_FILE_SELECTION (save_file)->ok_button;
  gtk_object_set_data (GTK_OBJECT (save_file), "ok_button2", ok_button2);
  gtk_widget_show (ok_button2);

  cancel_button2 = GTK_FILE_SELECTION (save_file)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (save_file), "cancel_button2", cancel_button2);
  gtk_widget_show (cancel_button2);

  gtk_signal_connect (GTK_OBJECT (save_file), "destroy",
                      GTK_SIGNAL_FUNC (on_save_file_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (ok_button2), "clicked",
                      GTK_SIGNAL_FUNC (on_ok_button2_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button2), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button2_clicked),
                      NULL);

  return save_file;
}

GtkWidget*
create_wbuscar (void)
{
  GtkWidget *wbuscar;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *buscar;
  GtkWidget *hbox2;
  GtkWidget *label2;
  GtkWidget *reemprazar;
  GtkWidget *hbuttonbox1;
  guint bbuscar_key;
  GtkWidget *bbuscar;
  guint breemprazar_key;
  GtkWidget *breemprazar;
  guint bpechar_key;
  GtkWidget *bpechar;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  wbuscar = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (wbuscar), "wbuscar", wbuscar);
  gtk_window_set_title (GTK_WINDOW (wbuscar), _("Gmanedit - Search and replace"));
  gtk_window_set_position (GTK_WINDOW (wbuscar), GTK_WIN_POS_CENTER);
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (wbuscar), icon_pixbuf);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (wbuscar), vbox2);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("Search:         "));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  buscar = gtk_entry_new ();
  gtk_widget_ref (buscar);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "buscar", buscar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (buscar);
  gtk_box_pack_start (GTK_BOX (hbox1), buscar, TRUE, TRUE, 0);
  gtk_entry_set_text (GTK_ENTRY (buscar), _("Option not yet available"));

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Replace with: "));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);

  reemprazar = gtk_entry_new ();
  gtk_widget_ref (reemprazar);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "reemprazar", reemprazar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (reemprazar);
  gtk_box_pack_start (GTK_BOX (hbox2), reemprazar, TRUE, TRUE, 0);
  gtk_entry_set_text (GTK_ENTRY (reemprazar), _("Option not yet available"));

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox1, TRUE, TRUE, 0);

  bbuscar = gtk_button_new_with_label ("");
  bbuscar_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (bbuscar)->child),
                                   _("_Search"));
  gtk_widget_add_accelerator (bbuscar, "clicked", accel_group,
                              bbuscar_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  gtk_widget_ref (bbuscar);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "bbuscar", bbuscar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bbuscar);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), bbuscar);

  breemprazar = gtk_button_new_with_label ("");
  breemprazar_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (breemprazar)->child),
                                   _("_Replace"));
  gtk_widget_add_accelerator (breemprazar, "clicked", accel_group,
                              breemprazar_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  gtk_widget_ref (breemprazar);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "breemprazar", breemprazar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (breemprazar);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), breemprazar);

  bpechar = gtk_button_new_with_label ("");
  bpechar_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (bpechar)->child),
                                   _("_Close"));
  gtk_widget_add_accelerator (bpechar, "clicked", accel_group,
                              bpechar_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  gtk_widget_ref (bpechar);
  gtk_object_set_data_full (GTK_OBJECT (wbuscar), "bpechar", bpechar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bpechar);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), bpechar);

  gtk_signal_connect (GTK_OBJECT (bbuscar), "clicked",
                      GTK_SIGNAL_FUNC (on_bbuscar_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (breemprazar), "clicked",
                      GTK_SIGNAL_FUNC (on_breemprazar_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bpechar), "clicked",
                      GTK_SIGNAL_FUNC (on_bpechar_clicked),
                      NULL);

  gtk_window_add_accel_group (GTK_WINDOW (wbuscar), accel_group);

  return wbuscar;
}

GtkWidget*
create_about (void)
{
  const gchar *authors[] = {
    "Sergio Rua <srua@gpul.org>",
    "Joop Stakenborg <pg4i@amsat.org>",
    "Anibal Avelar <aavelar@cofradia.org>",
    NULL
  };
  const gchar *doc[] = {
    "Sergio Rua <srua@gpul.org>",
    NULL
  };
  const gchar *translators = "Sergio Rua <srua@gpul.org\n"
	"Jordi Mallach <jordi@sindominio.net>\n"
	"Takeshi Aihana <aihana@muc.biglobe.ne.jp>";

  GtkWidget *about;
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit.png");

  about = gnome_about_new ("Gmanedit", VERSION,
	"2000-2001 Sergio Rua\n2008 Joop Stakenborg",
	_("Man Pages Editor"), authors, doc, translators, icon_pixbuf);
  icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (about), icon_pixbuf);

  gtk_object_set_data (GTK_OBJECT (about), "about", about);

  return about;
}

GtkWidget*
create_exit_dialog (void)
{
  GtkWidget *exit_dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *bdialog_yes;
  GtkWidget *bdialog_no;

  exit_dialog = gnome_dialog_new (_("Gmanedit - Exit"), NULL);
  gtk_object_set_data (GTK_OBJECT (exit_dialog), "exit_dialog", exit_dialog);
  gtk_window_set_position (GTK_WINDOW (exit_dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_policy (GTK_WINDOW (exit_dialog), FALSE, FALSE, FALSE);
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (exit_dialog), icon_pixbuf);

  dialog_vbox1 = GNOME_DIALOG (exit_dialog)->vbox;
  gtk_object_set_data (GTK_OBJECT (exit_dialog), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  label3 = gtk_label_new (_("Exit from gmanedit?"));
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (exit_dialog), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label3, FALSE, FALSE, 0);

  dialog_action_area1 = GNOME_DIALOG (exit_dialog)->action_area;
  gtk_object_set_data (GTK_OBJECT (exit_dialog), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area1), 8);

  gnome_dialog_append_button (GNOME_DIALOG (exit_dialog), GNOME_STOCK_BUTTON_YES);
  bdialog_yes = GTK_WIDGET (g_list_last (GNOME_DIALOG (exit_dialog)->buttons)->data);
  gtk_widget_ref (bdialog_yes);
  gtk_object_set_data_full (GTK_OBJECT (exit_dialog), "bdialog_yes", bdialog_yes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bdialog_yes);

  gnome_dialog_append_button (GNOME_DIALOG (exit_dialog), GNOME_STOCK_BUTTON_NO);
  bdialog_no = GTK_WIDGET (g_list_last (GNOME_DIALOG (exit_dialog)->buttons)->data);
  gtk_widget_ref (bdialog_no);
  gtk_object_set_data_full (GTK_OBJECT (exit_dialog), "bdialog_no", bdialog_no,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bdialog_no);

  gtk_signal_connect (GTK_OBJECT (bdialog_yes), "clicked",
                      GTK_SIGNAL_FUNC (on_bdialog_yes_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bdialog_no), "clicked",
                      GTK_SIGNAL_FUNC (on_bdialog_no_clicked),
                      NULL);

  gtk_widget_grab_default (bdialog_no);
  return exit_dialog;
}

GtkWidget*
create_wpreferences (void)
{
  GtkWidget *wpreferences;
  GtkWidget *vbox3;
  GtkWidget *vbox4;
  GtkWidget *hbox3;
  GtkWidget *label4;
  GtkWidget *entry_command;
  GtkWidget *hbox4;
  GtkWidget *label17;
  GtkWidget *combo2;
  GList *combo2_items = NULL;
  GtkWidget *cbinet;
  GtkWidget *chgnome_help;
  GtkWidget *hbuttonbox2;
  GtkWidget *bpok;
  GtkWidget *bpcancel;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  wpreferences = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (wpreferences), "wpreferences", wpreferences);
  gtk_window_set_title (GTK_WINDOW (wpreferences), _("Gmanedit - Preferences"));
  gtk_window_set_position (GTK_WINDOW (wpreferences), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (wpreferences), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (wpreferences), 150, 87);
  GdkPixbuf *icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (wpreferences), icon_pixbuf);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (wpreferences), vbox3);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox4);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "vbox4", vbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox3, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("Command to test created man pages: "));
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox3), label4, FALSE, FALSE, 0);

  entry_command = gtk_entry_new ();
  gtk_widget_ref (entry_command);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "entry_command", entry_command,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry_command);
  gtk_box_pack_start (GTK_BOX (hbox3), entry_command, FALSE, TRUE, 0);
  gtk_entry_set_text (GTK_ENTRY (entry_command), _("xterm -e man "));

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);

  label17 = gtk_label_new (_("Internet browser"));
  gtk_widget_ref (label17);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "label17", label17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox4), label17, FALSE, FALSE, 0);

  combo2 = gtk_combo_new ();
  gtk_widget_ref (combo2);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "combo2", combo2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo2);
  gtk_box_pack_start (GTK_BOX (hbox4), combo2, TRUE, TRUE, 0);
  combo2_items = g_list_append (combo2_items, (gpointer) _("netscape"));
  combo2_items = g_list_append (combo2_items, (gpointer) _("mozilla"));
  combo2_items = g_list_append (combo2_items, (gpointer) _("galeon"));
  combo2_items = g_list_append (combo2_items, (gpointer) _("yelp"));
  combo2_items = g_list_append (combo2_items, (gpointer) _("konqueror"));
  combo2_items = g_list_append (combo2_items, (gpointer) _("nautilus"));
  gtk_combo_set_popdown_strings (GTK_COMBO (combo2), combo2_items);
  g_list_free (combo2_items);

  cbinet = GTK_COMBO (combo2)->entry;
  gtk_widget_ref (cbinet);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "cbinet", cbinet,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbinet);
  gtk_tooltips_set_tip (tooltips, cbinet, _("Select your internet browser from this list"), NULL);
  gtk_entry_set_text (GTK_ENTRY (cbinet), _("netscape"));

  chgnome_help = gtk_check_button_new_with_label (_("Use yelp"));
  gtk_widget_ref (chgnome_help);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "chgnome_help", chgnome_help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chgnome_help);
  gtk_box_pack_start (GTK_BOX (vbox3), chgnome_help, FALSE, FALSE, 0);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox3), hbuttonbox2, TRUE, TRUE, 0);

  bpok = gtk_button_new_from_stock (GNOME_STOCK_BUTTON_OK);
  gtk_widget_ref (bpok);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "bpok", bpok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bpok);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), bpok);

  bpcancel = gtk_button_new_from_stock (GNOME_STOCK_BUTTON_CANCEL);
  gtk_widget_ref (bpcancel);
  gtk_object_set_data_full (GTK_OBJECT (wpreferences), "bpcancel", bpcancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bpcancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), bpcancel);

  gtk_signal_connect (GTK_OBJECT (chgnome_help), "toggled",
                      GTK_SIGNAL_FUNC (on_chgnome_help_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bpok), "clicked",
                      GTK_SIGNAL_FUNC (on_bpok_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (bpcancel), "clicked",
                      GTK_SIGNAL_FUNC (on_bpcancel_clicked),
                      NULL);

  gtk_object_set_data (GTK_OBJECT (wpreferences), "tooltips", tooltips);

  return wpreferences;
}

GtkWidget*
create_wizard (void)
{

  GtkWidget *wizard;
  GtkWidget *druid_vbox2;
  GtkWidget *fixed1;
  GtkWidget *mname;
  GtkWidget *mdate;
  GtkWidget *mtitle;
  GtkWidget *mauthor;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkWidget *label16;
  GtkWidget *label1;
  GtkWidget *druid_vbox3;
  GtkWidget *fixed2;
  GtkWidget *label8;
  GtkWidget *label2;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label7;
  GtkWidget *label6;
  GtkWidget *label5;
  GtkWidget *label4;
  GtkWidget *label3;
  GtkWidget *combo1;
  GList *combo1_items = NULL;
  GtkWidget *snumber;
  GtkWidget *druid_vbox1;
  GtkWidget *fixed3;
  GtkWidget *label13;
  GtkWidget *cherrorh;
  GtkWidget *chenvironment;
  GtkWidget *chfiles;
  GtkWidget *chusage;
  GtkWidget *cherrors;
  GtkWidget *chsynopsis;
  GtkWidget *chdescription;
  GtkWidget *choptions;
  GtkWidget *chname;
  GtkWidget *chseealso;
  GtkWidget *chauthor;
  GtkWidget *chreturnvalues;
  GtkWidget *chdiagnostics;
  GtkWidget *chsecurity;
  GtkWidget *chconformingto;
  GtkWidget *chnotes;
  GtkWidget *chbugs;
  GtkTooltips *tooltips;
  GdkPixbuf *icon_pixbuf;
  time_t fech;
  struct tm *fecha;
	gchar cad[30];
	gchar *meses[]={_("January"),_("February"),_("Mars"),_("April"),
        		_("May"),_("June"),_("Jule"),_("August"),
        		_("September"),_("October"),_("November"),_("December")};

  tooltips = gtk_tooltips_new ();

  wizard = gtk_assistant_new ();
  gtk_window_set_title (GTK_WINDOW (wizard), _("Gmanedit - Wizard"));
  gtk_window_set_position (GTK_WINDOW (wizard), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (wizard), TRUE);
  gtk_window_set_policy (GTK_WINDOW (wizard), FALSE, FALSE, FALSE);
  icon_pixbuf = create_image ("gmanedit_icon.png");
  gtk_window_set_icon (GTK_WINDOW (wizard), icon_pixbuf);


  GtkWidget *labelbegin = gtk_label_new (_("Welcome to the Gmanedit Man Pages Wizard. Follow the next steps to build a Basic Man Page"));
  gtk_label_set_line_wrap (GTK_LABEL(labelbegin), TRUE);
  gtk_widget_show (labelbegin);
  gtk_assistant_append_page (GTK_ASSISTANT (wizard), labelbegin);
  gtk_assistant_set_page_title (GTK_ASSISTANT (wizard), labelbegin, _("Welcome"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (wizard), labelbegin, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (wizard), labelbegin, TRUE);

  druid_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (druid_vbox2);

  fixed1 = gtk_fixed_new ();
  gtk_widget_ref (fixed1);
  gtk_widget_show (fixed1);
  gtk_box_pack_start (GTK_BOX (druid_vbox2), fixed1, TRUE, TRUE, 0);

  mname = gtk_entry_new ();
  gtk_widget_ref (mname);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "mname", mname,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mname);
  gtk_fixed_put (GTK_FIXED (fixed1), mname, 248, 24);
  gtk_widget_set_usize (mname, 158, 26);
  gtk_tooltips_set_tip (tooltips, mname, _("Short name to the man page (example: gmanedit)"), NULL);
  

  mdate = gtk_entry_new ();
  gtk_widget_ref (mdate);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "mdate", mdate,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mdate);
  gtk_fixed_put (GTK_FIXED (fixed1), mdate, 248, 80);
  gtk_widget_set_usize (mdate, 158, 26);
  gtk_tooltips_set_tip (tooltips, mdate, _("Creation date (default today)"), NULL);
  time(&fech);
  fecha=localtime(&fech);
	
	if (fecha!=NULL)
	{
		sprintf(cad,"%s %d, %d",meses[fecha->tm_mon],fecha->tm_mday,fecha->tm_year+1900);			
		gtk_entry_set_text(GTK_ENTRY(mdate),cad);
	}


  mtitle = gtk_entry_new ();
  gtk_widget_ref (mtitle);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "mtitle", mtitle,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mtitle);
  gtk_fixed_put (GTK_FIXED (fixed1), mtitle, 248, 136);
  gtk_widget_set_usize (mtitle, 158, 26);
  gtk_tooltips_set_tip (tooltips, mtitle, _("Long man page name or title"), NULL);

  mauthor = gtk_entry_new ();
  gtk_widget_ref (mauthor);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "mauthor", mauthor,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mauthor);
  gtk_fixed_put (GTK_FIXED (fixed1), mauthor, 248, 192);
  gtk_widget_set_usize (mauthor, 158, 26);
  gtk_tooltips_set_tip (tooltips, mauthor, _("First_Name Last_Name <my@email.com>"), NULL);

  label14 = gtk_label_new (_("Creation Date:"));
  gtk_widget_ref (label14);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label14", label14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label14);
  gtk_fixed_put (GTK_FIXED (fixed1), label14, 64, 80);
  gtk_widget_set_usize (label14, 176, 24);
  gtk_label_set_line_wrap (GTK_LABEL (label14), TRUE);

  label15 = gtk_label_new (_("Man page Title:"));
  gtk_widget_ref (label15);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label15", label15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label15);
  gtk_fixed_put (GTK_FIXED (fixed1), label15, 64, 136);
  gtk_widget_set_usize (label15, 176, 24);
  gtk_label_set_line_wrap (GTK_LABEL (label15), TRUE);

  label16 = gtk_label_new (_("Author:"));
  gtk_widget_ref (label16);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label16", label16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label16);
  gtk_fixed_put (GTK_FIXED (fixed1), label16, 64, 192);
  gtk_widget_set_usize (label16, 120, 24);
  gtk_label_set_line_wrap (GTK_LABEL (label16), TRUE);


  label1 = gtk_label_new (_("Man page Name:"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_fixed_put (GTK_FIXED (fixed1), label1, 64, 24);
  gtk_widget_set_usize (label1, 184, 24);
  gtk_label_set_line_wrap (GTK_LABEL (label1), TRUE);


  gtk_assistant_append_page (GTK_ASSISTANT (wizard), druid_vbox2);
  gtk_assistant_set_page_title (GTK_ASSISTANT (wizard), druid_vbox2, _("Step 1: Basic Information"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (wizard), druid_vbox2, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (wizard), druid_vbox2, TRUE);

  druid_vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (druid_vbox3);

  fixed2 = gtk_fixed_new ();
  gtk_widget_ref (fixed2);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "fixed2", fixed2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixed2);
  gtk_box_pack_start (GTK_BOX (druid_vbox3), fixed2, TRUE, TRUE, 0);

  label8 = gtk_label_new (_("Section 5: File format and conventions"));
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_fixed_put (GTK_FIXED (fixed2), label8, 32, 136);
  gtk_widget_set_usize (label8, 224, 40);
  gtk_label_set_line_wrap (GTK_LABEL (label8), TRUE);

  label2 = gtk_label_new (_("Section number"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_fixed_put (GTK_FIXED (fixed2), label2, 56, 208);
  gtk_widget_set_usize (label2, 144, 24);

  label9 = gtk_label_new (_("Section 6: Games"));
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_fixed_put (GTK_FIXED (fixed2), label9, 264, 40);
  gtk_widget_set_usize (label9, 184, 24);
  gtk_label_set_line_wrap (GTK_LABEL (label9), TRUE);

  label10 = gtk_label_new (_("Section 7: Macro packages and Coventions"));
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_fixed_put (GTK_FIXED (fixed2), label10, 264, 64);
  gtk_widget_set_usize (label10, 240, 40);
  gtk_label_set_line_wrap (GTK_LABEL (label10), TRUE);

  label11 = gtk_label_new (_("Section 8: System management commands"));
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_fixed_put (GTK_FIXED (fixed2), label11, 264, 104);
  gtk_widget_set_usize (label11, 240, 40);
  gtk_label_set_line_wrap (GTK_LABEL (label11), TRUE);

  label12 = gtk_label_new (_("Section 9: Kernel routines"));
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_fixed_put (GTK_FIXED (fixed2), label12, 264, 152);
  gtk_widget_set_usize (label12, 240, 24);

  label7 = gtk_label_new (_("Section 4: Special files"));
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_fixed_put (GTK_FIXED (fixed2), label7, 0, 112);
  gtk_widget_set_usize (label7, 232, 24);

  label6 = gtk_label_new (_("Section 3: Library calls"));
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_fixed_put (GTK_FIXED (fixed2), label6, 0, 88);
  gtk_widget_set_usize (label6, 232, 24);

  label5 = gtk_label_new (_("Section 2: System calls"));
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_fixed_put (GTK_FIXED (fixed2), label5, 0, 64);
  gtk_widget_set_usize (label5, 240, 24);

  label4 = gtk_label_new (_("Section 1: Commands"));
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_fixed_put (GTK_FIXED (fixed2), label4, 0, 40);
  gtk_widget_set_usize (label4, 232, 24);

  label3 = gtk_label_new (_("There are 9 sections for man pages. Select one:"));
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_fixed_put (GTK_FIXED (fixed2), label3, 0, 16);
  gtk_widget_set_usize (label3, 504, 16);

  combo1 = gtk_combo_new ();
  gtk_widget_ref (combo1);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "combo1", combo1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo1);
  gtk_fixed_put (GTK_FIXED (fixed2), combo1, 240, 208);
  gtk_widget_set_usize (combo1, 175, 26);
  GTK_WIDGET_SET_FLAGS (combo1, GTK_CAN_FOCUS);
  gtk_combo_set_value_in_list (GTK_COMBO (combo1), TRUE, FALSE);
  combo1_items = g_list_append (combo1_items, (gpointer) _("1"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("2"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("3"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("4"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("5"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("6"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("7"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("8"));
  combo1_items = g_list_append (combo1_items, (gpointer) _("9"));
  gtk_combo_set_popdown_strings (GTK_COMBO (combo1), combo1_items);
  g_list_free (combo1_items);

  snumber = GTK_COMBO (combo1)->entry;
  gtk_widget_ref (snumber);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "snumber", snumber,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (snumber);
  gtk_tooltips_set_tip (tooltips, snumber, _("Select section number"), NULL);
  gtk_entry_set_text (GTK_ENTRY (snumber), _("1"));

  gtk_assistant_append_page (GTK_ASSISTANT (wizard), druid_vbox3);
  gtk_assistant_set_page_title (GTK_ASSISTANT (wizard), druid_vbox3, _("Step 2: Section number"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (wizard), druid_vbox3, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (wizard), druid_vbox3, TRUE);

  druid_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (druid_vbox1);

  fixed3 = gtk_fixed_new ();
  gtk_widget_ref (fixed3);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "fixed3", fixed3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixed3);
  gtk_box_pack_start (GTK_BOX (druid_vbox1), fixed3, TRUE, TRUE, 0);

  label13 = gtk_label_new (_("What sections would you like include?"));
  gtk_widget_ref (label13);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "label13", label13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label13);
  gtk_fixed_put (GTK_FIXED (fixed3), label13, 64, 16);
  gtk_widget_set_usize (label13, 344, 24);

  cherrorh = gtk_check_button_new_with_label (_("ERROR HANDLING"));
  gtk_widget_ref (cherrorh);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "cherrorh", cherrorh,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cherrorh);
  gtk_fixed_put (GTK_FIXED (fixed3), cherrorh, 16, 184);
  gtk_widget_set_usize (cherrorh, 176, 24);

  chenvironment = gtk_check_button_new_with_label (_("ENVIRONMENT"));
  gtk_widget_ref (chenvironment);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chenvironment", chenvironment,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chenvironment);
  gtk_fixed_put (GTK_FIXED (fixed3), chenvironment, 192, 184);
  gtk_widget_set_usize (chenvironment, 152, 24);

  chfiles = gtk_check_button_new_with_label (_("FILES"));
  gtk_widget_ref (chfiles);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chfiles", chfiles,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chfiles);
  gtk_fixed_put (GTK_FIXED (fixed3), chfiles, 192, 152);
  gtk_widget_set_usize (chfiles, 121, 28);

  chusage = gtk_check_button_new_with_label (_("USAGE"));
  gtk_widget_ref (chusage);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chusage", chusage,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chusage);
  gtk_fixed_put (GTK_FIXED (fixed3), chusage, 192, 120);
  gtk_widget_set_usize (chusage, 121, 28);

  cherrors = gtk_check_button_new_with_label (_("ERRORS"));
  gtk_widget_ref (cherrors);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "cherrors", cherrors,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cherrors);
  gtk_fixed_put (GTK_FIXED (fixed3), cherrors, 192, 56);
  gtk_widget_set_usize (cherrors, 121, 28);

  chsynopsis = gtk_check_button_new_with_label (_("SYNOPSIS"));
  gtk_widget_ref (chsynopsis);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chsynopsis", chsynopsis,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chsynopsis);
  gtk_fixed_put (GTK_FIXED (fixed3), chsynopsis, 16, 88);
  gtk_widget_set_usize (chsynopsis, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chsynopsis), TRUE);

  chdescription = gtk_check_button_new_with_label (_("DESCRIPTION"));
  gtk_widget_ref (chdescription);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chdescription", chdescription,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chdescription);
  gtk_fixed_put (GTK_FIXED (fixed3), chdescription, 16, 120);
  gtk_widget_set_usize (chdescription, 136, 24);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chdescription), TRUE);

  choptions = gtk_check_button_new_with_label (_("OPTIONS"));
  gtk_widget_ref (choptions);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "choptions", choptions,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (choptions);
  gtk_fixed_put (GTK_FIXED (fixed3), choptions, 192, 88);
  gtk_widget_set_usize (choptions, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (choptions), TRUE);

  chname = gtk_check_button_new_with_label (_("NAME"));
  gtk_widget_ref (chname);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chname", chname,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chname);
  gtk_fixed_put (GTK_FIXED (fixed3), chname, 16, 56);
  gtk_widget_set_usize (chname, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chname), TRUE);

  chseealso = gtk_check_button_new_with_label (_("SEE ALSO"));
  gtk_widget_ref (chseealso);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chseealso", chseealso,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chseealso);
  gtk_fixed_put (GTK_FIXED (fixed3), chseealso, 112, 208);
  gtk_widget_set_usize (chseealso, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chseealso), TRUE);

  chauthor = gtk_check_button_new_with_label (_("AUTHOR"));
  gtk_widget_ref (chauthor);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chauthor", chauthor,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chauthor);
  gtk_fixed_put (GTK_FIXED (fixed3), chauthor, 272, 208);
  gtk_widget_set_usize (chauthor, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chauthor), TRUE);

  chreturnvalues = gtk_check_button_new_with_label (_("RETURN VALUES"));
  gtk_widget_ref (chreturnvalues);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chreturnvalues", chreturnvalues,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chreturnvalues);
  gtk_fixed_put (GTK_FIXED (fixed3), chreturnvalues, 16, 152);
  gtk_widget_set_usize (chreturnvalues, 160, 32);

  chdiagnostics = gtk_check_button_new_with_label (_("DIAGNOSTICS"));
  gtk_widget_ref (chdiagnostics);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chdiagnostics", chdiagnostics,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chdiagnostics);
  gtk_fixed_put (GTK_FIXED (fixed3), chdiagnostics, 336, 56);
  gtk_widget_set_usize (chdiagnostics, 144, 24);

  chsecurity = gtk_check_button_new_with_label (_("SECURITY"));
  gtk_widget_ref (chsecurity);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chsecurity", chsecurity,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chsecurity);
  gtk_fixed_put (GTK_FIXED (fixed3), chsecurity, 336, 88);
  gtk_widget_set_usize (chsecurity, 121, 28);

  chconformingto = gtk_check_button_new_with_label (_("CONFORMING TO"));
  gtk_widget_ref (chconformingto);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chconformingto", chconformingto,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chconformingto);
  gtk_fixed_put (GTK_FIXED (fixed3), chconformingto, 336, 120);
  gtk_widget_set_usize (chconformingto, 168, 32);

  chnotes = gtk_check_button_new_with_label (_("NOTES"));
  gtk_widget_ref (chnotes);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chnotes", chnotes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chnotes);
  gtk_fixed_put (GTK_FIXED (fixed3), chnotes, 336, 152);
  gtk_widget_set_usize (chnotes, 121, 28);

  chbugs = gtk_check_button_new_with_label (_("BUGS"));
  gtk_widget_ref (chbugs);
  gtk_object_set_data_full (GTK_OBJECT (wizard), "chbugs", chbugs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chbugs);
  gtk_fixed_put (GTK_FIXED (fixed3), chbugs, 336, 184);
  gtk_widget_set_usize (chbugs, 121, 28);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chbugs), TRUE);

  gtk_assistant_append_page (GTK_ASSISTANT (wizard), druid_vbox1);
  gtk_assistant_set_page_title (GTK_ASSISTANT (wizard), druid_vbox1, _("Step 3: Man Sections"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (wizard), druid_vbox1, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (wizard), druid_vbox1, TRUE);


  GtkWidget *labelend = gtk_label_new (_("Thank you for using me! You can now edit the man page with gmanedit to change and complete the text"));
  gtk_label_set_line_wrap (GTK_LABEL(labelend), TRUE);
  gtk_widget_show (labelend);
  gtk_assistant_append_page (GTK_ASSISTANT (wizard), labelend);
  gtk_assistant_set_page_title (GTK_ASSISTANT (wizard), labelend, _("The End"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (wizard), labelend, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (wizard), labelend, TRUE);

  g_signal_connect (G_OBJECT (wizard), "close",
                    G_CALLBACK (on_dthe_end_finish), NULL);
  
  g_signal_connect (G_OBJECT (wizard), "cancel",
                    G_CALLBACK (on_assistant_cancel), NULL);
  return wizard;
}

