#include <safe_print.h>
#include <string.h>
#include <tinyweb.h>

#define BUFSIZE 1000

int parse_header(char * buffer, http_req_t * request)
{
	return 0;
}

int parse_version(char * buffer, http_req_t * request)
{	
	char version_string[9];
	char * walker = strstr(buffer, "\r\n");
	
	if (walker)
	{	
		strncpy(version_string, buffer, walker-buffer);
		if (strcmp(version_string, "HTTP/1.1") == 0)
		{
			return parse_header(walker + 2, request);
		}
		return -1;
	}
	return -1;
}

int parse_resource_string(char * buffer, http_req_t * request)
{
	char resource_string[255];
	
	char * walker = strstr(buffer, " ");
	if (walker)
	{
		strncpy(resource_string, buffer, walker-buffer );	
		request->resource = resource_string;
		safe_printf("Resource String: %s\n" , resource_string);
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
	char buffer[BUFSIZE];
	strcpy(buffer, req_string);

	int err = parse_method(buffer, request);
	if (err < 0 )
	{
		safe_printf("Error on parsing request!\n");
	}
	return err;
}

