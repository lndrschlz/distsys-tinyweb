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

#include <sys/stat.h>
#include <fcntl.h>


static int accept_clients(int sd, char * response_file);
static int handle_client(int sd, char * response_file);

int main(int argc, char **argv)
{
	int port;
	
	// Gibt einen Fehler wenn es keinen ersten Parameter gibt (Null Pointer)
	if (argc < 3)
	{
		printf("Error: no port or file given.\n");
		exit(1);
	}
	
	port = atoi(argv[1]);
	
	// Liefert einen socket zurück (Fehlerbehandlung für sd < 0 möglich)
	// sd ist ein Filedescriptor
	int sd = passive_tcp(port);
	
	if (sd < 0)
	{
		/* Fehler */
	}
	
	accept_clients(sd, argv[2]);		
	exit(0);
}

// Das static sorgt dafür das die Funktion "Modullokal" wird. Wird das Modul zu anderen Dateien 
// hinzugelinkt, ist die Funktion für diese nicht sichtbar. (vgl. private/protected in JAVA)
static int accept_clients(int sd, char * response_file)
{	
	// return code und new socket descriptor
	int retcode, nsd; 
	
	struct sockaddr_in from_client;
	
	while(1)
	{		
		// Bestimme die Länge der Struktur
		// muss neu initialisiert werden, weil accept(...) die Struktur überschreibt
		int from_client_len  = sizeof(from_client);
		
		// Aufruf erwartet eine GENERISCHE Struktur, die ipv4-spezifische wird "reingecastet"
		nsd = accept(/* in */sd, /*in out */(struct sockaddr *) &from_client, /*in out*/ &from_client_len);
		
		// Fehler: break;
			
		// Server kann immer nur einen Client gleichzeitig verarbeiten, der nächste Client wird erst akzeptiert
		// wenn handle_client() durchgelaufen ist. Es empfiehlt sich einen fork() durchzuführen und handle_client()
		// erst im Kindprozess auszuführen.
		handle_client(nsd, response_file);
	}
	
	// Das hier wird nur ausgeführt wenn ein Fehler aufgetreten ist
	// nsd enthält in diesem Fall den Fehlercode (deshalb wird nsd als exit code zurückgegeben)
	return nsd;
}


#define BUFSIZE 100

int handle_client(int sd, char * response_file){
	
	// BUFSIZE ist als globale konstante #definiert 
	char buf[BUFSIZE];
	
	int cc; // Character count
		
	// Der Rückgabewert von read wird gleichzeitig cc zugewiesen und von while überprüft
	cc = read(sd, buf, BUFSIZE);
	
		if (cc < 0)
		{
			printf("Error when reading request -> Exiting\n");
			exit(sd);
		}
		printf("\n==========\nREQUEST\n==========\n");
		buf[cc] = '\0';
		printf("%s", buf);
		printf("\n==========\nRESPONSE\n==========\n");
	
	    int fd = open(response_file, O_RDONLY);
		
		while (cc = read(fd, buf, BUFSIZE))
		{
			if (cc < 0)
			{
				printf("Error when reading response file (No. %d) -> Exiting\n", cc);
				exit(cc);
			}
			buf[cc] = '\0';
			printf("%s", buf);
			write(sd, buf, cc);
		}
		close(fd);
	
	printf("\n===========\n");
			
	// Verbindung schließen
	close(sd);

	return(sd);	
}