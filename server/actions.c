#include "actions.h"
#include "assets.h"
#include <systemd/sd-bus.h>
#include <stdlib.h>

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
    system("xdotool key s");
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

    //int err = NULL;
    system("xdotool key f");
    //err=set_dbus_property("Fullscreen",g_variant_new("b",TRUE));
    //if(err != NULL){
      //  printf("error");
      //}

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
