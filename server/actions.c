#include "actions.h"
#include "assets.h"
#include <systemd/sd-bus.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct video_info curent_info_video_pointer;

int action_stop (void) {
    system("pkill chromium");
    return 0;
}

int action_shutdown (void) {
    system("shutdown now");
    return 0;
}

int action_subtitles (void) {
    system("xdotool key c");
    return 0;
}

int action_restart (void) {
    system("reboot");
    return 0;
}

int action_playpause (sd_bus *bus) {

sd_bus_error err= SD_BUS_ERROR_NULL;
sd_bus_message *msg= NULL;

sd_bus_call_method(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2.Player",
"PlayPause",&err,&msg,NULL);
if(err._need_free==0){
    return 1;
}
return 0;
}

int action_skipad (void) {

    system("xdotool mousemove 1870 980 click 1");
    system("xdotool mousemove 1920 500");
    return 0;
}

int action_fullscreen (sd_bus *bus) {
    sd_bus_error err = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = NULL;
    bool fullscreen;
    int loop=0;
    int error;

    printf("fullscreen\n");
    while(fullscreen==0&&loop<=20){
    loop++;
    usleep(1000000);
    system("xdotool key f");
    if(error<0)
        printf("%s",err.name);

    usleep(2500000);

            error=sd_bus_get_property(bus, "org.mpris.MediaPlayer2.plasma-browser-integration", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2", "Fullscreen", &err, &msg, "b");
    if(error<0)
        printf("%s\n",err.message);
            error=sd_bus_message_read_basic(msg, 'b', &fullscreen);
            if(error<0)
                printf("%s\n",err.message);
    printf("%d\n",fullscreen);
    if(err._need_free!=0){
        printf("returned error: %s\n",err.message);
    }else{
        if(fullscreen==1){
            printf("is fullscreen\n");
        }else if(fullscreen==0){
            printf("not fullscreen\n");
            printf("%d\n",fullscreen);
            error=sd_bus_set_property(bus, "org.mpris.MediaPlayer2.plasma-browser-integration" ,"/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2","Fullscreen", &err, "b", "1");
            usleep(2500000);

            sd_bus_get_property(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2",
"Fullscreen",
&err,&msg,"b");
            sd_bus_message_read_basic(msg, SD_BUS_TYPE_BOOLEAN, &fullscreen);


            if(fullscreen==1){
                printf("is fullscreen\n");
            }else if(fullscreen==0){
                printf("not fullscreen\n");
                printf("%d\n",fullscreen);
            }
        }
    }
    }
    printf("exit full %d\n",fullscreen);
  return 0;
}

int volumeplus(void){
    system("pactl set-sink-volume @DEFAULT_SINK@ +5%");

return 0;
}

int volumeminus(void){
    system("pactl set-sink-volume @DEFAULT_SINK@ -5%");
return 0;
}

int set_video_position(void *current_video_info, int new_slider_position, sd_bus *bus){
  int64_t new_video_time;

  new_video_time=((curent_info_video_pointer*)current_video_info)->time_per_incorment*new_slider_position;
  sd_bus_error err= SD_BUS_ERROR_NULL;
sd_bus_message *msg= NULL;
printf("seg\n");
printf("trackid: %s\n",((curent_info_video_pointer*)current_video_info)->title);

  sd_bus_call_method(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2.Player",
"SetPosition",
&err,&msg,"ox",((curent_info_video_pointer*)current_video_info)->trackid,new_video_time);
         if(err._need_free!=0){
            printf("error %s %s",err.name,err.message);
            return 1;
         }
  return 0;
}
