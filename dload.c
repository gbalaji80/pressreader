#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>
#include "buildurl.h"

//extern char* fName;
//size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int main(int c, char* argv[]) { 
    fName = argv[1];
    int num_lines;
    getPageKeys(fName,argv[2]);
    char** lines_array = read_lines_from_file("out.txt", &num_lines);
    printf("Total keys fetched:\t%d\n", num_lines);
    int i=0;
    while (i < num_lines) {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
    	if (curl) {
	  int len = snprintf(NULL, 0,"wsj_%d.jpg", i);
	  char* outfilename = malloc(len + 1);
	  if(i < 10) {
	   sprintf(outfilename,"wsj_0%d.jpg",i);
	  } else {
	   sprintf(outfilename,"wsj_%d.jpg",i);
	  }
          fp = fopen(outfilename, "wb");
          curl_easy_setopt(curl, CURLOPT_URL, buildURL(i+1,lines_array[i]));
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
          res = curl_easy_perform(curl);
          /* clean up */
          curl_easy_cleanup(curl);
          fclose(fp);
	  free(lines_array[i]);
        }
	i=i+1;
    }
    free(lines_array);
    return 0;
}


