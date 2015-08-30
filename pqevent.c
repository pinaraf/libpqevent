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


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <libpq-fe.h>
#include <string.h>

#include "pqevent.h"

static void
pq_cb(evutil_socket_t sig, short events, void *user_data)
{
    PGnotify   *notify;
    struct pqevent *cb_data = (struct pqevent *) user_data;
    
    event_free(cb_data->event);
    
    PQconsumeInput(cb_data->conn);
    
    while ((notify = PQnotifies(cb_data->conn)) != NULL)
    {
        if (cb_data->callback != NULL)
            cb_data->callback(cb_data, notify);
        PQfreemem(notify);
    }

    cb_data->event = event_new(cb_data->base, PQsocket(cb_data->conn), EV_READ, pq_cb, cb_data);
    event_add(cb_data->event, NULL);
}

struct pqevent *pqevent_new(struct event_base *base, PGconn *conn, void (*callback) (struct pqevent *, PGnotify *))
{
    struct pqevent *result = (struct pqevent *) malloc(sizeof(struct pqevent));
    int         sock;
    sock = PQsocket(conn);
    result->base = base;
    result->conn = conn; 
    result->event = event_new(base, sock, EV_READ, pq_cb, result);
    result->callback = callback;
    event_add(result->event, NULL);
    return result;
}

void pqevent_listen(struct pqevent *pqevent, const char *listen_name)
{
    char *query = (char *)malloc(sizeof(char) * strlen(listen_name) + 12);
    sprintf(query, "LISTEN \"%s\"", listen_name);
    fprintf(stderr, "%s\n", query);
    PGresult *res = PQexec(pqevent->conn, query);
    free(query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        // Todo : return errors !
        fprintf(stderr, "LISTEN command failed: %s", PQerrorMessage(pqevent->conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    // Return everything is ok
}

void pqevent_free(struct pqevent *pqevent)
{
    // More to do ?
    event_free(pqevent->event);
    free(pqevent);
}
