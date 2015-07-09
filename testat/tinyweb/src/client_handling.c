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
#include <safe_print.h>

#define BUFSIZE 100000
#define WRITE_TIMEOUT 1000

#define _DEBUG
 
 // GLOBAL REQUEST COUNTER
static int request_counter = 0;
 
int accept_client(int sd /*accepting socket */, int nsd /*listening socket */)
{	
    // increase request_counter to give request a sequence number
    request_counter++;
    int req_no = request_counter;
    
    // Fork durchführen, damit ein Kindprozess den Request bearbeiten kann
	pid_t child_pid;
	if ((child_pid = fork()))
	{	
		// Vaterprozess
		// Listening socket schließen
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
		
		// Accepting socket schließen
		int sd_err = close(sd);
		if ( sd_err < 0 ) {
	           printf("Error #%d: close of socket descriptor failed.\n", sd_err);
	           exit(sd_err);
	       }
		
		int ret;
		struct rusage usage;
		time_t end_time;
		
		// Client bearbeiten
		ret = handle_client(nsd);
		
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

	// status zeile vorbereiten
	int index = response->status;
	http_status_entry_t status = http_status_list[index];
	char status_code[50];
	sprintf(status_code, "%u", status.code);
	printf("[INFO] STATUS CODE: %s\n", status_code);
	// status zeile zusammen bauen
	char* status_line = malloc(BUFSIZE);
	strcpy(status_line, "HTTP/1.1 ");
	strcat(status_line, status_code);
	strcat(status_line, " ");
	strcat(status_line, status.text);
	strcat(status_line, "\r\n");
	
	// status zeile auf den socket schreiben
	err = write_to_socket(sd, status_line, strlen(status_line), WRITE_TIMEOUT);
	if ( err < 0 ) {
	    safe_printf("Error: Unable to write status_line to socket.\n");
	}
	
	// get server and write to socket
	if ( strcmp(response->server, "") ) {
		char* server = (char*)malloc(BUFSIZE);
 		strcpy(server, "Server: ");
		strcat(server, response->server);
 		strcat(server, "\r\n");
 		err = write_to_socket(sd, server, strlen(server), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write date to socket.\n");
 		}
	}
	
	// get date and write date to socket
	if ( strcmp(response->date, "") ) {
		char* date = (char*)malloc(BUFSIZE);
 		strcpy(date, "Date: ");
		strcat(date, response->date);
 		strcat(date, "\r\n");
 		err = write_to_socket(sd, date, strlen(date), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 		    safe_printf("Error: Unable to write date to socket.\n");
 		}
	}
 	
    // get last_modified and write to socket
    if ( strcmp(response->date, "") ) {
		char* lm = (char*)malloc(BUFSIZE);
 		strcpy(lm, "Last-Modified: ");
		strcat(lm, response->last_modified);
 		strcat(lm, "\r\n");
 		err = write_to_socket(sd, lm, strlen(lm), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write date to socket.\n");
 		}
    }
	
	// get content_length and write to socket
	if ( strcmp(response->content_length, "") ) {
		char* content_length = (char*)malloc(BUFSIZE);
 		strcpy(content_length, "Content-Length: ");
		strcat(content_length, response->content_length);
 		strcat(content_length, "\r\n");
 		err = write_to_socket(sd, content_length, strlen(content_length), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write content_length to socket.\n");
 		}
	}
 	
 	// get content_type and write to socket
 	if ( strcmp(response->content_type, "") ) {
		char* content_type = (char*)malloc(BUFSIZE);
 		strcpy(content_type, "Content-Type: ");
		strcat(content_type, response->content_type);
 		strcat(content_type, "\r\n");
 		err = write_to_socket(sd, content_type, strlen(content_type), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write content_type to socket.\n");
 		}
 	}
	
	// get connection and write to socket
	if ( strcmp(response->connection, "") ) {
		char* connection = (char*)malloc(BUFSIZE);
 		strcpy(connection, "Connection: ");
		strcat(connection, response->connection);
 		strcat(connection, "\r\n");
 		err = write_to_socket(sd, connection, strlen(connection), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to connection date to socket.\n");
 		}
	}
 	
 	// get accept_ranges and write to socket
 	if ( strcmp(response->accept_ranges, "") ) {
		char* accept_ranges = (char*)malloc(BUFSIZE);
 		strcpy(accept_ranges, "Accept-Ranges: ");
		strcat(accept_ranges, response->accept_ranges);
 		strcat(accept_ranges, "\r\n");
 		err = write_to_socket(sd, accept_ranges, strlen(accept_ranges), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write accept_ranges to socket.\n");
 		}
 	}
 	
 	// get location and write to socket
 	if ( strcmp(response->location, "") ) {
		char* location = (char*)malloc(BUFSIZE);
 		strcpy(location, "Location: ");
		strcat(location, response->location);
 		strcat(location, "\r\n");
 		err = write_to_socket(sd, location, strlen(location), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write location to socket.\n");
 		}
 	}
 	
 	// append new line
 	char* newline = (char*)malloc(BUFSIZE);
 	strcpy(newline, "\r\n");
 	err = write_to_socket(sd, newline, strlen(newline), WRITE_TIMEOUT);
 	if ( err < 0 ) {
 	    safe_printf("Error: Unable to write new line to socket.\n");
 	}
 	
	// get body and write to socket
	if ( strcmp(response->body, "") ) {
		char* body = (char*)malloc(BUFSIZE);
 		strcpy(body, response->body);
 		strcat(body, "\r\n\0");
 		err = write_to_socket(sd, body, strlen(body), WRITE_TIMEOUT);
 		if ( err < 0 ) {
 	    	safe_printf("Error: Unable to write body to socket.\n");
 		}
	}
 	
	return 0;
}

int handle_client(int sd)
{	 
	http_req_t req;
	http_res_t res;
	char * req_string = malloc(BUFSIZE);
	
	//char * req_string = "GET /test/resource/test.jpg HTTP/1.1\r\nRange:Test\r\nContent-Length:0\r\n\r\n";
	
	read_from_socket (sd, req_string, BUFSIZE, 1);
	int err = parse_request(&req, req_string);
	if (err < 0)
	{
		//exit(-1);
	}
	else
	{
		safe_printf("Method: %s\nResource: %s\nRange: %s\n", http_method_list[req.method].name, req.resource, req.range);
	}
	
	
	// request einlesen (read...)
	
	// request parsen -> request 
	//int err = parse_request(&request, request_str);

	//request.methode = GET
	res.status = HTTP_STATUS_OK;
	

	res.date = "Day, 01 Jan 2000 12:00:00 GMT";
	res.server = "C-Server DistSys";
	res.last_modified = "1";
	res.content_length = "2";
    res.content_type = "";
    res.connection = "4";
    res.accept_ranges = "";
    res.location = "6";
    res.body = "<html><body>Hello world</body></html>";
	err = send_response(&res, sd);
	if ( err < 0 ) {
		safe_printf("Failed to send the response: %d\n", err);
	}
	return 0;
}

