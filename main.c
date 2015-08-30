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

#include "pqevent.h"

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
    struct event_base *base = user_data;
    struct timeval delay = { 1, 0 };

    printf("Caught an interrupt signal; exiting cleanly in one sec.\n");

    event_base_loopexit(base, &delay);
}

static void process_postgresql_notify(struct pqevent *source_event, PGnotify *notify)
{
    printf("Notification '%s' from PID %d\n", notify->relname, notify->be_pid);
    if (notify->extra != NULL)
    {
        printf("Extra : >>%s<<\n", notify->extra);
    }
}

int main(int argc, char **argv) {
    struct event_base *base;
    struct event *signal_event;
    const char *conninfo;

    conninfo = "dbname = test";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Could not connect to postgresql\n");
        return -1;
    }

    base = event_base_new();
    if (!base) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

    if (!signal_event || event_add(signal_event, NULL)<0) {
        fprintf(stderr, "Could not create/add a signal event!\n");
        return 1;
    }

    struct pqevent* pqev = pqevent_new(base, conn, process_postgresql_notify);
    pqevent_listen(pqev, "my_notification");

    event_base_dispatch(base);
    event_free(signal_event);
    pqevent_free(pqev);
    PQfinish(conn);
    event_base_free(base);

    printf("done\n");

    return 0;
}


