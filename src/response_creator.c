#include "../include/response_creator.h"

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

void concat(char *dest, char *source) {
    int i;

    for(i = 0; dest[i]!='\0';i++);
    for(int j = 0; source[j]!='\0';j++) {
        dest[i] = source[j];
        i++;
    }
    dest[i] = '\0';

}

const char *file_suffix(const char path[])
{
    const char *result;
    int i, n;

    assert(path != NULL);
    n = strlen(path);
    i = n - 1;
    while ((i > 0) && (path[i] != '.') && (path[i] != '/') && (path[i] != '\\')) {
        i--;
    }
    if ((i > 0) && (i < n - 1) && (path[i] == '.') && (path[i - 1] != '/') && (path[i - 1] != '\\')) {
        result = path + i;
    } else {
        result = path + n;
    }
    return result;
}

const char * get_mime_type(char *filename) {
   uint8_t i;
   uint8_t n;
   static const char default_mimeType[] = "application/octet-stream";

   char *extension = file_suffix(filename);

   for(i = 0; i < sizeof(mimeTypeList) / sizeof(mimeTypeList[0]); i++) {
      if(strcmp(mimeTypeList[i].extension,extension) == 0) {
         return mimeTypeList[i].type;
      }
   }
   return default_mimeType;
}

int create_response(int new_socket,char *path_file,int status_code, char *response, int *response_size){
    // create_header(status_code,path_file);
    FILE *f = fopen(path_file,"rb");
    char *data = 0;
    long length;


    if(f == 0 && status_code != 400){
        status_code = 404;
    }
    create_header(status_code,path_file,response);
        if(f > 0) {
            fseek(f,0,SEEK_END);
            length = ftell(f);
            fseek(f,0,SEEK_SET);
            data = (char *) malloc(length);
        if(data) {
            fread(data,1,length,f);
        }
        fclose(f);
        concat(response,data);
    }
    *response_size += strlen(response);
}

int create_header(int status_code,char *path_file,char *response){
    char *header = (char *) malloc(BUFFERSIZE * sizeof(char));
    char *status_message = (char *) malloc(BUFFERSIZE * sizeof(char));

    const char *mime_type = status_code != 404 
        ? get_mime_type(path_file) 
        : "text/plain";

    switch (status_code) {
        case 200:
            status_message = "OK"; break;
        case 404:
            status_message = "Not found"; break;
        case 400:
            status_message = "Bad Request"; break;
    }
    sprintf(header,"HTTP/1.1 %d %s\nContent-type: %s\n\n",status_code,status_message,mime_type);
    
    strcpy(response,header);
}

/*
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
    }

    free(header);
    return 0;
};
*/