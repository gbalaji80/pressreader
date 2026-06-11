#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#ifndef buildURL_H
#define buildURL_H
#define STR_SIZE 100

extern char* fName;
extern char* baseUrl;
extern size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
char* buildURL(const int pageNum, const char* pageKey);
char** read_lines_from_file(const char* filename, int* num_lines_out);
void getPageKeys(char* issueNumber, char* bearerToken);
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the data received by libcurl
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0; // Null-terminate the string

    return realsize;
}

#endif 
