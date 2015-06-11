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

int send_response(http_res_t * response,int sd)
{
	// status zeile schreiben
	// write(...)
	
	// header
	// if header != ""
	// write(...)
	
	// if response.body != ""
	// 
	return 0;
}

int handle_client(int sd)
{	 
	http_req_t req;
	http_res_t res;
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
	
	
	send_response(&res, sd);
	return 0;
}

