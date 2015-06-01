/***
* Programm: file-o-req
* Laboruebung 1, Aufgabe 2
* Schulz, Reutebuch, Polkehn (TIT12)
***/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <connect_tcp.h>

#define BUFSIZE 1000

int main(int argc, char **argv)
{
	// Check if all required arguments are given
	if (argc < 4 )
	{
		fprintf(stderr, "Error: %d arguments missing\n", 4 - argc);
		fprintf(stderr, "Usage: > file-o-req <SERVER> <PORT> <REQUEST_FILE>\n");
		exit(1);
	}
	
	// Open a tcp socket to the given server
	int port = atoi(argv[2]);
	int sd = connect_tcp(argv[1], port);
	
	// Open the given text file
	int fd = open(argv[3], O_RDONLY);
	
	// Loop and read the text file and write the buffer to the socket
	int cc;
	char buf[BUFSIZE];
	
	while ((cc = read(fd, buf, BUFSIZE)))
	{
		if (cc < 0)
		{
			fprintf(stderr, "Error: %d when reading file!\n", cc); 
			exit(1);
		}
		
		int err = write(sd, buf, cc);
		if ( err < 0 ){
		    printf("[ERR #%d] Error when writing buffer. Exiting.\n", err);
            exit(err); 
	    }
	}
	
	int fd_err = close(fd);
	if ( fd_err < 0 ) {
	    printf("Error #%d: close of file descriptor failed.\n", fd_err);
	    exit(fd_err);
	}
	
	printf("[INFO] Waiting for server response\n");
	
	// Loop and read the server response, write response to stdout
	while ((cc = read(sd, buf, BUFSIZE)))
	{
		if (cc < 0)
		{
			fprintf(stderr, "Error: %d when reading server response!\n", cc);
			exit(1);
		}
		
		fwrite(buf, sizeof(char), cc, stdout);
	}
		
	// Close the socket
	int sd_err = close(sd);
	if ( sd_err < 0 ) {
	    printf("Error #%d: close of socket descriptor failed.\n", sd_err);
	    exit(sd_err);
	}
	
	printf("[INFO] Connection closed.\n");
	
	// Exit the programmn
	exit(0);
	
}
