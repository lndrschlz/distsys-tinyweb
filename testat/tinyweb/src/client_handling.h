/*
 * module name:         tinyweb.h
 * author:              Jonas Pohlken, Leander Schulz, Christian Reutebuch
 * purpose:             headerfile for client_handle module
 *
 */

int accept_client(int sd, int nsd);
int send_response(http_res_t * response,int sd);
int parse_request(http_req_t * request, char *req_string);