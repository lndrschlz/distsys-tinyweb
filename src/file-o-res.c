/* 
FILE-O-RES
LABORÜBUNG 1
Schulz, Reutebuch, Polkehn
*/

// Bibliotheken, die immer empfehlenswert sind
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

#define BUFSIZE 100

static int accept_clients(int sd, char * response_file);
static int handle_client(int sd, char * response_file, struct sockaddr_in * from_client, int req_no);

// GLOBAL REQUEST COUNTER
static int request_counter = 0;

int main(int argc, char **argv)
{
	int port;
	
	// Teste ob alle Parameter gegeben sind.
	if (argc < 3)
	{
		printf("[ERR] Not enough arguments given. Exiting.\n");
		printf("[INFO] Syntax is ./file-o-res <PORT> <RESPONSE_FILE>\n");
		exit(1);
	}
	
	// Lese die Portnummer aus dem ersten Argument
	port = atoi(argv[1]);
	
	// Erzeuge ein listening socket, auf dem requests angenommen werden
	int sd = passive_tcp(port, 5);
	
	// sd < 0 weist auf einen Fehler hin
	if (sd < 0)
	{
		printf("[ERR #%d] Error when creating listening socket. Exiting.\n", sd);
	}
	
	// Fange an Clients zu bearbeiten (Endlos)
	accept_clients(sd, argv[2]);	
	
	// Beende das Programm	
	exit(0);
}

// Das static sorgt dafür das die Funktion "Modullokal" wird. Wird das Modul zu anderen Dateien 
// hinzugelinkt, ist die Funktion für diese nicht sichtbar. (vgl. private/protected in JAVA)
/* PURPOSE: Akzeptiere Requests in einer Endlosschleife und erzeuge Kindprozesse die die Requests bearbeiten*/
static int accept_clients(int sd, char * response_file)
{	
	// Descriptor fuer die Listening socket deklarieren
	int nsd; 
	
	// Struktur fuer die Adrese des Client deklarieren
	struct sockaddr_in from_client;
	
	while(1)
	{		
		// Bestimme die Länge der Struktur
		// muss neu initialisiert werden, weil accept(...) die Struktur überschreibt
		socklen_t from_client_len  = sizeof(from_client);
		
		// Aufruf erwartet eine GENERISCHE Struktur, die ipv4-spezifische wird "reingecastet"
		nsd = accept(/* in */sd, /*in out */(struct sockaddr *) &from_client, /*in out*/ &from_client_len);
		
		// Requesz-Zaehler erhoehen und dem Request eine Nummer zuweisen
		request_counter++;
		int req_no = request_counter;
		
		// Fork durchführen, damit ein Kindprozess den Request bearbeiten kann
		pid_t child_pid;
		if ((child_pid = fork()))
		{	
			// Vaterprozess
			// Accepting socket schließen
			close(nsd);
			
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
			close(sd);
			
			int ret;
			struct rusage usage;
			time_t end_time;
			
			// Client bearbeiten
			ret = handle_client(nsd, response_file, &from_client, req_no);
			
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
		
	}
	
	// Das hier wird nur ausgeführt wenn ein Fehler aufgetreten ist
	// nsd enthält in diesem Fall den Fehlercode (deshalb wird nsd als exit code zurückgegeben)
	return nsd;
}

/* PURPOSE: Schreibe einen HTTP 1.1 Header in das socket sd */
static int write_res_header(int sd, time_t res_time)
{   // \\ backslash
    char res_header;
    char timestr[BUFSIZE];
	
	char * status_line = "HTTP/1.1 200 OK\r\n";
	char header[BUFSIZE];
    
//    res_header = "HTTP/1.1 200 OK \r\n"; // CRLF \r\n
//    res_header[18] = "Date: ";
    
    struct tm *ts;
	ts = localtime(&res_time);
	strftime(timestr, BUFSIZE, "%a, %d %b %Y %T %z", ts);
	
	sprintf(header, "Date:%s", timestr);
	
	// concatenate timestr to res_header
    sprintf(&res_header, "%s"\
					     "%s\r\n"\
						 "\r\n"
			,(char *)status_line, (char *)header);
    
    // write header & time to Socket
    int err = write(sd, &res_header, strlen(&res_header));
	
    if ( err < 0 ){
		printf("[ERR #%d] Error when writing header. Exiting.\n", err);
        exit(err); // good luck searching the unexpected exit point
	}
	
	return 0;
}

/* PURPOSE: Schreibe einen HTTP response body in das socket sd */
static int write_res_body(int sd, time_t res_time, char * template_file)
{	
	char response_body[BUFSIZE];
	char response_template[BUFSIZE];
	char  timestr[BUFSIZE];
	struct tm *ts;
		
	ts = localtime(&res_time);
	strftime(timestr, BUFSIZE, "%a, %d %b %Y %T %z", ts);	
	
	// read html from file
	
	sprintf(response_body, 	"<html>"\
								"<head>"\
									"<title>Server file-o-res.c - Uhrzeit</title>"
								"</head>"\
								"<body>"\
									"Uhrzeit: %s"\
								"</body>"\
							"</html>"
			, timestr);
	
	//printf("Length: %lu\n", strlen(response_body));
	//fwrite(response_body, strlen(response_body), sizeof(char), stdout);
	write(sd, response_body, strlen(response_body)); 
	//write(sd, "\0", sizeof(char));
	return 0;	
}

/* PURPOSE: Behandle die Request EINES clients und sende eine response zurueck */
int handle_client(int sd, char * response_file,struct sockaddr_in * from_client, int req_no){
	
	request_counter++;
	
	// BUFSIZE ist als globale konstante #definiert 
	char buf[BUFSIZE];
	int cc; // Character count
	
	// Die aktuelle Zeit wird bestimmt um sie weiter unten den Methoden write_header und ..body zu uebergeben
	time_t current_time = time(NULL);
	printf("[REQ #%d] SRC %s:%d\n",req_no, inet_ntoa(from_client->sin_addr), ntohs(from_client->sin_port));
		
	// Die Request wird in den Buffer gelesen
	cc = read(sd, buf, BUFSIZE);
	
	// Wenn der characater count < 0 ist, ist ein Fehler aufgetreten.
	if (cc < 0)
	{
		printf("Error when reading request -> Exiting\n");
		close(sd);
		exit(sd);
	}
	
	buf[cc] = '\0';
	printf("%s\n", buf);
	printf("[RES #%d] DST %s:%d\n", req_no, inet_ntoa(from_client->sin_addr), ntohs(from_client->sin_port));
	
	// Response Header und Body in die socket schreiben
	write_res_header(sd, current_time );
	write_res_body(sd, current_time, response_file );
	
	
	// Flush socket - Alles durchschreiben
	shutdown(sd, SHUT_RDWR);
	
	// Verbindung schließen
	close(sd);

	return(sd);	
}	