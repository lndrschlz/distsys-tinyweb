/*
 * module name:         tinyweb.h
 * author/modifier:     Jonas Pohlken, Leander Schulz, Christian Reutebuch
 * purpose:             headerfile that defines several structs for the 'tinyweb' webserver
 *
 */
#ifndef _TINYWEB_H
#define _TINYWEB_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>

#include "http.h"

#define err_print(s)              fprintf(stderr, "ERROR: %s, %s:%d\n", (s), __FILE__, __LINE__)

#define BUFFER_SIZE                      8192
#define DEFAULT_HTML_PAGE      "default.html"


typedef struct prog_options {
    char               *progname;
    char               *root_dir;
    char               *log_filename;
    FILE               *log_fd;
    bool                verbose;
    unsigned short      timeout;
    struct addrinfo    *server_addr;
    int                 server_port;
} prog_options_t;

/*
 * struct to store http_request
 */
typedef struct http_req {
    http_method_t       method;
    char*               ressource;
    char*               range;    
} http_req_t;

/*
 * struct to store http_response
 */
typedef struct http_res {
    http_status_t               status;
    http_header_line_entry_t    headerlist[];
} http_res_t;

#endif

