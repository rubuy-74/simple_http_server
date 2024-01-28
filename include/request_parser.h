#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFERSIZE 1024
#define HTTP_VERSION "HTTP/1.1"
#define SOURCE_PATH "../www/"

enum REQUEST_TYPE {
    GET,
    HEAD,
    ERROR,
    DEFAULT
};;

int parse_file(char *file_path,char *start_line, char *host_line);

void get_host(char *raw_request,char* host_path);

enum REQUEST_TYPE parse_start_line(char* start_line,char* file_path);

void get_host_name(char *host_line,char *host_name);

uint8_t get_payload(char *file_path,char *host_name,char *host_line);

uint8_t absolute_path(char *file_path);