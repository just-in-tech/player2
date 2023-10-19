#include <stdio.h>
#include <ulfius.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <systemd/sd-bus.h>
#include "actions.h"
#include "assets.h"
#include "dbus.h"
#include "json-gen.h"

#define PORT 8181
#define PREFIX_WEBSOCKET "/websocket"

typedef struct video_info curent_info_video_pointer;

//define threads
pthread_t thread;
pthread_t player_status;
pthread_t position_thread;
pthread_t ch4;
pthread_t ch6;
pthread_t ch9;
pthread_t ch10;


//define fuctions
int callback_websocket_echo (const struct _u_request * request, struct _u_response * response, void * user_data);


//define thread fuctions


//define variables
char ch4_action[4][6];
char ch6_action[6];
char ch9_action[9];
char ch10_action[10];
int ch4_counter=6;
bool ch4_run=0;
bool ch6_run=0;
bool ch9_run=0;
bool ch10_run=0;
bool quit=0;
bool new_info=0;
bool seek=0;
sd_bus* bus=NULL;
char web_browser[30]="chromium-browser";

void* thread_ch4(void* d){
int action_counter=0;
//bool keep_running=1;
//int timer_counter=0;
//timer_counter=0;
//ch4_run=1;
    //while(action_counter>=ch4_counter&&keep_running==1){
    if(strcmp("vol+",ch4_action[action_counter])==0){
        volumeplus();
    }else{
        if(strcmp("vol-",ch4_action[action_counter])==0){
            volumeminus();
        }else{
            if(strcmp("stop",ch4_action[action_counter])==0){
                action_stop();
            }else{
                if(strcmp("mute",ch4_action[action_counter])==0){

                }else{

                    write_to_log_que("no matching actions","4 char",0);
                }
            }
        }
    }
    /*if(action_counter==7&&ch4_counter<=6){
        action_counter=0;
    }
    if(action_counter<=ch4_counter){
    ++action_counter;
    }
    while(action_counter>ch4_counter){
        sleep(0.5);
        ++timer_counter;
        printf("inloop\n");
        if(timer_counter==11){
            keep_running=0;
            ch4_counter=6;
            goto end;
        }
    }
    timer_counter=0;


    }*/
    ch4_run=0;
    return 0;
}

void* thread_ch6(void* d){
    if(strcmp("reboot",ch6_action)==0){
        action_restart();
    }else{
        if(strcmp("skipad",ch6_action)==0){
            action_skipad();
        }else{
            if(strcmp("update",ch6_action)==0){
                printf("update\n");
            }else{
                write_to_log_que("no matching actions","6 char",0);

            }
        }
    }
    return 0;
}

void* thread_ch9(void* d){
    if(strcmp("playpause",ch9_action)==0){
        action_playpause(bus);
    }else{
        if(strcmp("closecapt",ch9_action)==0){
            action_subtitles();
        }else{
            if(strcmp("powerdown",ch9_action)==0){
                action_shutdown();
            }else{
                write_to_log_que("no matching actions","9 char",0);
            }
        }
    }
    return 0;
}

void* thread_ch10(void* d){
    if(strcmp("fullscreen",ch9_action)==0){
        action_fullscreen(bus);
    }else{
        write_to_log_que("no matching actions","10 char",0);
    }
    return 0;
}

int callback_notimevideo (const struct _u_request * request, struct _u_response * response, void * user_data) {
    ulfius_set_string_body_response(response, 200, "playing video");
    printf("you want to: %s\n",u_map_get(request->map_url, "url"));
    char command[200];
    int maxlen = sizeof command;
    char play_url[70];
    strcpy(play_url, u_map_get(request->map_url, "url"));
    snprintf(command,maxlen,"pkill %s",web_browser);
    system(command);
    snprintf(command,maxlen,"%s --autoplay-policy=no-user-gesture-required --kiosk --start-fullscreen --no-sandbox --disable-infobars %s &", web_browser, play_url);
    sleep(1);
    system(command);

return U_CALLBACK_CONTINUE;
}

void* hotspot(void* d){
    int x=0;
    while(x==0){

        x=system("ping -s1 -c1 google.com > /dev/null 2>&1");
        loop:sleep(360);

    }
    x=system("ping -s1 -c1 google.com > /dev/null 2>&1");
    if(x==0){

        sleep(360);
        goto loop;
    }
    system("systemctl stop hotspot.service &");
return 0;
}

int callback_hotspot (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, u_map_get(request->map_url, "offon"));
  pthread_t phone;
  phone=thread;
  printf("turn hotspot off or on");
  if(strcmp(u_map_get(request->map_url, "offon"),"on")==0){
        system("sudo systemctl start hotspot.service &");
        pthread_create( &phone, NULL, hotspot, NULL);
        }
  return U_CALLBACK_CONTINUE;
}

void websocket_onclose_callback (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data) {

  if (websocket_onclose_user_data != NULL) {
    write_to_log_que("websoket closed","websoket onclose",0);
    printf("websocket_onclose_user_data is %s\n", websocket_onclose_user_data);
    o_free(websocket_onclose_user_data);
  }
}

//runs all the time the websocket is active
void websocket_manager_callback(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data) {

bool first_time=1;
static char json[700];
char time_string[8];

    while(quit==0){
        usleep(795000);

        if(ulfius_websocket_status(websocket_manager)==U_WEBSOCKET_STATUS_CLOSE){
            goto end;
        }

        //print position to string
        sprintf(time_string,"%d:%02d:%02d",((curent_info_video_pointer*)websocket_manager_user_data)->position.hour,((curent_info_video_pointer*)websocket_manager_user_data)->position.minute,((curent_info_video_pointer*)websocket_manager_user_data)->position.second);

        //write json to be sent
        json_start(json);
        json_write_string(json,"position",time_string);
        json_write_int(json,"slider-position",((curent_info_video_pointer*)websocket_manager_user_data)->slider_position);
        if(new_info==1||first_time==1){
            //add all info to json
            write_to_log_que("sending full json payload","websocket manager",0);
            sprintf(time_string,"%d:%02d:%02d",((curent_info_video_pointer*)websocket_manager_user_data)->length.hour,((curent_info_video_pointer*)websocket_manager_user_data)->length.minute,((curent_info_video_pointer*)websocket_manager_user_data)->length.second);
            json_write_string(json,"length",time_string);
            json_write_string(json,"url",((curent_info_video_pointer*)websocket_manager_user_data)->url);
            json_write_string(json,"title",((curent_info_video_pointer*)websocket_manager_user_data)->title);
            json_write_string(json,"art-url",((curent_info_video_pointer*)websocket_manager_user_data)->art_url);
            first_time=0;

        }

        json_finish(json);
        int maxlen = strlen(json);
        if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, maxlen, json) != U_OK) {
            write_to_log_que("error sending json to client","websocket manager",0);
        }
    }
end: return ;
}

void websocket_message_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data) {
  //printf("Incoming message, rsv: 0x%02x, opcode: 0x%02x, mask: %d, len: %zu, text payload '%.*s'\n", last_message->rsv, last_message->opcode, last_message->has_mask, last_message->data_len, last_message->data_len, last_message->data);
  //char message[6];
  char first_ch[2];
  char temp_log[200];
  int maxlen;
  int num=last_message->data_len;

  pthread_t c4;
  pthread_t c6;
  pthread_t c9;
  pthread_t c10;
  c4=ch4;
  c6=ch6;
  c9=ch9;
  c10=ch10;

  snprintf(first_ch,2,"%s",last_message->data);
  if(strcmp("V",first_ch)==0){

    }else if(strcmp("P",first_ch)==0){
        char slider_position_array[6];
        char slider_position_parsed[5];
        int slider_position;

        seek=1;
        strcpy(slider_position_array,last_message->data);
        strcpy(slider_position_parsed,&slider_position_array[1]);
        // convert text to number
        slider_position=atoi(slider_position_parsed);
        set_video_position(websocket_incoming_message_user_data,slider_position, bus);
        return;
    }else{
        switch(num){
    case 4:
        maxlen=5;
        printf("there are 4 charaters in the string\n");
        ++ch4_counter;
        if(ch4_counter==7){
            ch4_counter=0;
        }
        snprintf(ch4_action[ch4_counter],maxlen,"%s",last_message->data);
        if(ch4_run==0){
            printf("no thread active starting\n");
            pthread_create( &c4, NULL, thread_ch4, NULL);
        }
        break;
    case 6:
        printf("there are 6 charaters in the string\n");
        if(ch6_run==0){
            maxlen=7;
            snprintf(ch6_action,maxlen,"%s",last_message->data);
            printf("no thread active starting\n");
            pthread_create( &c6, NULL, thread_ch6, NULL);
        }
        break;
    case 9:
        printf("there are 9 charaters in the string\n");
        if(ch9_run==0){
            maxlen=10;
            snprintf(ch9_action,maxlen,"%s",last_message->data);
            printf("no thread active starting");
            pthread_create( &c9, NULL, thread_ch9, NULL);
        }
        break;
    case 10:
        printf("there are 10 charaters in the string\n");
        if(ch10_run==0){
            maxlen=11;
            snprintf(ch9_action,maxlen,"%s",last_message->data);
            printf("no thread active starting\n");
            pthread_create( &c10, NULL, thread_ch10, NULL);
        }
        break;
    default:
        printf("not valid\n");
        sprintf(temp_log,"unvalid websoket message %s",last_message->data);
        write_to_log_que(temp_log,"websoket message callback",0);
        break;
  }
    }


  return ;
}

int callback_websocket_open (const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;

  //open websocket connection
  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_callback, user_data, &websocket_message_callback, user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    ulfius_add_websocket_deflate_extension(response);
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}

//gets new video infomation
void* get_new_video_info_thread(void * d){

int loop=0;
sd_bus_error err=SD_BUS_ERROR_NULL;
sd_bus_message *msg = NULL;
int error=0;
int long prev_len=0;
bool check_title=0;

printf("start looking for new info\n");
for(loop=0;loop<30&&new_info==0;loop++){
error=sd_bus_get_property(bus,
"org.mpris.MediaPlayer2.plasma-browser-integration",
"/org/mpris/MediaPlayer2",
"org.mpris.MediaPlayer2.Player",
"Metadata",
&err,&msg,"a{sv}");
if(error<0){
    printf("please make sure plasma boswer intergation is installed\n");

}
error = sd_bus_message_enter_container(msg, SD_BUS_TYPE_ARRAY, "{sv}");
        if (error < 0){
            write_to_log_que("error entering array to get new video info","get new video info",0);
            return error;
        }

        while ((error = sd_bus_message_enter_container(msg, SD_BUS_TYPE_DICT_ENTRY, "sv")) > 0) {
                const char *name;
                const char *contents;
                const char *ch;
                long int temp;


                error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_STRING, &name);
                if (error < 0){
                    write_to_log_que("error geting name of dbus property","get new video info",0);
                    return error;
                }

                error = sd_bus_message_peek_type(msg, NULL, &contents);
                if (error < 0){
                    write_to_log_que("error getting var of dbus proptery","get new video info",0);
                    return error;
                }

                error = sd_bus_message_enter_container(msg, SD_BUS_TYPE_VARIANT, contents);
                if (error < 0){
                    write_to_log_que("error enetering var container","get new video info",0);
                    return error;
                }

                    if(strcmp("mpris:artUrl",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_STRING, &ch);
                        if (error < 0){
                            write_to_log_que("unable to get art url","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);
                            if (error < 0)
                                write_to_log_que("unable to skip","get new video info",0);
                        }else{
                            snprintf(((curent_info_video_pointer*)d)->art_url,160,"%s",ch);
                        }

                        goto next;
                    }else if(strcmp("mpris:length",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_INT64, &temp);
                        if (error < 0){
                            write_to_log_que("unable get length","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);
                        }else{
                            ((curent_info_video_pointer*)d)->length=decode_time(temp);
                            ((curent_info_video_pointer*)d)->time_per_incorment=temp/1080;
                            if(prev_len!=temp){
                                if(loop!=0){
                                    check_title=1;
                                    write_to_log_que("got new length","get new video info",0);
                                }
                            }
                        }
                        goto next;

                    }else if(strcmp("mpris:trackid",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_OBJECT_PATH, &ch);
                        if(error < 0){
                            write_to_log_que("unable to get trackid","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);
                            if (error < 0)
                                write_to_log_que("unable to skip","get new video info",0);
                        }else{
                            sprintf(((curent_info_video_pointer*)d)->trackid,"%s",ch);
                        }
                        //current_video_info->=value;
                        goto next;
                    }else if(strcmp("xesam:title",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_STRING, &ch);
                        if(error < 0){
                            write_to_log_que("unable to get title","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);
                            if (error < 0)
                                write_to_log_que("unable to skip","get new video info",0);
                        }else{
                            if(check_title==1||loop>28){
                                if(strcmp(ch,((curent_info_video_pointer*)d)->title)==0){
                                    new_info=1;
                                    write_to_log_que("got new info","get new video info",0);
                                }
                            }
                            sprintf(((curent_info_video_pointer*)d)->title,"%s",ch);
                        }
                        goto next;
                    }else if(strcmp("xesam:url",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_STRING, &ch);
                        if(error < 0){
                            write_to_log_que("unable to get url","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);
                            if (error < 0)
                                write_to_log_que("unable to skip","get new video info",0);
                        }else{
                            sprintf(((curent_info_video_pointer*)d)->url,"%s",ch);
                        }
                        goto next;
                    }else if(strcmp("mpris:Position",name)==0){
                        error = sd_bus_message_read_basic(msg, SD_BUS_TYPE_INT64, &temp);
                        ((curent_info_video_pointer*)d)->position=decode_time(temp);
                        if(error < 0){
                            write_to_log_que("unable to get position","get new video info",0);
                            error = sd_bus_message_skip(msg, contents);

                            if (error < 0)
                                write_to_log_que("unable to skip","get new video info",0);
                        }else{
                            ((curent_info_video_pointer*)d)->position=decode_time(temp);
                        }
                        goto next;
                    }else{
                        error = sd_bus_message_skip(msg, contents);
                                if (error < 0){
                                    write_to_log_que("unable to skip unwanted info","get new video info",0);

                                }
                                 goto next;

                    }

                next:error = sd_bus_message_exit_container(msg);
                if (error < 0)
                    return NULL;

                error = sd_bus_message_exit_container(msg);
                if (error < 0)
                        return NULL;
        }
        if (error < 0)
            return NULL;
        if(new_info==1){
            goto new_info_timer;
        }
        usleep(1600000);
    }
    if(new_info==1){
        write_to_log_que("no matching actions","get new video info",0);
        new_info_timer:usleep(1000000);
        new_info=0;
        action_fullscreen(bus);
    }

  return U_CALLBACK_CONTINUE;
}

int get_video_position(struct video_info *video_info_current){
    static long long prev_pos;
    static long long prev_video_pos;
    static long long pos;
    sd_bus_message *msg = NULL;
    sd_bus_error err = SD_BUS_ERROR_NULL;
    int error=0;
    //get player postion
    error=sd_bus_get_property(bus,
    "org.mpris.MediaPlayer2.plasma-browser-integration",
    "/org/mpris/MediaPlayer2",
    "org.mpris.MediaPlayer2.Player",
    "Position",
    &err,&msg,"x");
    if(error<0){
        return 2;
    }
    sd_bus_message_read(msg,"x",&pos);
    if(error<0){

        return -1;
    // figure out if new video is playing
    }else if(pos<prev_pos && seek==0){
        prev_video_pos = prev_pos;
        prev_pos = pos;
        video_info_current->position=decode_time(pos);
        return 1;
    }
    prev_pos = pos;
    video_info_current->slider_position=pos/video_info_current->time_per_incorment;
    video_info_current->position=decode_time(pos);

    return 0;
}

int monitor_video_playback(struct video_info *current_video_info){

    pthread_t player;
    player=player_status;
    int error;
    bool check_all_info=0;
    bool first_time=1;
    bool no_video=1;

    while(1){
        if(check_all_info==1){
            //fetch all infomation

            seek=0;
            pthread_create( &player, NULL, get_new_video_info_thread,(void *) current_video_info);
            check_all_info=0;
        }

        usleep(180000);
        //returns 1 if it thinks there is a new video playing and 2 if it hink none is playing
        error=get_video_position(current_video_info);

        if(error < 0){
            write_to_log_que("unable read what dbus postion","montoir info",0);
        }else {
            if(error==1){
                //when set to one next time it all check if there is a new video and return the info of it
                check_all_info=1;
                no_video=1;

            }else if(error==0){
                if(no_video==1){
                    check_all_info=1;
                    printf("new video dected\n");
                    no_video=0;
                }
            }else if(error==2){
                static int loop=0;
                no_video_playing(current_video_info);
                if(new_info==1&&loop!=5){
                    printf("no video send\n");
                    loop++;
                    if(loop==5){
                        new_info=0;
                    }
                }
                seek=0;
                if(first_time==1){
                    printf("no video playing or plasma broswer intragtion is not installed\n");
                    write_to_log_que("waiting for first video","montoir info",0);
                    no_video=1;
                    first_time=0;
                }else if(no_video==0){
                    printf("please send video\n");
                    no_video_playing(current_video_info);
                    no_video=1;
                    new_info=1;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    struct _u_instance instance;
    char * cert_file = NULL, * key_file = NULL;
    sd_bus_error err= SD_BUS_ERROR_NULL;

    struct video_info current_video_info;

    //minmize terminal window
    sleep(5);
    system("xdotool windowminimize $\(xdotool getactivewindow)");

    printf("initialising\n");


    no_video_playing(&current_video_info);

    //initialise message que lock
    if (pthread_mutex_init(&lock_que, NULL) != 0) {
        printf("\n mutex init has failed\n");
        write_to_log_que("mutex init has failed","main",0);
        return -1;
    }
    start_logging();

    //connect to user dbus
    if(sd_bus_default_user(&bus)<=0){
        write_to_log_que("failed to connect to dbus exiting","main",1);
        return -1;

    }

    printf("connected to user bus\n");
    write_to_log_que("connected to user dbus","main",0);


  // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        write_to_log_que("failed to initialize ulfius exiting","main",1);
        printf("error starting network services");
        return(1);
    }

  // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", PREFIX_WEBSOCKET, "/controller", 0, &callback_websocket_open, &current_video_info);
    ulfius_add_endpoint_by_val(&instance, "GET", "/hotspot", NULL, 0, &callback_hotspot, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/notimevideo", NULL, 0, &callback_notimevideo, NULL);

// start with https if defined
    if (argc > 3 && 0 == o_strcmp(argv[1], "-https")) {
        key_file = read_file(argv[2]);
        cert_file = read_file(argv[3]);
        if (key_file == NULL || cert_file == NULL) {
            printf("https specifited but no certificate files are spesified\n");
            write_to_log_que("no https certificate files speseified","main",0);
        } else {

         if (ulfius_start_secure_framework(&instance, key_file, cert_file) == U_OK) {
    printf("framework started on port %d\n", instance.port);
    //loop to get video infomation
    monitor_video_playback(&current_video_info);
  } else {
  printf("error starting network services\n");
  write_to_log_que("error starting framework","main",1);

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
    pthread_mutex_destroy(&lock_que);
  return 0;
  }
      }
      o_free(key_file);
      o_free(cert_file);
    //if https not defined start in http mode
    } else {
        write_to_log_que("no https specseified starting in unsecure mode","main",0);
        printf("no certificate to start in secure mode\n");
       if (ulfius_start_framework(&instance) == U_OK) {
        printf("framework started on port %d\n", instance.port);
        //loop to get video infomation
        monitor_video_playback(&current_video_info);
  } else {
     printf("error starting network services");
     write_to_log_que("error starting ulfius framework" ,"main", 1);
    sd_bus_error_free(&err);
    sd_bus_unref(bus);
    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
}
    pthread_mutex_destroy(&lock_que);
  return 0;
  }

  return 0;
}
