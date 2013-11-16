/*
 * budgie-settings-view.h
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef budgie_settings_view_h
#define budgie_settings_view_h

#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct _BudgieSettingsView BudgieSettingsView;
typedef struct _BudgieSettingsViewClass   BudgieSettingsViewClass;

#define BUDGIE_SETTINGS_VIEW_TYPE (budgie_settings_view_get_type())
#define BUDGIE_SETTINGS_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUDGIE_SETTINGS_VIEW_TYPE, BudgieSettingsView))
#define IS_BUDGIE_SETTINGS_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUDGIE_SETTINGS_VIEW_TYPE))
#define BUDGIE_SETTINGS_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), BUDGIE_SETTINGS_VIEW_TYPE, BudgieSettingsViewClass))
#define IS_BUDGIE_SETTINGS_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), BUDGIE_SETTINGS_VIEW_TYPE))
#define BUDGIE_SETTINGS_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), BUDGIE_SETTINGS_VIEW_TYPE, BudgieSettingsViewClass))

/* BudgieSettingsView object */
struct _BudgieSettingsView {
        GtkBox parent;
        GtkWidget *paths;
        GSettings *settings;

        GtkWidget *path_remove;
        GtkWidget *path_add;
};

/* BudgieSettingsView class definition */
struct _BudgieSettingsViewClass {
        GtkBoxClass parent_class;
};


/* Boilerplate GObject code */
static void budgie_settings_view_class_init(BudgieSettingsViewClass *klass);
static void budgie_settings_view_init(BudgieSettingsView *self);
static void budgie_settings_view_dispose(GObject *object);
GType budgie_settings_view_get_type(void);

/* BudgieSettingsView methods */
GtkWidget* budgie_settings_view_new(void);

/* Refresh view from settings */
static void budgie_settings_refresh(BudgieSettingsView *self);

#endif /* budgie_settings_view_h */
