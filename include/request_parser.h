#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFERSIZE 1024
#define HTTP_VERSION "HTTP/1.1"
#define SOURCE_PATH "../www/"
#define MAX_HEADER_NUM 16
#define LINE_SIZE 2048

enum REQUEST_TYPE {
    GET,
    HEAD,
    ERROR,
    DEFAULT
};

struct header_field {
    char *key;
    char *value;
};

int parse_file(char *file_path,char *start_line, char *host_line);

enum REQUEST_TYPE parse_start_line(char* start_line,char* file_path);

void get_host_name(char *host_line,char *host_name);

uint8_t get_payload(char *file_path,char *host_name,char *host_line);

uint8_t absolute_path(char *file_path);

void parse_response(char *raw_request,char *start_line,char *host_line);

void split_field(char *string,char *key,char *value);