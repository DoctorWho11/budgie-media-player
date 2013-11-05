/*
 * media-db.c
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "media-db.h"

G_DEFINE_TYPE_WITH_PRIVATE(MediaDB, media_db, G_TYPE_OBJECT);

/* Private utilities */
static gboolean media_db_serialize(MediaInfo *info, uint8_t **target);
static gboolean media_db_deserialize(uint8_t* source, MediaInfo **target);

/* MediaInfo API */
void free_media_info(gpointer p_info)
{
        MediaInfo *info;

        info = (MediaInfo*)p_info;
        if (info->title)
                g_free(info->title);
        if (info->artist)
                g_free(info->artist);
        if (info->album)
                g_free(info->album);
        if (info->genre)
                g_free(info->genre);
        if (info->path)
                g_free(info->path);
        if (info->mime)
                g_free(info->mime);
}

/* Initialisation */
static void media_db_class_init(MediaDBClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &media_db_dispose;
}

static void media_db_init(MediaDB *self)
{
        const gchar *config;
        self->priv = media_db_get_instance_private(self);

        /* Our storage location */
        config = g_get_user_config_dir();
        self->priv->storage_path = g_strdup_printf("%s/%s", config,
                CONFIG_NAME);

        /* Open the database */
        self->priv->db = gdbm_open(self->priv->storage_path, 0,
                GDBM_WRCREAT, 0600, NULL);
        if (!self->priv->db)
                g_error("Failed to initialise database!");
}

static void media_db_dispose(GObject *object)
{
        MediaDB *self;

        self = MEDIA_DB(object);
        if (self->priv->storage_path)
                g_free(self->priv->storage_path);

        gdbm_close(self->priv->db);
        /* Destruct */
        G_OBJECT_CLASS(media_db_parent_class)->dispose(object);
}

/* Utility; return a new MediaDB */
MediaDB* media_db_new(void)
{
        MediaDB *self;

        self = g_object_new(MEDIA_DB_TYPE, NULL);
        return MEDIA_DB(self);
}

void media_db_store_media(MediaDB *self, MediaInfo *info)
{
        datum value;
        uint8_t *store = NULL;

        /* Path is the unique key */
        datum key = { (char*)info->path, strlen(info->path)+1 };

        if (!media_db_serialize(info, &store)) {
                g_warning("Unable to serialize data!");
                goto end;
        }
        value.dptr = (char*)store;
        value.dsize = malloc_usable_size(store);;

        gdbm_store(self->priv->db, key, value, GDBM_REPLACE);
end:
        if (store)
                free(store);
}

MediaInfo* media_db_get_media(MediaDB *self, gchar *path)
{
        datum value;
        MediaInfo *ret = NULL;
        uint8_t* store = NULL;
        datum key = { (char*)path, strlen(path)+1 };

        memset(&value, 0, sizeof(datum));
        value = gdbm_fetch(self->priv->db, key);
        if (value.dsize < 0 || value.dptr == NULL)
                goto end;
        store = (uint8_t*)value.dptr;

        if (!media_db_deserialize(store, &ret)) {
                g_message("Unable to deserialize");
                goto end;
        }
        ret->path = g_strdup(path);
end:
        if (store)
                free(store);
        return ret;
}

GSList* media_db_get_all_media(MediaDB* self)
{
        datum key, nextkey;
        GSList* ret = NULL;
        char *path;
        MediaInfo *cur = NULL;

        key = gdbm_firstkey(self->priv->db);
        while (key.dptr) {
                path = (char*)key.dptr;
                cur = media_db_get_media(self, path);
                ret = g_slist_append(ret, cur);
                nextkey = gdbm_nextkey(self->priv->db, key);
                free(key.dptr);
                key = nextkey;
        }
        return ret;
}
/** PRIVATE **/
static gboolean media_db_serialize(MediaInfo *info, uint8_t **target)
{
        uint8_t* data = NULL;
        gboolean ret = FALSE;
        unsigned int length;
        unsigned int size;
        unsigned int offset = 0;

        /* 5 member fields */
        if (info->title)
                size = strlen(info->title)+1;
        if (info->artist)
                size += strlen(info->artist)+1;
        if (info->album)
                size += strlen(info->album)+1;
        if (info->genre)
                size += strlen(info->genre)+1;
        size += strlen(info->mime)+1;

        /* 5 size fields */
        size += sizeof(unsigned int)*5;

        data = malloc(size);
        if (!data)
                goto end;

        /* Title */
        if (info->title)
                length = strlen(info->title)+1;
        else
                length = 0;
        memcpy(data, &length, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (info->title)
                memcpy(data+offset, info->title, length);
        offset += length;

        /* Artist */
        if (info->artist)
                length = strlen(info->artist)+1;
        else
                length = 0;
        memcpy(data+offset, &length, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (info->artist)
                memcpy(data+offset, info->artist, length);
        offset += length;

        /* Album */
        if (info->album)
                length = strlen(info->album)+1;
        else
                length = 0;
        memcpy(data+offset, &length, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (info->album)
                memcpy(data+offset, info->album, length);
        offset += length;

        /* Genre */
        if (info->genre)
                length = strlen(info->genre)+1;
        else
                length = 0;
        memcpy(data+offset, &length, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (info->genre)
                memcpy(data+offset, info->genre, length);
        offset += length;

        /* Mime */
        length = strlen(info->mime)+1;
        memcpy(data+offset, &length, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        memcpy(data+offset, info->mime, length);


        ret = TRUE;
        *target = data;
end:
        if (!ret && data)
                free(data);
        return ret;
}

static gboolean media_db_deserialize(uint8_t* source, MediaInfo **target)
{
        MediaInfo *ret = NULL;
        gchar *title = NULL;
        gchar *artist = NULL;
        gchar *album = NULL;
        gchar *genre = NULL;
        gchar *mime = NULL;
        unsigned int title_len, artist_len;
        unsigned int album_len, genre_len;
        unsigned int mime_len;
        unsigned int offset = 0;
        gboolean op = FALSE;

        ret = malloc(sizeof(MediaInfo));
        if (!ret)
                goto end;

        memset(ret, 0, sizeof(MediaInfo));
        /* Title */
        memcpy(&title_len, source, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (title_len > 0) {
                title = malloc(title_len);
                memcpy(title, source+offset, title_len);
        }
        offset += title_len;

        /* Artist */
        memcpy(&artist_len, source+offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (artist_len > 0) {
                artist = malloc(artist_len);
                memcpy(artist, source+offset, artist_len);
        }
        offset += artist_len;

        /* Album */
        memcpy(&album_len, source+offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (album_len > 0) {
                album = malloc(album_len);
                memcpy(album, source+offset, album_len);
        }
        offset += album_len;

        /* Genre */
        memcpy(&genre_len, source+offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        if (genre_len > 0) {
                genre = malloc(genre_len);
                memcpy(genre, source+offset, genre_len);
        }
        offset += genre_len;

        /* Mime */
        memcpy(&mime_len, source+offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        mime = malloc(mime_len);
        memcpy(mime, source+offset, mime_len);

        offset += mime_len; /* Reserved */
        /* Copy the data instead of exposing internals */
        if (title)
                ret->title = g_strdup(title);
        if (artist)
                ret->artist = g_strdup(artist);
        if (album)
                ret->album = g_strdup(album);
        if (genre)
                ret->genre = g_strdup(genre);

        ret->mime = g_strdup(mime);
        op = TRUE;
        *target = ret;

end:
        if (title)
                free(title);
        if (artist)
                free(artist);
        if (album)
                free(album);
        if (genre)
                free(genre);
        if (mime)
                free(mime);
        return op;
}