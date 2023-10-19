#include "actions.h"
#include "assets.h"
#include <systemd/sd-bus.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

char log_temp[200];

typedef struct video_info curent_info_video_pointer;

int action_stop (void) {
    // this dose not need to change if operating system uses different name for chromium
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
    sprintf(log_temp,"error method call %s",err.message);
    write_to_log_que(log_temp,"action play pause",0);
    return 1;
}
return 0;
}

int action_skipad (void) {
    // made for 1080p screens
    system("xdotool mousemove 1870 980 click 1");
    //system("xdotool mousemove 1920 500");
    return 0;
}

int action_fullscreen (sd_bus *bus) {
    sd_bus_error err = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = NULL;
    bool fullscreen;
    int loop=0;
    int error;



    error=sd_bus_get_property(bus, "org.mpris.MediaPlayer2.plasma-browser-integration", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2", "Fullscreen", &err, &msg, "b");
    if(error<0){
        sprintf(log_temp,"error getting fullscreen status: %s\n",err.message);
        write_to_log_que(log_temp,"action fullscreen",0);
        err = SD_BUS_ERROR_NULL;
    }else {
        error=sd_bus_message_read_basic(msg, 'b', &fullscreen);
        if(error<0){
            write_to_log_que("error reading fullscreen status","action fullscreen",0);
        }else if(fullscreen==1){
            write_to_log_que("video is all ready fullscreen","action fullscreen",0);
            return 0;
        }
    }
    fullscreen:while(fullscreen==0&&loop<=20){
    loop++;
    usleep(1000000);
    system("xdotool key f");
    usleep(3000000);

            error=sd_bus_get_property(bus, "org.mpris.MediaPlayer2.plasma-browser-integration", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2", "Fullscreen", &err, &msg, "b");
if(error<0){

        error=0;
        sprintf(log_temp,"get property error: %s\n",err.message);
        write_to_log_que(log_temp,"action fullscreen",0);
        err = SD_BUS_ERROR_NULL;
        }else{
            error=sd_bus_message_read_basic(msg, 'b', &fullscreen);
            if(error<0){
                    error=0;
                    write_to_log_que("fullscreen get property error\n","action fullscreen",0);
                }else{
                    if(fullscreen==1){
            write_to_log_que("is fullscreen with f","action fullscreen",0);
        }else if(fullscreen==0){
            write_to_log_que("not fullscreen with f","action fullscreen",0);
            error=sd_bus_set_property(bus, "org.mpris.MediaPlayer2.plasma-browser-integration" ,"/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2","Fullscreen", &err, "b", "1");
            if(error<0){
                    error=0;
                    //not compltete
                    //sprintf("fullscreen set property error %s\n",err.message);
                    write_to_log_que(log_temp,"action fullscreen",0);
                    err = SD_BUS_ERROR_NULL;
                }
            usleep(3000000);

            sd_bus_get_property(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2",
"Fullscreen",
&err,&msg,"b");
            sd_bus_message_read_basic(msg, SD_BUS_TYPE_BOOLEAN, &fullscreen);


            if(fullscreen==1){
                write_to_log_que("is fullscreen property","action fullscreen",0);
            }else if(fullscreen==0){
                write_to_log_que("not fullscreen property","action fullscreen",0);

            }
        }
                }
            }

    }

    usleep(3000000);
                sd_bus_get_property(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2",
"Fullscreen",
&err,&msg,"b");
            sd_bus_message_read_basic(msg, SD_BUS_TYPE_BOOLEAN, &fullscreen);


            if(fullscreen==1){
                write_to_log_que("is fullscreen check","action fullscreen",0);
                return 0;
            }else if(fullscreen==0){
                if(loop==20){
                    write_to_log_que("failed to make video fullscreen check","action fullscreen",0);
                    return -1;
                }else{
                    write_to_log_que("not fullscreen check","action fullscreen",0);
                    goto fullscreen;
                }
            }
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


  sd_bus_call_method(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2.Player",
"SetPosition",
&err,&msg,"ox",((curent_info_video_pointer*)current_video_info)->trackid,new_video_time);
         if(err._need_free!=0){
            sprintf(log_temp,"error set postions %s trackid: %s",err.message ,((curent_info_video_pointer*)current_video_info)->trackid);
            write_to_log_que(log_temp,"action set position",0);
            return 1;
         }
  return 0;
}
