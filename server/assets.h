#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

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

char * read_file(const char * filename);
int write_to_file(char to_write);
struct time decode_time(long int miroseconds);
int Loging(char message,int return_value);

#endif // ASSETS_H_INCLUDED
