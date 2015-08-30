/* This file is part of libpqevent
   Copyright (C) 2015 Pierre Ducroquet <pinaraf@pinaraf.info>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __PQ_EVENT_H__
#define __PQ_EVENT_H__

#include <libpq-fe.h>

struct pqevent {
    struct event_base *base;
    struct event *event;
    PGconn *conn;
    void (*callback) (struct pqevent *, PGnotify *);
};

struct pqevent *pqevent_new(struct event_base *base, PGconn *conn, void (*callback) (struct pqevent *, PGnotify *));
void pqevent_listen(struct pqevent *pqevent, const char *listen_name);
void pqevent_free(struct pqevent *pqevent);

#endif