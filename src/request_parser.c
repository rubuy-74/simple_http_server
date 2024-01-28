#include "../include/request_parser.h"

int parse_file(char *file_path,char *start_line, char *host_line){
    char *host_name = (char *) malloc(BUFFERSIZE * sizeof(char));

    uint8_t request_type = parse_start_line(start_line,file_path);

    if(request_type == GET && get_payload(file_path,host_name,host_line) == 0) {
        char *tmp = strdup(SOURCE_PATH);
        strcat(tmp,file_path);
        strcpy(file_path,tmp);

        free(tmp);        
        return 0;
    } else if (request_type == HEAD) {
        printf("HEAD REQUEST\n");
        return 0;
    } else {
        printf("%d : ERROR\n",request_type);
        return -1;
    }
}

enum REQUEST_TYPE parse_start_line(char* start_line,char* file_path) {
    char *arr[1024];
    char *token = (char *)malloc(BUFFERSIZE * sizeof(char));
    char *string = (char *)malloc(BUFFERSIZE * sizeof(char));
    const char space[2] = " ";

    strcpy(string,start_line);

    token = strtok(string,space);

    int i = 0;
    while(token != NULL) {
        arr[i] = token;
        i++;
        token = strtok(NULL,space);
    }
    arr[i] = NULL;

    int j = 0;
    while(arr[j] != NULL) j++;

    if(strcmp(arr[2],HTTP_VERSION) != 0) return ERROR;

    if((j != 3 && j != 2)) return ERROR;
    if(strcmp(arr[0],"HEAD") == 0 || j == 2) return HEAD;

    strcpy(file_path,arr[1]);
    if(strcmp(arr[0],"GET") == 0) return GET;
    
    return ERROR;
}

uint8_t get_payload(char *file_path,char *host_name,char *host_line) {
    if(!absolute_path(file_path) && strlen(host_line) == 0) return 1;
    if(absolute_path(file_path)){
        strcpy(file_path,file_path + strlen("http://"));
    } else if (file_path[0] == '/' && strlen(host_line) != 0){
        get_host_name(host_line,host_name);
        strcat(host_name,file_path);
        strcpy(file_path,host_name);
    }
    return 0;
}

void get_host_name(char *host_line,char *host_name) {
    strcpy(host_name,strchr(strtok(host_line,"\n"),':') + 2);
}

uint8_t absolute_path(char *file_path) {
    int status;
    regex_t reg;

    char *rstr = "http://*";
    if ((status = regcomp(&reg,rstr,REG_EXTENDED)) == - 1){
        perror("regcomp"); exit(1);
    }
    status = regexec(&reg,file_path,0, NULL, 0);
    return !status;
}

