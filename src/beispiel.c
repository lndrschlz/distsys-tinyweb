/* 
* Beispielprogramm von Hr. Reutemann
* 21.05.2015 - Tafelabschrieb 
*/

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
	}
}
