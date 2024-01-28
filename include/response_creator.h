#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define BUFFERSIZE 1024

struct MimeType {
   char* extension;
   char* type;
};

const char * get_mime_type(char *filename);

int create_response(int new_socket,char *file, char *response,int *response_size);