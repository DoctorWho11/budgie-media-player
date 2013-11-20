/*
 * player-view.h
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
#ifndef player_view_h
#define player_view_h

#include <glib-object.h>
#include <gtk/gtk.h>

#include "db/budgie-db.h"

typedef struct _PlayerView PlayerView;
typedef struct _PlayerViewClass   PlayerViewClass;

#define PLAYER_VIEW_TYPE (player_view_get_type())
#define PLAYER_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PLAYER_VIEW_TYPE, PlayerView))
#define IS_PLAYER_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PLAYER_VIEW_TYPE))
#define PLAYER_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), PLAYER_VIEW_TYPE, PlayerViewClass))
#define IS_PLAYER_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), PLAYER_VIEW_TYPE))
#define PLAYER_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), PLAYER_VIEW_TYPE, PlayerViewClass))

#define PLAY_CHAR "<span size='x-large'>\u25B6</span>"

/* TreeView columns */
typedef enum PlayerViewColumns {
        PLAYER_COLUMN_STATUS = 0,
        PLAYER_COLUMN_NAME,
        PLAYER_COLUMN_TIME,
        PLAYER_COLUMN_ARTIST,
        PLAYER_COLUMN_ALBUM,
        PLAYER_COLUMN_GENRE,
        PLAYER_COLUMN_INFO,
        PLAYER_COLUMNS
} PlayerViewColumns;

/* PlayerView object */
struct _PlayerView {
        GtkBin parent;

        GtkWidget *tree;
};

/* PlayerView class definition */
struct _PlayerViewClass {
        GtkBinClass parent_class;
};

GType player_view_get_type(void);

/* PlayerView methods */
GtkWidget* player_view_new(void);

/**
 * Set the current list
 * @param list Current list
 */
void player_view_set_list(PlayerView *self, GSList* list);

/**
 * Get the currently selected media
 * @return a MediaInfo representing currently selected media
 */
MediaInfo* player_view_get_current_selection(PlayerView *self);

/**
 * Set the current media to play
 * @param media A MediaInfo to play
 */
void player_view_set_current_selection(PlayerView *self, MediaInfo *info);

/**
 * Get the next media in the queue
 * @return a MediaInfo for the next item, or NULL
 */
MediaInfo* player_view_get_next_item(PlayerView *self);

/**
 * Get the previous media in the queue
 * @return a MediaInfo for the next item, or NULL
 */
MediaInfo* player_view_get_previous_item(PlayerView *self);

/**
 * Get a random track/media
 * @return a random MediaInfo
 */
MediaInfo *player_view_get_random_item(PlayerView *self);


#endif /* player_view_h */
