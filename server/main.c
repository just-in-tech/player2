#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <systemd/sd-bus.h>
#include <cjson/cJSON.h> 


#define PORT 8181
#define PREFIX_WEBSOCKET "/websocket"




// main funcation/ main thread
int main(){
    load_config();


}

// second thread


//read configuration file
int load_config(){
    // open the file 
    FILE *fp = fopen("data.json", "r"); 
    if (fp == NULL) { 
        printf("Error: Unable to open the file.\n"); 
        return 1; 
    } 
  
    // read the file contents into a string 
    char buffer[1024]; 
    int len = fread(buffer, 1, sizeof(buffer), fp); 
    fclose(fp); 
  
    // parse the JSON data 
    cJSON *json = cJSON_Parse(buffer); 
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        return 1; 
    } 
  
    // access the JSON data 
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name"); 
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        printf("Name: %s\n", name->valuestring); 
    } 
  
    // delete the JSON object 
    cJSON_Delete(json); 
    return 0; 
}

//open dbus connection
int open_dbus_connection(){

}

//init websocket


