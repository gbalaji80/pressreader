#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "buildurl.h"

char* baseUrl= "https://i.prcdn.co/img?file=";
char* fName = "";
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

char* buildURL(const int pageNum, const char* pageKey) {
    // 1. Calculate the required length for the format string
    // Format: base?id=%d&cat=%s&q=%s
    int len = snprintf(NULL, 0, "%s%s&page=%d&scale=220&ticket=%s", baseUrl, fName, pageNum, pageKey);

    // 2. Allocate exact memory needed (+1 for '\0')
    char *url = malloc(len + 1);
    if (url == NULL) return NULL; // Check for allocation failure

    // 3. Write the actual data into the buffer
    sprintf(url, "%s%s&page=%d&scale=220&ticket=%s", baseUrl, fName, pageNum, pageKey);
    printf("URL generated: %s\n",url );
    return url; // Caller must free this memory
} 

char** read_lines_from_file(const char* filename, int* num_lines_out) {
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Initial allocation for the array of line pointers
    int capacity = 10;
    char** lines = malloc(capacity * sizeof(char*));
    if (lines == NULL) {
        perror("Memory allocation failed for lines array");
        fclose(fptr);
        exit(EXIT_FAILURE);
    }

    char buffer[256]; // Buffer to hold each line temporarily
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        // Remove the trailing newline character if it exists
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Dynamically allocate memory for the current line's content and copy the data
        lines[line_count] = strdup(buffer); // strdup allocates memory and copies string
        if (lines[line_count] == NULL) {
            perror("Memory allocation failed for line content");
            // Free already allocated memory before exiting
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(fptr);
            exit(EXIT_FAILURE);
        }
        line_count++;

        // Resize the array of pointers if needed
        if (line_count >= capacity) {
            capacity *= 2;
            char** temp = realloc(lines, capacity * sizeof(char*));
            if (temp == NULL) {
                perror("Memory reallocation failed for lines array");
                // Free already allocated memory before exiting
                for (int i = 0; i < line_count; i++) {
                    free(lines[i]);
                }
                free(lines);
                fclose(fptr);
                exit(EXIT_FAILURE);
            }
            lines = temp;
        }
    }

    fclose(fptr);
    *num_lines_out = line_count;
    return lines;
}

 void getPageKeys(char* issueNumber, char* bearerToken) {

  int keyListLength = 0;	 
  char** keyList = malloc(sizeof(char*) * 100);
  
  CURL *curl;
  CURLcode res;

  struct MemoryStruct chunk;
  chunk.memory = malloc(1);  // will be grown as needed by realloc
  chunk.size = 0;    // no data at this point

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  //
   // Define URL components
    const char* base_url = "https://ingress.pressreader.com";
    int len = snprintf(NULL,0,"%s/services/IssueInfo/GetPageKeys?issue=%s&pageNumber=1&preview=true",base_url,issueNumber);

    // Declare a buffer with a sufficient size
    char* dynamic_url = malloc(len + 1);

    // Use snprintf to construct the URL safely
    // It returns the number of characters that would have been written
    sprintf(dynamic_url,"%s/services/IssueInfo/GetPageKeys?issue=%s&pageNumber=1&preview=true",base_url,issueNumber);

    // Print the constructed URL
    printf("Constructed URL: %s\n", dynamic_url);

    if(curl) {
      // Set the URL to fetch the JSON data from
      curl_easy_setopt(curl, CURLOPT_URL, dynamic_url);

      // Tell libcurl to write the response data to our custom callback function
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

      // Pass our 'chunk' struct to the callback function
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

      // Optionally, specify that you prefer a JSON response
      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers, "Accept: application/json");

      int len = snprintf(NULL, 0, "Authorization: Bearer %s", bearerToken);

      char* bToken = malloc(len + 1);
      sprintf(bToken, "Authorization: Bearer %s", bearerToken);
      printf("Constructed Bearer Token: %s\n",bToken);

      headers = curl_slist_append(headers, bToken);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Mobile Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl);

    // Check for errors
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
    // Data is in chunk.memory; print it or process it as JSON
      printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
      //printf("Received JSON data: %s\n", chunk.memory);
    // Here you would use a JSON parsing library (like json-c) to extract specific elements
    struct json_object *parsed_json = json_tokener_parse(chunk.memory);
    struct json_object *page_keys_array;

    if (parsed_json == NULL) {
      // Handle parsing error
      exit(1);
    }

    if (json_object_object_get_ex(parsed_json, "PageKeys", &page_keys_array)) {
        // 3. Get the length of the array
        int array_len = json_object_array_length(page_keys_array);
        printf("Total Number of page Keys: %d\n",array_len);

        //MagickWand *wand;
        //MagickBooleanType status;

        // 1. Initialize MagickWand
        //MagickWandGenesis();
        //wand = NewMagickWand();


        for (int i = 0; i < array_len; i++) {
            // 4. Get each object in the array by index
            struct json_object *item = json_object_array_get_idx(page_keys_array, i);
            struct json_object *key_obj;

            // 5. Extract the "Key" value from the item object
            if (json_object_object_get_ex(item, "Key", &key_obj)) {
		const char* pageKeyName=json_object_get_string(key_obj);

                printf("page Key Name %s\n", pageKeyName);
		////////////////////
		CURL *curl;
                FILE *fp;
                CURLcode res;
                curl = curl_easy_init();
    	        if (curl) {
	          int len = snprintf(NULL, 0,"wsj_%d.png", i);
	          char* outfilename = malloc(len + 1);
	          if(i < 10) {
	            sprintf(outfilename,"wsj_0%d.png",i);
                    //status = MagickReadImage(wand, "wsj_0%d.png");
	          } else {
	            sprintf(outfilename,"wsj_%d.png",i);
                    //status = MagickReadImage(wand, "wsj_%d.png");
	          }
                  fp = fopen(outfilename, "wb");
                  curl_easy_setopt(curl, CURLOPT_URL, buildURL(i+1,json_object_get_string(key_obj)));
                  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                  res = curl_easy_perform(curl);
                                   
                  /* clean up */
                  curl_easy_cleanup(curl);
                  fclose(fp);
	          //free(pageKeyName);
		  //free(json_object_get_string(key_obj));
               }
		///////////////////
            }
        }
    }

      json_object_put(parsed_json); // Crucial: Free memory
    }
    // Clean up
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);
      free(chunk.memory); // Free the dynamically allocated memory
    }
    curl_global_cleanup();
}
