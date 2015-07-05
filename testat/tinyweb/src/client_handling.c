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

#define BUFSIZE 100000
#define WRITE_TIMEOUT 10


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
	           safe_printf("Error #%d: close of socket descriptor failed.\n", nsd_err);
	           exit(nsd_err);
	       }
		
		// Info ausgeben			
		if (child_pid < 0){
			// Fehler beim Forken - Infomeldung ausgeben
			safe_printf("[ERR #%d] Could not create child process for request. Request Denied.\n", child_pid);
		}
		else
		{	
			// Fork hat funktioniert - Infomeldung mit Prozess ausgeben
			safe_printf("[INFO] Created child process (%d) to handle request #%d.\n", child_pid, req_no);
		}
	}
	else 
	{	
		safe_printf("%s\n", __LINE__);
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
    safe_printf("%s\n", __LINE__);
	// get http status code and text
	int index = response->status;
	safe_printf("%s\n", __LINE__);
    http_status_entry_t status = http_status_list[index];
    safe_printf("%s\n", __LINE__);
    char  status_code[50];
    safe_printf("%s\n", __LINE__);
	sprintf(status_code, "%u", status.code);
	safe_printf("%s\n", __LINE__);
	
	printf("[INFO] STATUS CODE: %s\n", status_code);
	
	// parse http status line
	char* status_line = malloc(BUFSIZE);
	safe_printf("%s\n", __LINE__);
	strcpy(status_line, "HTTP/1.1 ");
	safe_printf("%s\n", __LINE__);
	//char* status_line = "HTTP/1.1 ";
	strcat(status_line, status_code);
	safe_printf("%s\n", __LINE__);
	strcat(status_line, " ");
	safe_printf("%s\n", __LINE__);
	strcat(status_line, status.text);
	safe_printf("%s\n", __LINE__);
	strcat(status_line, "\r\n");
	safe_printf("%s\n", __LINE__);
	
	
	
	// write http status line to socket 
	err = write_to_socket(sd, status_line, strlen(status_line), WRITE_TIMEOUT);
	safe_printf("%s\n", __LINE__);
	if ( err < 0 ) {
	    print_log("Error: Unable to write status_line to socket.\n");
	}

	
	
	// get number of filled header fields
/*	int size = sizeof(response->headerlist[0]);
	safe_printf("Response_size: %d\n", size);
	// parse http header line by line
	for(int i=0; i<2; i++) // TODO: find a better way than i<3; 
	{
		safe_printf("%d\n", __LINE__);
	    char line[BUFSIZE];
	    safe_printf("%d\n", sizeof(response->headerlist[i]));
	    http_header_line_entry_t *nextHeader = response->headerlist[i];
	    safe_printf("%s\n", nextHeader->value);
	    char* headerString = http_header_list[response->headerlist[i]->name];
	    safe_printf("%d\n", __LINE__);
	    char* headerValue  = response->headerlist[i]->value;
	    safe_printf("%d\n", __LINE__);
	    strcat(line, headerString);
	    strcat(line, ": ");
	    strcat(line, headerValue);
	    strcat(line, "\r\n");
	    printf("[INFO] HeaderString: %s, headerValue: %s\n", headerString, headerValue);
	    safe_printf("%d\n", __LINE__);
	    err = write_to_socket(sd, line, strlen(line), WRITE_TIMEOUT);
	    if ( err < 0 ) {
	        print_log("Error: Unable to write %s to socket.\n", headerString);
	    }
	    safe_printf("%d\n", __LINE__);
	} 
	*/
	
	// if response.body != ""
	// 
	return 0;
}

int handle_client(int sd)
{	 
	http_req_t req;
	safe_printf("%s\n", __LINE__);
	http_res_t *res = malloc(1000);  
	safe_printf("%s\n", __LINE__);// check up needed size later
	char * req_string = malloc(BUFSIZE);
	safe_printf("%s\n", __LINE__);
	
	read_from_socket (sd, req_string, BUFSIZE, WRITE_TIMEOUT);
	safe_printf("%s\n", __LINE__);
	int err = parse_request(&req, req_string);
	safe_printf("%s\n", __LINE__);
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
/*	http_header_line_entry_t my_date;
	my_date.name = HTTP_HEADER_LINE_DATE;
	my_date.value = "Mein Timestamp";
	
	http_header_line_entry_t my_server;
	my_server.name = HTTP_HEADER_LINE_SERVER;
	my_server.value = "polkehn.c-gurus.com";
*/	
	//http_header_line_entry_t my_headers[3] = { my_date, my_server, my_mod }; // besteht aus { name, value}
	//*res->headerlist = my_headers;
	safe_printf("%s\n", __LINE__);
	res->status = HTTP_STATUS_OK;
	safe_printf("%s\n", __LINE__);
	struct tm *ts;
	safe_printf("%s\n", __LINE__);
	ts = localtime((time_t*) time(NULL));
	safe_printf("%s\n", __LINE__);
	strftime(res->date, BUFSIZE, "%a, %d %b %Y %T %z", ts);
safe_printf("%s\n", __LINE__);
	res->server = "C-Server DistSys";
	safe_printf("%s\n", __LINE__);
	int sr_err = send_response(res, sd);
	if ( sr_err < 0 ) {
		safe_printf("[ERROR] Failed to send response header! (send_response = %d)\n", sr_err);
	}
	
	return 0;
}

