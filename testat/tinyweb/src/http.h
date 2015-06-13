/*===================================================================
 * DHBW Ravensburg - Campus Friedrichshafen
 *
 * Vorlesung Verteilte Systeme
 *
 * Author:  Ralf Reutemann
 *
 *===================================================================*/
#ifndef _HTTP_H
#define _HTTP_H


typedef enum http_method {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_TEST,
    HTTP_METHOD_ECHO,
    HTTP_METHOD_NOT_IMPLEMENTED,
    HTTP_METHOD_UNKNOWN
} http_method_t;


typedef enum http_status {
    HTTP_STATUS_OK = 0,                    // 200
    HTTP_STATUS_PARTIAL_CONTENT,           // 206
    HTTP_STATUS_MOVED_PERMANENTLY,         // 301
    HTTP_STATUS_NOT_MODIFIED,              // 304
    HTTP_STATUS_BAD_REQUEST,               // 400
    HTTP_STATUS_FORBIDDEN,                 // 401
    HTTP_STATUS_NOT_FOUND,                 // 404
    HTTP_STATUS_RANGE_NOT_SATISFIABLE,     // 416
    HTTP_STATUS_INTERNAL_SERVER_ERROR,     // 500
    HTTP_STATUS_NOT_IMPLEMENTED            // 501
} http_status_t;

typedef enum http_header_line {
    HTTP_HEADER_LINE_DATE = 0,              // date
    HTTP_HEADER_LINE_SERVER,                // server
    HTTP_HEADER_LINE_LASTMODIFIED,          // last-modified
    HTTP_HEADER_LINE_CONTENTLENGHT,         // content-length
    HTTP_HEADER_LINE_CONTENTTYPE,           // content-type
    HTTP_HEADER_LINE_CONNECTION,            // connection
    HTTP_HEADER_LINE_ACCEPTRANGES,          // accept-ranges
    HTTP_HEADER_LINE_LOCATION               // location
} http_header_line_t;

typedef struct http_method_entry {
    char          *name;
    http_method_t  method;
} http_method_entry_t;


typedef struct http_status_entry {
    unsigned short   code;
    char            *text;
} http_status_entry_t;

/* typedef for header lines */
typedef struct http_header_line_entry {
    http_header_line_t      name;
    char*                   value;
} http_header_line_entry_t;


extern http_method_entry_t http_method_list[];
extern http_status_entry_t http_status_list[];
extern char*               http_header_list[];

#endif

