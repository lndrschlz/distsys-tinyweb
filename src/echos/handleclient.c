/*
* Modul handleclient für Beispielprogramm echos.c von Hr. Reutemann
* Tafelabschrieb - 21.05.2015
*/

#define BUFSIZE 1000

int handle_client(int sd){
	
	// BUFSIZE ist als globale konstante #definiert 
	char buf[BUFSIZE];
	
	int cc; // Character count
	
	// Der Rückgabewert von read wird gleichzeitig cc zugewiesen und von while überprüft
	while (cc = read(sd, buf, BUFSIZE))
	{
		if (cc < 0)
		{
			// Fehler
		}
		
		// Mit dem write(...) wird der gerade eingelesene buffer wieder zurückgeschrieben
		if (write(sd, buf, cc) < 0)
		{
			// Fehler
		}		
	}
	// Verbindung schließen
	close(sd);

	return(sd);	
}