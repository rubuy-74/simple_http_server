#include "../include/request_parser.h"

int parse_file(char *file_path,char *start_line, char *host_line, int *status_code){
    char *host_name = (char *) malloc(BUFFERSIZE * sizeof(char));

    uint8_t request_type = parse_start_line(start_line,file_path);

    uint8_t sla = request_type == GET && get_payload(file_path,host_name,host_line) == 0;

    if(sla) {
        char *tmp = strdup(SOURCE_PATH);
        strcat(tmp,file_path);
        strcpy(file_path,tmp);
        if("%d\n",file_path[strlen(file_path) - 1] == '/'){
            strcat(file_path,"index.html");
        }
        return 0;
    } else if (request_type == HEAD) {
        printf("HEAD REQUEST\n");
        return 0;
    } else {
        *status_code = 400;
        //printf("%d : ERROR\n",request_type);
        return -1;
    }
}

enum REQUEST_TYPE parse_start_line(char* start_line,char* file_path) {
    char *arr[LINE_SIZE];
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

    // if(strcmp(arr[2],HTTP_VERSION) != 0) return ERROR;

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

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void get_host_name(char *host_line,char *host_name) {
    char *semi_host = strchr(strtok(host_line,"\n"),':') + 2;
    if(prefix("www",semi_host)){
        semi_host = semi_host + 4;
    }
    strtok(semi_host,".");
    strcpy(host_name,semi_host);
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

uint8_t split_field(char *string,char *key,char *value){
    long i = (long)index(string,':') - (long)string;
    if(i > strlen(string)) return 1;
    strncpy(key,string,i);
    strcpy(value,string+i + 1);
    return 0;
}

void parse_request(char *raw_request,char *start_line,char *host_line,struct header_field *fields, int fields_size) {
    char *token;
    char *arr[LINE_SIZE];
    char *string = (char *) malloc(BUFFERSIZE * sizeof(char));

    strcpy(string,raw_request);
    
    token = strtok(string,"\n");

    int i = 0;
    while(token != NULL){
        arr[i] = token;
        i++;
        token = strtok(NULL,"\n");
    }
    arr[i] = NULL;

    strcpy(start_line,arr[0]);
    strcpy(host_line,arr[1]);

    char *key = (char *)malloc(BUFFERSIZE * sizeof(char));
    char *value = (char *)malloc(BUFFERSIZE * sizeof(char));

    int j = 2;

    while (split_field(arr[j],key,value) == 0 && arr[j] != NULL) {
        fields[j-2].value = strdup(value);
        fields[j-2].key = strdup(key);
        j++;
    }
    fields_size = j - 2;

    free(string);
    free(key);
    free(value);
    free(fields);
}