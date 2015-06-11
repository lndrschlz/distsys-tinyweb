
#include <string.h>
#include <tinyweb.h>

#define BUFSIZE 100000

int parse_version(char * buffer, http_req_t * request)
{
	return 0;
}

int parse_resource_string(char * buffer, http_req_t * request)
{
	char resource_string[255];
	
	char * walker = strstr(buffer, " ");
	if (walker)
	{
		request->resource = strncpy(resource_string, buffer, walker-buffer );		
		parse_version(++walker, request);
		return 0;
	}	
	return -1;	
}

int parse_method(char * buffer, http_req_t * request){
	char * walker; 
	
	walker = buffer;
	
	if (*walker == 'G')
	{
		if (*(++walker) != 'E') { return -1;}
		if (*(++walker) != 'T') { return -1;}
		request->method = HTTP_METHOD_GET;
	}
	if (*walker == 'H')
	{
		if (*(++walker) != 'E') { return -1;}
		if (*(++walker) != 'A') { return -1;}
		if (*(++walker) != 'D') { return -1;}
		request->method = HTTP_METHOD_HEAD;
	}
	
	if (*(++walker) != ' ') { return -1;}
	
	int err = parse_resource_string(++walker, request);
	return err;
}

int parse_request(http_req_t * request, char *req_string)
{
	//char buffer[BUFSIZE];
	//strcpy(buffer, req_string);
	
	char * buffer = "GET /test/resource/test.jpg HTTP/1.1\r\n";
	int err = parse_method(buffer, request);
	if (err < 0 )
	{
		printf("Error on parsing request!\n");
	}
	return err;
}

