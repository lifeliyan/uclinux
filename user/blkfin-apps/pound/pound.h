/*
 * Pound - the reverse-proxy load-balancer
 * Copyright (C) 2002 Apsis GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * Contact information:
 * Apsis GmbH
 * P.O.Box
 * 8707 Uetikon am See
 * Switzerland
 * Tel: +41-1-920 4904
 * EMail: roseg@apsis.ch
 */

/*
 * $Id$
 * $Log$
 * Revision 1.1  2006/02/28 06:02:02  grace
 * Initial revision
 *
 * Revision 2.0  2006/02/01 11:45:32  roseg
 * Enhancements:
 *   - new configuration file syntax, offering significant improvements.
 *   - the ability to define listener-specific back-ends. In most cases this
 *     should eliminate the need for multiple Pound instances.
 *   - a new type of back-end: the redirector allows you to respond with a
 *     redirect without involving any back-end server.
 *   - most "secondary" properties (such as error messages, client time-out,
 *     etc.) are now private to listeners.
 *   - HAport has an optional address, different from the main back-end
 *   - added a -V flag for version
 *   - session keeping on a specific Header
 *
 * Revision 1.10  2006/02/01 11:19:54  roseg
 * Enhancements:
 *   added NoDaemon configuration directive (replaces compile-time switch)
 *   added LogFacility configuration directive (replaces compile-time switch)
 *   added user name logging
 *
 * Bug fixes:
 *   fixed problem with the poll() code
 *   fixed problem with empty list in gethostbyname()
 *   added call to setsid() if daemon
 *   conflicting headers are removed (Content-length - Transfer-encoding)
 *
 * Last release in the 1.x series.
 *
 *
 */

#include    "config.h"
#include    <stdio.h>

#if HAVE_STDLIB_H
#include    <stdlib.h>
#else
#error "Pound needs stdlib.h"
#endif

#if HAVE_UNISTD_H
#include    <unistd.h>
#else
#error "Pound needs unistd.h"
#endif

#if HAVE_GETOPT_H
#include    <getopt.h>
#endif

#if HAVE_PTHREAD_H
#include    <pthread.h>
#else
#error "Pound needs pthread.h"
#endif

#if HAVE_STRING_H
#include    <string.h>
#else
#error "Pound needs string.h"
#endif

#if TIME_WITH_SYS_TIME
#if HAVE_SYS_TIME_H
#include    <sys/time.h>
#else
#error "Pound needs sys/time.h"
#endif
#if HAVE_TIME_H
#include    <time.h>
#else
#error "Pound needs time.h"
#endif
#else   /* may not mix sys/time.h and time.h */
#if HAVE_SYS_TIME_H
#include    <sys/time.h>
#elif   HAVE_TIME_H
#include    <time.h>
#else
#error "Pound needs time.h"
#endif
#endif  /* mix */

#if HAVE_SYS_TYPES_H
#include    <sys/types.h>
#else
#error "Pound needs sys/types.h"
#endif

#if HAVE_SYS_SOCKET_H
#include    <sys/socket.h>
#else
#error "Pound needs sys/socket.h"
#endif

#if HAVE_NETINET_IN_H
#include    <netinet/in.h>
#else
#error "Pound needs netinet/in.h"
#endif

#if HAVE_NETINET_TCP_H
#include    <netinet/tcp.h>
#else
#error "Pound needs netinet/tcp.h"
#endif

#if HAVE_ARPA_INET_H
#include    <arpa/inet.h>
#else
#error "Pound needs arpa/inet.h"
#endif

#if HAVE_NETDB_H
#include    <netdb.h>
#else
#error "Pound needs netdb.h"
#endif

#if HAVE_SYS_POLL_H
#include    <sys/poll.h>
#else
#error "Pound needs sys/poll.h"
#endif

#if HAVE_OPENSSL_SSL_H
#define OPENSSL_THREAD_DEFINES
#include    <openssl/ssl.h>
#if OPENSSL_VERSION_NUMBER >= 0x00907000L
#ifndef OPENSSL_THREADS
#error  "Pound requires OpenSSL with thread support"
#endif
#else
#ifndef THREADS
#error  "Pound requires OpenSSL with thread support"
#endif
#endif
#else
#error "Pound needs openssl/ssl.h"
#endif

#if HAVE_OPENSSL_ENGINE_H
#include    <openssl/engine.h>
#endif

#if HAVE_PWD_H
#include    <pwd.h>
#else
#error "Pound needs pwd.h"
#endif

#if HAVE_GRP_H
#include    <grp.h>
#else
#error "Pound needs grp.h"
#endif

#if HAVE_SYSLOG_H
#include    <syslog.h>
#endif

#if HAVE_SYS_SYSLOG_H
#include    <sys/syslog.h>
#endif

#if HAVE_SIGNAL_H
#include    <signal.h>
#else
#error "Pound needs signal.h"
#endif

#if HAVE_REGEX_H
#include    <regex.h>
#else
#error "Pound needs regex.h"
#endif

#if HAVE_CTYPE_H
#include    <ctype.h>
#else
#error "Pound needs ctype.h"
#endif

#if HAVE_ERRNO_H
#include    <errno.h>
#else
#error "Pound needs errno.h"
#endif

#if HAVE_WAIT_H
#include    <wait.h>
#elif   HAVE_SYS_WAIT_H
#include    <sys/wait.h>
#else
#error "Pound needs sys/wait.h"
#endif

#if HAVE_SYS_STAT_H
#include    <sys/stat.h>
#else
#error "Pound needs sys/stat.h"
#endif

#if HAVE_FCNTL_H
#include    <fcntl.h>
#else
#error "Pound needs fcntl.h"
#endif

#if HAVE_STDARG_H
#include    <stdarg.h>
#else
#include    <varargs.h>
#endif

/*
 * Global variables needed by everybody
 */

extern char *user,              /* user to run as */
            *group,             /* group to run as */
            *root_jail,         /* directory to chroot to */
            *pid_name;          /* file to record pid in */

extern int  alive_to,           /* check interval for resurrection */
            daemonize,          /* run as daemon */
            log_facility,       /* log facility to use */
            log_level,          /* logging mode - 0, 1, 2 */
            print_log;          /* print log messages to stdout/stderr */

extern regex_t  HTTP,       /* normal HTTP requests: GET, POST, HEAD */
                XHTTP,      /* extended HTTP requests: PUT, DELETE */
                WEBDAV,     /* WebDAV requests: LOCK, UNLOCK, SUBSCRIBE, PROPFIND, PROPPATCH, BPROPPATCH, SEARCH,
                               POLL, MKCOL, MOVE, BMOVE, COPY, BCOPY, DELETE, BDELETE, CONNECT, OPTIONS, TRACE */
                HEADER,     /* Allowed header */
                CHUNK_HEAD, /* chunk header line */
                RESP_SKIP,  /* responses for which we skip response */
                RESP_IGN,   /* responses for which we ignore content */
                RESP_REDIR, /* responses for which we rewrite Location */
                LOCATION,   /* the host we are redirected to */
                AUTHORIZATION;  /* the Authorisation header */

#define MAXBUF      2048
#define MAXHEADERS  128

#ifndef F_CONF
#define F_CONF  "/usr/local/etc/pound.cfg"
#endif

#ifndef F_PID
#define F_PID  "/var/run/pound.pid"
#endif

/* matcher chain */
typedef struct _matcher {
    regex_t             pat;        /* pattern to match the request/header against */
    struct _matcher     *next;
}   MATCHER;

/* back-end types */
typedef enum    { BACK_END, REDIRECTOR }    BE_TYPE;
typedef enum    { S_NONE, S_IP, S_COOKIE, S_PARM, S_HEADER, S_BASIC }   SESS_TYPE;

/* back-end definition */
typedef struct _backend {
    BE_TYPE             be_type;
    struct sockaddr_in  addr;       /* address */
    int                 priority;   /* priority */
    int                 to;
    struct sockaddr_in  HA;         /* HAport */
    char                *url;       /* for redirectors */
    int                 alive;
    struct _backend     *next;
}   BACKEND;

/* session key max size */
#define KEY_SIZE    63

/* Session definition */
typedef struct _sess {
    char                key[KEY_SIZE + 1];  /* session key */
    BACKEND             *to_host;           /* backend pointer */
    time_t              last_acc;           /* time of last access */
    int                 children;           /* number of children */
    struct _sess        *left, *right;
}   SESS;

#define n_children(S)   ((S)? (S)->children: 0)

/* service definition */
typedef struct _service {
    MATCHER             *url,       /* request matcher */
                        *req_head,  /* required headers */
                        *deny_head; /* forbidden headers */
    BACKEND             *backends;
    int                 tot_pri;    /* total priority for all back-ends */
    pthread_mutex_t     mut;        /* mutex for this service */
    SESS_TYPE           sess_type;
    int                 sess_ttl;   /* session time-to-live */
    regex_t             sess_pat;   /* pattern to match the session data */
    char                *sess_parm; /* session cookie or parameter */
    SESS                *sessions;  /* currently active sessions */
    struct _service     *next;
}   SERVICE;

extern SERVICE          *services;  /* global services (if any) */

/* Listener definition */
typedef struct _listener {
    struct sockaddr_in  addr;       /* address */
    int                 sock;       /* listening socket */
    SSL_CTX             *ctx;       /* CTX for SSL connections */
    int                 clnt_check; /* client verification mode */
    int                 noHTTPS11;  /* HTTP 1.1 mode for SSL */
    char                *ssl_head;  /* extra SSL header */
    int                 xHTTP;      /* allow extended HTTP */
    int                 webDAV;     /* allow DAV */
    int                 to;         /* client time-out */
    regex_t             url_pat;    /* pattern to match the request against */
    char                *err414,    /* error messages */
                        *err500,
                        *err501,
                        *err503;
    long                max_req;    /* max. request size */
    MATCHER             *head_off;  /* headers to remove */
    int                 change30x;  /* rewrite redirect response */
    SERVICE             *services;
    struct _listener    *next;
}   LISTENER;

extern LISTENER         *listeners; /* all available listeners */

typedef struct  {
    int                 sock;
    LISTENER            *lstn;
    struct in_addr      from_host;
}   thr_arg;                        /* argument to processing threads: socket, origin */

/* Header types */
#define HEADER_ILLEGAL              -1
#define HEADER_OTHER                0
#define HEADER_TRANSFER_ENCODING    1
#define HEADER_CONTENT_LENGTH       2
#define HEADER_CONNECTION           3
#define HEADER_LOCATION             4
#define HEADER_HOST                 5
#define HEADER_REFERER              6
#define HEADER_USER_AGENT           7

#ifdef  NEED_INADDRT
/* for oldish Unices - normally this is in /usr/include/netinet/in.h */
typedef u_int32_t   in_addr_t;
#endif

#ifdef  NEED_INPORTT
/* for oldish Unices - normally this is in /usr/include/netinet/in.h */
typedef u_int16_t   in_port_t;
#endif

/*
 * handle an HTTP request
 */
extern void *thr_http(void *);

/*
 * Log an error to the syslog or to stderr
 */
extern void logmsg(int priority, char *fmt, ...);

/*
 * Find the right service for a request
 */
extern SERVICE  *get_service(LISTENER *, char *, char **);

/*
 * Find the right back-end for a request
 */
extern BACKEND  *get_backend(SERVICE *, struct in_addr, char *, char **);

/*
 * Find if a redirect needs rewriting
 * In general we have two possibilities that require it:
 * (1) if the redirect was done to the correct location with the wrong protocol
 * (2) if the redirect was done to the back-end rather than the listener
 */
extern int  need_rewrite(char *, char *, LISTENER *, BACKEND *);
/*
 * (for cookies only) possibly create session based on response headers
 */
extern void upd_session(SERVICE *, char **, BACKEND *);

/*
 * Parse a header
 */
extern int  check_header(char *, char *);

/*
 * mark a backend host as dead;
 * do nothing if no resurection code is active
 */
extern void kill_be(SERVICE *, BACKEND *);

/*
 * Non-blocking version of connect(2). Does the same as connect(2) but
 * ensures it will time-out after a much shorter time period CONN_TO.
 */
extern int  connect_nb(int, struct sockaddr *, socklen_t, int);

/*
 * Check if dead hosts returned to life;
 * runs every alive_to seconds
 */
extern void *thr_resurect(void *);

/*
 * Parse arguments/config file
 */
extern void config_parse(int, char **);

/*
 * RSA ephemeral keys: how many and how often
 */
#define N_RSA_KEYS  3
#ifndef T_RSA_KEYS
#define T_RSA_KEYS  300
#endif

/*
 * return a pre-generated RSA key
 */
extern RSA  *RSA_tmp_callback(SSL *, int, int);

/*
 * Pre-generate ephemeral RSA keys
 */
extern int  init_RSAgen(void);

/*
 * Periodically regenerate ephemeral RSA keys
 * runs every T_RSA_KEYS seconds
 */
extern void *thr_RSAgen(void *);
