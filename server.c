#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <regex.h>


#define PORT "8080"
#define BACKLOG 3
#define BUFFERSIZE 1024
#define MAXRESPONSESIZE 32768

struct MimeType {
   char* extension;
   char* type;
};

static const struct MimeType mimeTypeList[] =
{
  
    //Text MIME types
    {".css",   "text/css"},
    {".csv",   "text/csv"},
    {".htc",   "text/x-component"},
    {".htm",   "text/html"},
    {".html",  "text/html"},
    {".shtm",  "text/html"},
    {".shtml", "text/html"},
    {".stm",   "text/html"},
    {".txt",   "text/plain"},
    {".vcf",   "text/vcard"},
    {".vcard", "text/vcard"},
    {".xml",   "text/xml"},
  
    //Image MIME types
    {".gif",   "image/gif"},
    {".ico",   "image/x-icon"},
    {".jpg",   "image/jpeg"},
    {".jpeg",  "image/jpeg"},
    {".png",   "image/png"},
    {".svg",   "image/svg+xml"},
    {".tif",   "image/tiff"},
  
    //Audio MIME types
    {".aac",   "audio/x-aac"},
    {".aif",   "audio/x-aiff"},
    {".mp3",   "audio/mpeg"},
    {".wav",   "audio/x-wav"},
    {".wma",   "audio/x-ms-wma"},
  
    //Video MIME types
    {".avi",   "video/x-msvideo"},
    {".flv",   "video/x-flv"},
    {".mov",   "video/quicktime"},
    {".mp4",   "video/mp4"},
    {".mpg",   "video/mpeg"},
    {".mpeg",  "video/mpeg"},
    {".wmv",   "video/x-ms-wmv"},
  
    //Application MIME types
    {".doc",   "application/msword"},
    {".gz",    "application/x-gzip"},
    {".gzip",  "application/x-gzip"},
    {".js",    "application/javascript"},
    {".json",  "application/json"},
    {".ogg",   "application/ogg"},
    {".pdf",   "application/pdf"},
    {".ppt",   "application/vnd.ms-powerpoint"},
    {".rar",   "application/x-rar-compressed"},
    {".rtf",   "application/rtf"},
    {".tar",   "application/x-tar"},
    {".tgz",   "application/x-gzip"},
    {".xht",   "application/xhtml+xml"},
    {".xhtml", "application/xhtml+xml"},
    {".xls",   "application/vnd.ms-excel"},
    {".zip",   "application/zip"}
 };

const char *concat(char *dest, char *source) {
    int i;

    for(i = 0; dest[i]!='\0';i++);
    for(int j = 0; source[j]!='\0';j++) {
        dest[i] = source[j];
        i++;
    }
    dest[i] = '\0';

}

const char * get_mime_type(char *filename) {
   uint8_t i;
   uint8_t n;
   static const char default_mimeType[] = "application/octet-stream";

   char *extension = (char *)malloc(32 * sizeof(char));
   extension = strchr(filename,'.');

   for(i = 0; i < sizeof(mimeTypeList) / sizeof(mimeTypeList[0]); i++) {
      if(strcmp(mimeTypeList[i].extension,extension) == 0) {
         return mimeTypeList[i].type;
      }
   }
   return default_mimeType;
}

int create_response(int new_socket,char *file, char *response,int *response_size) {
    char *data = 0;
    char *header = (char *) malloc(BUFFERSIZE * sizeof(char));
    long length;
    FILE *f = fopen(file,"rb");

    if(f > 0){
        const char * mime_type = get_mime_type(file);
        snprintf(header,BUFFERSIZE,
            "HTTP/1.1 200 OK\r\n"
            "Content-type: %s\r\n"
            "\r\n", mime_type);

        *response_size = 0;
        memcpy(response,header,strlen(header));
        *response_size += strlen(header);

        fseek(f,0,SEEK_END);
        length = ftell(f);
        fseek(f,0,SEEK_SET);
        data = (char *) malloc(length);
        if(data) {
            fread(data,1,length,f);
        }
        fclose(f);

        concat(response,data);
        *response_size += strlen(data);

        free(data);
    } else {
        snprintf(response,BUFFERSIZE,
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "404 Not Found");
        *response_size += strlen(response);
        return 0;
    }
    return 0;
};

int parse_file(int new_socket,char *raw_request,char *file){
    int status;
    regex_t reg;
    regmatch_t pmatch[2];

    char *rstr = "GET /([^\?!]*) HTTP/1.1";
    if ((status = regcomp(&reg,rstr,REG_EXTENDED)) == - 1){
        perror("regcomp"); exit(1);
    }
    status = regexec(&reg,raw_request,2, pmatch, 0);
    //printf("%d %s",status,raw_request);
    if(status == 0){
        int v = pmatch[1].rm_so;
        char *cp = (char *)malloc(BUFFERSIZE * sizeof(char));
        strcpy(cp,raw_request);
        while(v > 0) {cp++;v--;}
        cp[pmatch[1].rm_eo - pmatch[1].rm_so] = 0;
        strcpy(file,cp);
        if(strcmp(file,"") == 0) {
            strcpy(file,"index.html");
        }
    } else {
        return 1;
    }
    return 0;
}

void handler(int new_socket) {
    char *raw_request = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *file = (char *) malloc(BUFFERSIZE * sizeof(char));
    char response[MAXRESPONSESIZE];
    int response_size = 0;


    if(recv(new_socket,raw_request,BUFFERSIZE,0) == -1){
        perror("response"); exit(1);
    }

    //printf("%s\n",raw_request);

    if(parse_file(new_socket,raw_request,file) == 0){
        char response[MAXRESPONSESIZE];

        printf("%s\n",file);

        if((create_response(new_socket,file,response,&response_size)) == -1){
            printf("error while creating response");
        }
    } else {
        printf("NOT VALID REQUEST:%s\n",raw_request);
    }


    printf("%s\n",response);
    send(new_socket,response,response_size,0);

    free(raw_request);
    free(file);
    return;
}

void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc,char const* argv[]){
    int server_socket,new_socket;
    struct addrinfo hints;
    socklen_t addr_length;
    struct sockaddr_storage client_addr;
    struct sigaction sa;
    struct addrinfo *servinfo, *p;
    char s[INET6_ADDRSTRLEN] = {0};
    int opt = 1;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC; // ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // FILL MY IP

    if(getaddrinfo(NULL,PORT,&hints,&servinfo) != 0){
        perror("getaddrinfo");
        exit(1);
    }

    for(p = servinfo; p != NULL; p = servinfo->ai_next) {
        if((server_socket = socket(p->ai_family,p->ai_socktype,0)) == -1){
            perror("socket");
            exit(1);
        }
        
        if((setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))) == -1){
            perror("setsockopt");
            exit(1);
        }

        if(bind(server_socket,p->ai_addr,p->ai_addrlen) == -1){
            perror("bind");
            exit(1);
        }
        break;
    }

    freeaddrinfo(servinfo);

    if(listen(server_socket,1) == - 1){
        perror("listen");
        exit(1);
    }

    printf("waiting for connection on Port %s\n",PORT);

    while(1) {
        addr_length = sizeof client_addr;
        new_socket = accept(server_socket,(struct sockaddr*)&client_addr,&addr_length);

        if(new_socket == -1){
            perror("accept");
            exit(1);
        }

        inet_ntop(
            client_addr.ss_family,
            get_in_addr((struct sockaddr*)&client_addr),
            s,
            sizeof s
        );

        printf("connected to %s\n",s);

        pthread_t t1;
        pthread_create(&t1,NULL,handler,new_socket);
        pthread_join(t1,NULL);
        close(new_socket);
    }
    return 0;
}


