/*===================================================================
 * DHBW Ravensburg - Campus Friedrichshafen
 *
 * Vorlesung Verteilte Systeme
 * 
 * Gruppe:  1 - Reutebuch, Schulvbz, Polkehn
 * Author:  Polkehn
 *
 *===================================================================*/
 
 
#include <stdio.h>
#include <stdlib.h>
 
// Spezielle Socket Bibliotheken
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <passive_tcp.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <socket_io.h>

#include <tinyweb.h>
#include <client_handling.h>
#include <request_parser.h>
#include "http.h"
#include "safe_print.h"
#include "sem_print.h"

#define BUFSIZE 1000
#define WRITE_TIMEOUT 1000

#define _DEBUG
 
 // GLOBAL REQUEST COUNTER
static int request_counter = 0;
 
int accept_client(int sd, int nsd)
{
    // increase request_counter to give request a sequence number
    request_counter++;
    int req_no = request_counter;
    
    // Fork durchführen, damit ein Kindprozess den Request bearbeiten kann
	pid_t child_pid;
	if ((child_pid = fork()))
	{	
		// Vaterprozess
		// Accepting socket schließen
		int nsd_err = close(nsd);
		if ( nsd_err < 0 ) {
	           printf("Error #%d: close of socket descriptor failed.\n", nsd_err);
	           exit(nsd_err);
	       }
		
		// Info ausgeben			
		if (child_pid < 0){
			// Fehler beim Forken - Infomeldung ausgeben
			printf("[ERR #%d] Could not create child process for request. Request Denied.\n", child_pid);
		}
		else
		{	
			// Fork hat funktioniert - Infomeldung mit Prozess ausgeben
			printf("[INFO] Created child process (%d) to handle request #%d.\n", child_pid, req_no);
		}
	}
	else 
	{
		// Kindprozess
		// Zeitmessung starten
		time_t start_time = time(NULL);	
		
		// Listening socket schließen
		int sd_err = close(sd);
		if ( sd_err < 0 ) {
	           printf("Error #%d: close of socket descriptor failed.\n", sd_err);
	           exit(sd_err);
	       }
		
		int ret;
		struct rusage usage;
		time_t end_time;
		
		// Client bearbeiten
		ret = 0;//handle_client(nsd);
		
		// Ressourcenverbrauch berechnen
		end_time = time(NULL);
		getrusage(RUSAGE_SELF,&usage);
		double duration = difftime(end_time, start_time);
		double utime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1000000;
		double stime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec/1000000;
		
		
		// Prozess wird beendet, Ressourcenverbrauch ausgeben
		printf("[INFO] Child process (%d) finished handling request #%d and will terminate.\n", getpid(), req_no);
		printf("       Real Time: %0.3f System Time: %0.6fs User Time: %0.6fs \n",duration, utime, stime);
		
		exit(ret);
	}
    
	return 0;	
}

int send_response(http_res_t * response, int sd)
{
    // error code for socket-write
    int err = 0;
    
	// get http status code and text
    http_status_entry_t status = http_status_list[response->status];
    char  status_code[3];
	sprintf(status_code, "%u", status.code);
	
	// parse http status line
	char* status_line = "HTTP/1.1 ";
	strcat(status_line, status_code);
	strcat(status_line, " ");
	strcat(status_line, status.text);
	strcat(status_line, "\r\n");
	
	// write http status line to socket 
	err = write_to_socket(sd, status_line, strlen(status_line), WRITE_TIMEOUT);
	if ( err < 0 ) {
	    print_log("Error: Unable to write status_line to socket.\n");
	}
	
	// get date and write date to socket
	char* date = "Date: ";
	//strcat(date, response->date);
	strcat(date, "\r\n");
	err = write_to_socket(sd, date, strlen(date), WRITE_TIMEOUT);
	if ( err < 0 ) {
	    print_log("Error: Unable to write date to socket.\n");
	}
	
	// write server to socket
	char* server = "Server: ";
	//strcat(server, response->server);
	strcat(server, "\r\n");
	err = write_to_socket(sd, server, strlen(server), WRITE_TIMEOUT);
	if ( err < 0 ) {
	    print_log("Error: Unable to write server to socket.\n");
	}
	
	// write last_modified to socket-write
	char* lastmodified = "Last-Modified: ";
	//strcat(lastmodified, response->last_modified);
	strcat(lastmodified, "\r\n");
	err = write_to_socket(sd, lastmodified, strlen(lastmodified), WRITE_TIMEOUT);
	if ( err < 0 ) {
	    print_log("Error: Unable to write lastmodified to socket.\n");
	}
	
	// TRY WITH LIST
	// "Date" == response->headerlist[0].value
	//
	//
	// get http status code and text
    // http_status_entry_t status = http_status_list[response->status];
	
	char* headerString = http_header_list[response->headerlist[0]->name];
	print_log("Name: %s, Value: %s\n", headerString, response->headerlist[0]->value); // get the header's value
//	printf("Name: %s, Value: %s\n", headerString, response->headerlist[0]->value); // get the header's value
//	safe_printf("Name: %s, Value: %s\n", headerString, response->headerlist[0]->value); // get the header's value
	
	
	
	
	
	// if response.body != ""
	// 
	return 0;
}

int handle_client(int sd)
{	 
	http_req_t req;
	http_res_t *res = malloc(1000);                 // check up needed size later
	char req_string[BUFSIZE];
	
	int err = parse_request(&req, req_string);
	if (err < 0)
	{
		// do stuff
	}
	// request einlesen (read...)
	
	// request parsen -> request 
	//int err = parse_request(&request, request_str);
	
	//request.methode = GET
	
	http_header_line_entry_t my_date;
	my_date.name = HTTP_HEADER_LINE_DATE;
	my_date.value = "Mein Timestamp";
	
	http_header_line_entry_t my_server;
	my_server.name = HTTP_HEADER_LINE_SERVER;
	my_server.value = "polkehn.c-gurus.com";
	
	http_header_line_entry_t my_headers[] = { my_date, my_server }; // besteht aus { name, value}
	*res->headerlist = my_headers;
	
	send_response(res, sd);
	return 0;
}

