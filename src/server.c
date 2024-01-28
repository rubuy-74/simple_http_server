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
#include "../include/request_parser.h"
#include "../include/response_creator.h"

// #define PORT "8080"
#define BACKLOG 3
#define BUFFERSIZE 1024
#define MAXRESPONSESIZE 32768


void handler(int new_socket) {
    char *file_path = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *start_line = "GET /index.html HTTP/1.1";

    printf("fuck off\n");
    /*
    char *raw_request = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *file = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *host_path = (char *) malloc(BUFFERSIZE * sizeof(char));
    char response[MAXRESPONSESIZE];
    int response_size = 0;


    if(recv(new_socket,raw_request,BUFFERSIZE,0) == -1){
        perror("response"); exit(1);
    }

    if(parse_file(new_socket,raw_request,file) == 0){
        get_host(raw_request,host_path);

        strcat(host_path,file);
        strcpy(file,host_path);

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
    */
}

void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main2(int argc,char const* argv[]){
    char *file_path = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *start_line = "GET /index.html HTTP/1.1";
    char *host_line = "Host: example.com";

    char *example_request = "GET /index.html HTTP/1.1"
    "Host: example.com"
    "";

    // parse_file(file_path,start_line,host_line);


}

void setup_sockets(struct addrinfo *servinfo,int *server_socket, char *a){
    int opt = 1;
    struct addrinfo hints;
    struct addrinfo *p;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC; // ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // FILL MY IP

    if(getaddrinfo(NULL,a,&hints,&servinfo) != 0){
        perror("getaddrinfo");
        exit(1);
    }

    for(p = servinfo; p != NULL; p = servinfo->ai_next) {
        if((*server_socket = socket(p->ai_family,p->ai_socktype,0)) == -1){
            perror("socket");
            exit(1);
        }
        
        if((setsockopt(*server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))) == -1){
            perror("setsockopt");
            exit(1);
        }

        if(bind(*server_socket,p->ai_addr,p->ai_addrlen) == -1){
            perror("bind");
            exit(1);
        }
        break;
    }

    freeaddrinfo(servinfo);

    if(listen(*server_socket,1) == - 1){
        perror("listen");
        exit(1);
    }
}

int main(int argc,char const* argv[]){
    int server_socket,new_socket;
    socklen_t addr_length;
    struct sockaddr_storage client_addr;
    struct sigaction sa;
    struct addrinfo *servinfo;
    char s[INET6_ADDRSTRLEN] = {0};

    setup_sockets(servinfo,&server_socket,argv[1]);

    printf("waiting for connection on Port %s\n",argv[1]);

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


