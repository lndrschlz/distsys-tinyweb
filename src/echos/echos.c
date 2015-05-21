/* 
* Beispielprogramm von Hr. Reutemann
* 21.05.2015 - Tafelabschrieb 
*
* Komplilieren:
* make echos
* ODER:
* gcc -o echos echos.c -O2 -g -m32 -Wall -Werror
*/

#include <stdlib.h>
#include "handleclient.h"

int main(int argc, char **argv)
{
	int port;
	
	// Gibt einen Fehler wenn es keinen ersten Parameter gibt (Null Pointer)
	port = atoi(argv[1]);
	
	// Liefert einen socket zurück (Fehlerbehandlung für sd < 0 möglich)
	// sd ist ein Filedescriptor
	int sd = passive_tcp(port);
	
	if (sd < 0)
	{
		/* Fehler */
	}
	
	accept_clients(sd);		
	exit(0);
}

// Das static sorgt dafür das die Funktion "Modullokal" wird. Wird das Modul zu anderen Dateien 
// hinzugelinkt, ist die Funktion für diese nicht sichtbar. (vgl. private/protected in JAVA)
static int accept_clients(int sd)
{	
	// return code und new socket descriptor
	int retcode, nsd; 
	
	struct sockaddr_in from_client;
	
	while(1)
	{		
		// Bestimme die Länge der Struktur
		// muss neu initialisiert werden, weil accept(...) die Struktur überschreibt
		from_client_len  = sizeof(from_client);
		
		// Aufruf erwartet eine GENERISCHE Struktur, die ipv4-spezifische wird "reingecastet"
		nsd = accept(/* in */sd, /*in out */(struct sockaddr *) &from_client, /*in out*/ &from_client_len);
		
		// Fehler: break;
			
		// Server kann immer nur einen Client gleichzeitig verarbeiten, der nächste Client wird erst akzeptiert
		// wenn handle_client() durchgelaufen ist. Es empfiehlt sich einen fork() durchzuführen und handle_client()
		// erst im Kindprozess auszuführen.
		handle_client(nsd);
	}
	
	// Das hier wird nur ausgeführt wenn ein Fehler aufgetreten ist
	// nsd enthält in diesem Fall den Fehlercode (deshalb wird nsd als exit code zurückgegeben)
	return nsd;
}
