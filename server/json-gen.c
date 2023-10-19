#include "json-gen.h"
#include <string.h>
#include <stdio.h>

int json_start(char *json){
    char start='{';
    snprintf(json,2,"%s",&start);
    return 0;
}
int json_write_key(char *json, char const* key){
    char temp[30];
    if(strlen(json)>1){
        strcat(json,",");
    }
    sprintf(temp,"\"%s\":",key);
    strcat(json,temp);
    return 0;
}
int json_write_int64(char *json,char const* key,long long number){
    char temp[15];

    json_write_key(json,key);
    sprintf(temp,"%lld",number);
    strcat(json,temp);
    return 0;
}
int json_write_string(char *json,char const* key,char *contents){
    char temp[200];
    json_write_key(json,key);
    sprintf(temp,"\"%s\"",contents);
    strcat(json,temp);
    return 0;
}
int json_write_int(char *json, char const* key, int number){
    char temp[7];
    json_write_key(json,key);
    sprintf(temp,"%d",number);
    strcat(json,temp);
    return 0;
}
int json_finish(char *json){
    char end[3];
    snprintf(end,2,"}");
    strcat(json,end);
    return 0;
}
