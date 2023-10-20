#include "assets.h"
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define error_que 5

static char log_messages[5][200];

int writing=5;
int qued_messages=5;
int next_free_que_position=0;
pthread_mutex_t lock_que;

pthread_t write_log;

void* write_log_file_thread(void* d);

char * read_file(const char * filename) {
  char * buffer = NULL;
  long length;
  FILE * f;
  if (filename != NULL) {
    f = fopen (filename, "rb");
    if (f) {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc (length + 1);
      if (buffer) {
        fread (buffer, 1, length, f);
        buffer[length] = '\0';
      }
      fclose (f);
    }
    return buffer;
  } else {
    return NULL;
  }
}

int write_to_file(char *file_to_write,char *to_write){
    int error;
    FILE *fp;

    //open the file for writing
    fp=fopen("test","w");
    if(fp == NULL){
        printf("Unable to create file.\n");
        return -1;

    }

    //write to text file
    error=fprintf(fp, "%s\n",to_write);
    if(error<0){
        printf("error writing to file");
    }
    //closes the file
    error=fclose(fp);
    if(error<0){
        return-1;
    }

    return 0;
}

struct time decode_time(long int microseconds){
    struct time time_decoded;
    time_decoded.second=((microseconds/(1000000)) %60);
    time_decoded.minute=((microseconds/(60000000)) %60);
    time_decoded.hour=((microseconds/(3600000000)) %24);
    return time_decoded;
}

int start_logging(void){

    pthread_create( &write_log, NULL, write_log_file_thread,NULL);

    return 0;
}

int write_to_log_que(char *message, char *section,bool print_message){
    int que_position;


    if(print_message==1)
        printf("please check the log to see what went wrong and report it if it keeps happening and you have try to fix it\n");

    pthread_mutex_lock(&lock_que);
    que_position=next_free_que_position;
    if(next_free_que_position==5){
        next_free_que_position=0;

    }else{
    next_free_que_position++;

    }
    pthread_mutex_unlock(&lock_que);
    //get time

    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    //write to que
    sprintf(log_messages[que_position],"[%d/%d/%d %d:%d:%d] section: %s message: %s", time.tm_mday,  time.tm_mon + 1, time.tm_year + 1900, time.tm_hour, time.tm_min, time.tm_sec, section, message);
    printf("%d written to que\n",que_position);
    while(1){
        usleep(100);
        if(qued_messages+1==que_position){
            qued_messages++;
            return 0;
        }else if(que_position==0&&qued_messages==5){
            qued_messages=0;
            return 0;
        }
    }

}

void* write_log_file_thread(void* d){
    int returned_error;
    FILE *fp;
    char file[50];
    bool error=0;


    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    sprintf(file,"/home/pi/logs/%d-%d_%d:%d:%d", time.tm_mday,  time.tm_mon + 1, time.tm_hour, time.tm_min, time.tm_sec);
    while(1){

        if(qued_messages<writing){

        }else if(qued_messages==5&&writing==0){

        }else{
            printf("waiting message %d writting message = %d\n",qued_messages ,writing);

            //open the file for writing
            fp=fopen(file,"a");
            if(fp == NULL){
                printf("Unable to create %s.\n",file);
                goto restart_log_write;
            }

            printf("%s\n",log_messages[writing]);
            //write to text file

            returned_error=fprintf(fp, "%s\n",log_messages[writing]);
            if(returned_error<0){
                printf("error writing to file\n");
                goto restart_log_write;
            }

            returned_error=fclose(fp);
            if(returned_error<0){
                printf("error");
                goto restart_log_write;
            }
            if(error==0){

                if(writing==5){
                    writing=0;


                }else{
                    writing++;

                }
            }else{
                printf("error writing log retry\n");
                error=0;
            }
            restart_log_write:;
        }
    }

    usleep(100);

    //closes the file
    returned_error=fclose(fp);
    if(returned_error<0){
        return NULL;
    }
    return NULL;
}

int no_video_playing(struct video_info *current_video_info){
    sprintf(current_video_info->art_url,"none");
    sprintf(current_video_info->url,"none");
    sprintf(current_video_info->title,"no video playing");
    sprintf(current_video_info->trackid,"none");
    current_video_info->slider_position=0;
    current_video_info->time_per_incorment=0;
    current_video_info->length.hour=0;
    current_video_info->length.minute=0;
    current_video_info->length.second=0;
    current_video_info->position.hour=0;
    current_video_info->position.minute=0;
    current_video_info->position.second=0;
    return 0;
}
