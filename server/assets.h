#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED
#include <pthread.h>
#include <stdbool.h>



extern pthread_mutex_t lock_que;

struct time{
    int second;
    int minute;
    int hour;
};

struct video_info{
    struct time position;
    struct time length;
    int slider_position;
    long long time_per_incorment;
    char title[160];
    char url[160];
    char art_url[160];
    char trackid[100];
};

int no_video_playing(struct video_info *current_video_info);
char * read_file(const char * filename);
int write_to_file(char *file_to_write,char *to_write);
struct time decode_time(long int miroseconds);
int start_logging(void);
int write_to_log_que(char *message,char *return_value,bool print_message);

#endif // ASSETS_H_INCLUDED
