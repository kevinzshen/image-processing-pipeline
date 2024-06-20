#include "request.h"
#include "response.h"
#include <string.h>


/******************************************************************************
 * ClientState-processing functions
 *****************************************************************************/
ClientState *init_clients(int n) {
    ClientState *clients = malloc(sizeof(ClientState) * n);
    for (int i = 0; i < n; i++) {
        clients[i].sock = -1;  // -1 here indicates available entry
    }
    return clients;
}

/* 
 * Remove the client from the client array, free any memory allocated for
 * fields of the ClientState struct, and close the socket.
 */
void remove_client(ClientState *cs) {
    if (cs->reqData != NULL) {
        free(cs->reqData->method);
        cs->reqData->method = NULL;
        free(cs->reqData->path);
        cs->reqData->path = NULL;
        for (int i = 0; i < MAX_QUERY_PARAMS && cs->reqData->params[i].name != NULL; i++) {
            free(cs->reqData->params[i].name);
            cs->reqData->params[i].name = NULL;
            free(cs->reqData->params[i].value);
            cs->reqData->params[i].value = NULL;
        }
        free(cs->reqData);
        cs->reqData = NULL;
    }
    close(cs->sock);
    cs->sock = -1;
    cs->num_bytes = 0;
}


/*
 * Search the first inbuf characters of buf for a network newline ("\r\n").
 * Return the index *immediately after* the location of the '\n'
 * if the network newline is found, or -1 otherwise.
 * Definitely do not use strchr or any other string function in here. (Why not?)
 */
int find_network_newline(const char *buf, int inbuf) {
    //IMPLEMENT THIS [COMPLETE]
    int index = 0;
    for (int i = 0; i < inbuf - 1; i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n') {
            index = i + 2;
            return index;
        }
    }
    return -1;
}

/*
 * Removes one line (terminated by \r\n) from the client's buffer.
 * Update client->num_bytes accordingly.
 *
 * For example, if `client->buf` contains the string "hello\r\ngoodbye\r\nblah",
 * after calling remove_line on it, buf should contain "goodbye\r\nblah"
 * Remember that the client buffer is *not* null-terminated automatically.
 */
void remove_buffered_line(ClientState *client) {
    //IMPLEMENT THIS
    int where;
    int bytes_left;
    // if network newline exists in buf, find the first occurence and remove one line (content up to and including network newline)
    if ((where = find_network_newline(client->buf, client->num_bytes)) > 0) {
        // moves characters following network newline to the beginning of the buf
        memmove(client->buf, &client->buf[where], bytes_left = client->num_bytes - where);

        // update client->num_bytes
        client->num_bytes -= where;

        // clear the rest of the buffer
        memset(&client->buf[client->num_bytes], 0, MAXLINE - client->num_bytes);
        // null terminate buf
        client->buf[bytes_left] = '\0';
    }
}


/*
 * Read some data into the client buffer. Append new data to data already
 * in the buffer.  Update client->num_bytes accordingly.
 * Return the number of bytes read in, or -1 if the read failed.

 * Be very careful with memory here: there might be existing data in the buffer
 * that you don't want to overwrite, and you also don't want to go past
 * the end of the buffer, and you should ensure the string is null-terminated.
 */
int read_from_client(ClientState *client) {
    //IMPLEMENT THIS [COMPLETE]
    remove_buffered_line(client);
    int num_bytes = read(client->sock, client->buf, MAXLINE);
    if (num_bytes < 0) {
        perror("read from socket");
        return -1;
    }
    client->num_bytes += num_bytes;
    
    // replace the return line with something more appropriate
    return num_bytes;
}


/*****************************************************************************
 * Parsing the start line of an HTTP request.
 ****************************************************************************/
// Helper function declarations.
void parse_query(ReqData *req, const char *str);
void update_fdata(Fdata *f, const char *str);
void fdata_free(Fdata *f);
void log_request(const ReqData *req);


/* If there is a full line (terminated by a network newline (CRLF)) 
 * then use this line to initialize client->reqData
 * Return 0 if a full line has not been read, 1 otherwise.
 */
int parse_req_start_line(ClientState *client) {
    //IMPLEMENT THIS
    // if there is a full line (indicated by a network newline)
    if (find_network_newline(client->buf, client->num_bytes) == -1) {
        return 0;
    }

    // initialize client->reqData
    client->reqData = malloc(sizeof(ReqData));


    // use strtok_r() to determine whether or not the method provided is GET or POST
    char *ptr;
    char *delimiter = " ";
    char *duplicate = strdup(client->buf);

    // retrieve method from full line
    char *method = strtok_r(duplicate, delimiter, &ptr);
    // retrieve target from full line
    char *target = strtok_r(NULL, delimiter, &ptr);
    
    char *temp;
    int allocated = 0;
    if ((strcmp(method, "GET") == 0) && strchr(target, '?')) {
        char *ptr2;
        char *delimiter2 = "?";
        temp = strdup(target);
        allocated = 1;

        // target is set to everything before the "?" 
        target = strtok_r(temp, delimiter2, &ptr2);
        // parse query params for name-value pairs
        parse_query(client->reqData, ptr2);
    }
    
    // set method and path members of ReqData struct
    client->reqData->method = strdup(method);
    client->reqData->path = strdup(target);

    if (allocated) {
        free(temp);
        temp = NULL;
    }

    free(duplicate);
    duplicate = NULL;

    // This part is just for debugging purposes.
    // log_request(client->reqData);
    return 1; 
    }


/*
 * Initializes req->params from the key-value pairs contained in the given 
 * string.
 * Assumes that the string is the part after the '?' in the HTTP request target,
 * e.g., name1=value1&name2=value2.
 */
void parse_query(ReqData *req, const char *str) {
    //IMPLEMENT THIS
    // create a duplicate of str
    char *duplicate = strdup(str);
    if (!duplicate) {
        perror("duplicate failed");
    }

    for (int i = 0; i < MAX_QUERY_PARAMS; i++) {
        req->params[i].name = NULL;
        req->params[i].value = NULL;
    }

    int num_params = 0;
    char *ptr1;
    char *delimiter1 = "&";
    char *delimiter2 = "=";

    char *token = strtok_r(duplicate, delimiter1, &ptr1);
    while (token != NULL && num_params < MAX_QUERY_PARAMS) {
        // since each name value pair will have exactly 1 name and 1 value separated by a "="
        // call strtok_r on each token twice, once to get the name and once to get the value
        char *ptr2;

        char *name = strtok_r(token, delimiter2, &ptr2);
        char *value = strtok_r(NULL, delimiter2, &ptr2);

        // set members of Fdata struct
        if (name && value) {
            req->params[num_params].name = strdup(name);
            req->params[num_params].value = strdup(value);
            num_params++;
        }

        // get next token
        token = strtok_r(NULL, delimiter1, &ptr1);
    }
    free(duplicate);
    duplicate = NULL;
}




/*
 * Print information stored in the given request data to stderr.
 */
void log_request(const ReqData *req) {
    fprintf(stderr, "Request parsed: [%s] [%s]\n", req->method, req->path);
    for (int i = 0; i < MAX_QUERY_PARAMS && req->params[i].name != NULL; i++) {
        fprintf(stderr, "  %s -> %s\n", 
                req->params[i].name, req->params[i].value);
    }
}


/******************************************************************************
 * Parsing multipart form data (image-upload)
 *****************************************************************************/

char *get_boundary(ClientState *client) {
    int len_header = strlen(POST_BOUNDARY_HEADER);

    while (1) {
        int where = find_network_newline(client->buf, client->num_bytes);
        if (where > 0) {
            if (where < len_header || strncmp(POST_BOUNDARY_HEADER, client->buf, len_header) != 0) {
                remove_buffered_line(client);
            } else {
                // We've found the boundary string!
                // We are going to add "--" to the beginning to make it easier
                // to match the boundary line later
                char *boundary = malloc(where - len_header + 1);
                strncpy(boundary, "--", where - len_header + 1);
                strncat(boundary, client->buf + len_header, where - len_header - 1);
                boundary[where - len_header] = '\0';
                return boundary;
            }
        } else {
            // Need to read more bytes
            if (read_from_client(client) <= 0) {
                // Couldn't read; this is a bad request, so give up.
                return NULL;
            }
        }
    }
    return NULL;
}


char *get_bitmap_filename(ClientState *client, const char *boundary) {
    int len_boundary = strlen(boundary);

    // Read until finding the boundary string.
    while (1) {
        int where = find_network_newline(client->buf, client->num_bytes);
        if (where > 0) {
            if (where < len_boundary + 2 ||
                    strncmp(boundary, client->buf, len_boundary) != 0) {
                remove_buffered_line(client);
            } else {
                // We've found the line with the boundary!
                remove_buffered_line(client);
                break;
            }
        } else {
            // Need to read more bytes
            if (read_from_client(client) <= 0) {
                // Couldn't read; this is a bad request, so give up.
                return NULL;
            }
        }
    }

    int where = find_network_newline(client->buf, client->num_bytes);

    client->buf[where-1] = '\0';  // Used for strrchr to work on just the single line.
    char *raw_filename = strrchr(client->buf, '=') + 2;
    int len_filename = client->buf + where - 3 - raw_filename;
    char *filename = malloc(len_filename + 1);
    strncpy(filename, raw_filename, len_filename);
    filename[len_filename] = '\0';

    // Restore client->buf
    client->buf[where - 1] = '\n';
    remove_buffered_line(client);
    return filename;
}

/*
 * Read the file data from the socket and write it to the file descriptor
 * file_fd.
 * You know when you have reached the end of the file in one of two ways:
 *    - search for the boundary string in each chunk of data read 
 * (Remember the "\r\n" that comes before the boundary string, and the 
 * "--\r\n" that comes after.)
 *    - extract the file size from the bitmap data, and use that to determine
 * how many bytes to read from the socket and write to the file
 */
int save_file_upload(ClientState *client, const char *boundary, int file_fd) {
    // Read in the next two lines: Content-Type line, and empty line
    remove_buffered_line(client);
    remove_buffered_line(client);

    // IMPLEMENT THIS
    if(!(client->buf[0] == 'B' && client->buf[1] == 'M')) {
        int index = find_network_newline(client->buf, client->num_bytes);
        if(index == -1) {
            read_from_client(client);
            remove_buffered_line(client);
            remove_buffered_line(client);
        }
    }
    // Write the initial part of the bitmap data
    write(file_fd, client->buf, client->num_bytes);

    // Construct the boundary string
    char boundary_string[strlen(boundary) + 6];
    sprintf(boundary_string, "\r\n%s--\r\n", boundary);

    // Read and write data until the boundary string is found
    while (1) {
        int bytes_read = read(client->sock, client->buf, MAXLINE);
        if(bytes_read < 1) {
            break;
        }

        char *boundary_pos = strstr(client->buf, boundary_string);
        // Boundary string found
        if (boundary_pos) { 
            *boundary_pos = '\0';
            write(file_fd, client->buf, strlen(client->buf));
            break;
        }
        // Boundary string not found 
        else {
            write(file_fd, client->buf, bytes_read);
        }
    }
    return 0;
}
