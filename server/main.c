#include <stdio.h>
#include <stdlib.h>
#include <ulfius.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib-2.0/gio/gio.h>
#include <glib-2.0/glib.h>
#include <math.h>

#define PORT 8181
#define PREFIX_WEBSOCKET "/websocket"

//gio dbus connection
GDBusConnection *conn;
GMainLoop *loop;
GDBusError dbus_error;

void print_dbus_error (char *str);

//define threads
pthread_t thread;
pthread_t thread1;
pthread_t player_status;
pthread_t position_thread;
pthread_t ch4;
pthread_t ch6;
pthread_t ch9;
pthread_t ch10;

int callback_websocket_echo (const struct _u_request * request, struct _u_response * response, void * user_data);

const char *const INTERFACE_NAME = "org.mpris.MediaPlayer2.Player";
const char *const CLIENT_BUS_NAME = "player2.video-control";
const char *const SERVER_OBJECT_PATH_NAME = "/org/mpris/MediaPlayer2";
const char *const CLIENT_OBJECT_PATH_NAME = "/in/crago/playrer2.0";
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
char name[15];
char url[150];
char title[160];
char new_title[160];
char art[160];
char postion[30];
char length[30];
char objectpath[50];
char temp[4][160];
long long pos;
long long num;
long long timePerIncroment;
long hour;
long minute;
long second;
int sliderpos;
/*
for furture restructure
struct time{
    long second;
    long minute;
    long hour;
};

struct video_info{
    int title;
    int position;
    int length
    int url;
    char art_url;
};
*/

static char * read_file(const char * filename) {
  char * buffer = NULL;
  long length;
  FILE * f;
  if (filename != NULL) {
    f = fopen (filename, "rb");
    if (f) {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = o_malloc (length + 1);
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

static int set_dbus_property(const char *prop, GVariant *value){
    GVariant *result;
    GError *error = NULL;

    result = g_dbus_connection_call_sync(conn,"org.mpris.MediaPlayer2.plasma-browser-integration", "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Set", g_variant_new("(ssv)", "org.mpris.MediaPlayer2", prop, value), NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    if(error != NULL)
        return 1;

    g_variant_unref(result);
    return 0;

}



static struct video_info dbus_property_values(const gchar *key, GVariant *value){
	const gchar *type = g_variant_get_type_string(value);
    int len;
    int loopwrite=0;
    char prop8[7];
    char prop9[8];
    char prop11[10];
    char prop12[11];

	switch(*type) {
		case 's':
			sprintf(temp[loopwrite],"%s", g_variant_get_string(value, NULL));
			//printf("%s\n", g_variant_get_string(value, NULL));
			break;
		case 'b':
			sprintf(temp[loopwrite],"%d", g_variant_get_boolean(value));
			//printf("%d\n", g_variant_get_boolean(value));
			break;
		case 'u':
            sprintf(temp[loopwrite],"%d", g_variant_get_uint32(value));
            //printf("%d\n", g_variant_get_uint32(value));
			break;
        case 'a':
			printf("");
			const gchar *uuid;
			GVariantIter *v;
			GVariantIter i;
			g_variant_iter_init(&i, value);
			while(g_variant_iter_next(&i, "{sv}", &uuid, &v)){
				dbus_property_values(uuid, v);
            }
			break;
        case 'x':

            sprintf(temp[loopwrite],"%ld", g_variant_get_int64(value));
            num=g_variant_get_int64(value);
            break;
        case 'o':
            sprintf(objectpath,"%s", g_variant_get_string(value,NULL));

            break;
		default:
			//g_print("Other %s\n",type);
			break;
	}
	len=9;
	snprintf(prop8,len,"%s", key);
	if(strcmp("Position",prop8)==0&&loopwrite==0){
        second=((num/(1000000)) %60);
        minute=((num/(60000000)) %60);
        hour=((num/(3600000000)) %24);
        sprintf(postion,"\"position\": \"%li:%02li:%02li\"", hour, minute, second);
        pos=num;
	}
	len=10;
	snprintf(prop9,len,"%s", key);
	if(strcmp("xesam:url",prop9)==0){
        sprintf(url,"\"url\":\"%s\"",temp[loopwrite]);
        loopwrite=4;
	}
	len=12;
	snprintf(prop11,len,"%s", key);
	sprintf(new_title,"\"title\":\"%s\"", temp[loopwrite]);
	if(strcmp("xesam:title",prop11)==0){
        if(strcmp(new_title,title)!=0){
          new_info=1;
          printf("new info");
          sprintf(title,"\"title\":\"%s\"", temp[loopwrite]);

        }

        loopwrite=3;
	}
	len=13;
    snprintf(prop12,len,"%s", key);
	if(strcmp("mpris:artUrl",prop12)==0){
        sprintf(art,"\"art-url\":\"%s\"", temp[loopwrite]);
        loopwrite=1;
	}
	if(strcmp("mpris:length",prop12)==0){
        second=((num/(1000000)) %60);
        minute=((num/(60000000)) %60);
        hour=((num/(3600000000)) %24);
        sprintf(length,"\"length\": \"%li:%02li:%02li\"", hour, minute, second);
        timePerIncroment=num/1080;
        loopwrite=2;
	}
}
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

int action_playpause (void) {


    GVariant *result;
    GError *error = NULL;

    const char *const METHOD_NAME = "PlayPause";

    result = g_dbus_connection_call_sync(conn,"org.mpris.MediaPlayer2.playerctld", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", METHOD_NAME, NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    if(error != NULL)
        return 1;

    g_variant_unref(result);
    return 0;


  return 0;
}


int action_skipad (void) {

    system("xdotool mousemove 1870 980 click 1");
    system("xdotool mousemove 1920 500");
    return 0;
}

int action_fullscreen (void) {

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
                printf("stop\n");
                action_stop();
            }else{
                if(strcmp("mute",ch4_action[action_counter])==0){

                }else{

                    //printf("%d\n",action_counter);
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
    end:ch4_run=0;
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
                printf("no match\n");

            }
        }
    }
    return 0;
}

void* thread_ch9(void* d){
    if(strcmp("playpause",ch9_action)==0){
        action_playpause();
    }else{
        if(strcmp("closecapt",ch9_action)==0){
            action_subtitles();
        }else{
            if(strcmp("powerdown",ch9_action)==0){
                action_shutdown();
            }else{
                printf("no match\n");
            }
        }
    }
    return 0;
}

void* thread_ch10(void* d){
    if(strcmp("fullscreen",ch9_action)==0){
        action_fullscreen();
            }
    return 0;
}

void*thread_position(void* d){
  int64_t newPosition;
  newPosition=timePerIncroment*sliderpos;
  printf("%d\n",newPosition);
          GVariant *result;
    GError *error = NULL;

    const char *const METHOD_NAME = "SetPosition";

    result = g_dbus_connection_call_sync(conn,"org.mpris.MediaPlayer2.playerctld", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", METHOD_NAME, g_variant_new("(ox)", objectpath,newPosition), NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    if(error != NULL)
        return 1;

    g_variant_unref(result);
    return 0;
  return 0;
}

int callback_notimevideo (const struct _u_request * request, struct _u_response * response, void * user_data) {
    ulfius_set_string_body_response(response, 200, "playing video");
    printf("you want to: %s\n",u_map_get(request->map_url, "url"));
    char command[200];
    int maxlen = sizeof command;
    char play_url[70];
    strcpy(play_url, u_map_get(request->map_url, "url"));
    snprintf(command,maxlen,"chromium-browser --kiosk --start-fullscreen --no-sandbox --disable-infobars %s &", play_url);
    system("pkill chromium");
    sleep(1);
    system(command);
    sleep(15);
    system("xdotool key f");

return U_CALLBACK_CONTINUE;
}


void* hotspot(void* d){
    int x=0;
    while(x==0){
        x=system("ping -s1 -c1 google.com > /dev/null 2>&1");
        loop:sleep(360);
        printf("looping");
    }
    x=system("ping -s1 -c1 google.com > /dev/null 2>&1");
    if(x==0){
        printf("may not");
        sleep(360);
        goto loop;
    }
    printf("stop");
    system("systemctl stop hotspot.service &");
return 0;
}

int callback_hotspot (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, u_map_get(request->map_url, "offon"));
  pthread_t phone;
  phone=thread1;
  printf("turn hotspot off or on");
  if(strcmp(u_map_get(request->map_url, "offon"),"on")==0){
        system("sudo systemctl start hotspot.service &");
        pthread_create( &phone, NULL, hotspot, NULL);
        }
  return U_CALLBACK_CONTINUE;
}

static void mpris_getall_property(GDBusConnection *con,GAsyncResult *res, gpointer data){
	GVariant *result = NULL;
	GMainLoop *loop = NULL;

	loop = (GMainLoop *)data;

	result = g_dbus_connection_call_finish(con, res, NULL);
	if(result == NULL)
		g_print("Unable to get video info if a video is play make sure plasma intergration addon is installed\n");

	if(result) {
		result = g_variant_get_child_value(result, 0);
		const gchar *property_name;
		GVariantIter i;
		GVariant *prop_val;
		g_variant_iter_init(&i, result);
		while(g_variant_iter_next(&i, "{&sv}", &property_name, &prop_val)){
			dbus_property_values(property_name, prop_val);
        }
		g_variant_unref(prop_val);
	}
}

void websocket_onclose_callback (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data) {
  if (websocket_onclose_user_data != NULL) {
    printf("websocket_onclose_user_data is %s\n", websocket_onclose_user_data);
    o_free(websocket_onclose_user_data);
  }
}
//runs all the time the websocket is active
void websocket_manager_callback(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data) {

char json[700];
//int leng;
//json = (char *) malloc(400);

int sliderPosition=0;
char sliderPosition_json[22];

  //if (websocket_manager_user_data != NULL) {
   // printf("websocket_manager_user_data is %s", websocket_manager_user_data);
  //}
  sleep(1);
  sliderPosition=pos/timePerIncroment;
  sprintf(sliderPosition_json,"\"sliderposition\":%d",sliderPosition);
     // leng = strlen(postion);
      //leng = leng + strlen(length);
      //leng = leng + strlen(url);
      //leng = leng + strlen(art);
      //leng = leng + strlen(title);
      //leng = leng + 6;
      //json = (char *) realloc(json, leng);
      sprintf(json,"{%s,%s,%s,%s,%s}", length, postion, url, title, art);
      int maxlen = strlen(json);
      if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, maxlen, json) != U_OK) {
    printf("Error ulfius_websocket_send_message\n");
  }
  while(quit==0){
  usleep(795000);

  if(ulfius_websocket_status(websocket_manager)==U_WEBSOCKET_STATUS_CLOSE){
  goto end;
  }
    if(new_info==1){
      printf("new has being sent");
      sliderPosition=pos/timePerIncroment;
      sprintf(sliderPosition_json,"\"sliderposition\":%d",sliderPosition);
      //leng = strlen(postion);
      //leng = leng + strlen(length);
      //leng = leng + strlen(url);
      //leng = leng + strlen(art);
      //leng = leng + strlen(title);
      //leng = leng + 6;
      //json = (char *) realloc(json, leng);
      sprintf(json,"{%s,%s,%s,%s,%s,%s}", length, postion, sliderPosition_json, url, title, art);
    }else{
    //leng =strlen(postion);
    //leng=leng+2;
    //json = (char *) realloc(json, leng);
    sliderPosition=pos/timePerIncroment;
    sprintf(sliderPosition_json,"\"sliderposition\":%d",sliderPosition);
    sprintf(json,"{%s,%s}", postion,sliderPosition_json);
    }
    maxlen = strlen(json);
    if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, maxlen, json) != U_OK) {
    printf("Error ulfius_websocket_send_message\n");
  }
 }
end: return U_CALLBACK_CONTINUE;
}

void websocket_echo_message_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data) {
  printf("Incoming message, rsv: 0x%02x, opcode: 0x%02x, mask: %d, len: %zu, text payload '%.*s'\n", last_message->rsv, last_message->opcode, last_message->has_mask, last_message->data_len, last_message->data_len, last_message->data);
  //char message[6];
  char first_ch[2];
  int maxlen;
  int num=last_message->data_len;
  char sliderpostion[6];
  char sliderpostionParsed[5];

  pthread_t c4;
  pthread_t c6;
  pthread_t c9;
  pthread_t c10;
  pthread_t pos_thread;
  pos_thread=position_thread;
  c4=ch4;
  c6=ch6;
  c9=ch9;
  c10=ch10;

  snprintf(first_ch,2,"%s",last_message->data);
  if(strcmp("V",first_ch)==0){

  }else{
    if(strcmp("P",first_ch)==0){

        strcpy(sliderpostion,last_message->data);
        strcpy(sliderpostionParsed,&sliderpostion[1]);
        sliderpos=atoi(sliderpostionParsed);
        printf("%d",sliderpos);
        pthread_create( &pos_thread, NULL, thread_position, NULL);
        goto end;
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
        break;
  }
    }
  }
  printf("%d\n", num);

  //hh
  end:return U_CALLBACK_CONTINUE;
}

int callback_websocket_echo (const struct _u_request * request, struct _u_response * response, void * user_data) {
  char * websocket_user_data = o_strdup("my_user_data");
  int ret;
  if ((ret = ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_callback, websocket_user_data, &websocket_echo_message_callback, websocket_user_data, &websocket_onclose_callback, websocket_user_data)) == U_OK) {
    ulfius_add_websocket_deflate_extension(response);
    return U_CALLBACK_CONTINUE;
  } else {
    return U_CALLBACK_ERROR;
  }
}

//gets video infomation to display on web app/site
void* thread_info(void* d){
int counter=0;
while(1){
  usleep(200000);

  new_info=0;
g_dbus_connection_call(conn,
				"org.mpris.MediaPlayer2.plasma-browser-integration",
				"/org/mpris/MediaPlayer2",
				"org.freedesktop.DBus.Properties",
				"GetAll",
				g_variant_new("(s)", "org.mpris.MediaPlayer2.Player"),
				G_VARIANT_TYPE("(a{sv})"),
				G_DBUS_CALL_FLAGS_NONE,
				-1,
				NULL,
				(GAsyncReadyCallback)mpris_getall_property,
				loop);

}
  return U_CALLBACK_CONTINUE;
}

int main(int argc, char *argv[]) {
  struct _u_instance instance;
  pthread_t player;
  player=player_status;
  char * cert_file = NULL, * key_file = NULL;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", PREFIX_WEBSOCKET, "/echo", 0, &callback_websocket_echo, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/hotspot", NULL, 0, &callback_hotspot, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/notimevideo", NULL, 0, &callback_notimevideo, NULL);
    if(strcmp(argv[1],"start")==0){
GMainLoop *loop;

	conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, NULL);
	if(conn == NULL) {
		g_print("Not able to get connection to system bus\n");
		return 1;
	}
if (argc > 3 && 0 == o_strcmp(argv[2], "-https")) {
      key_file = read_file(argv[3]);
      cert_file = read_file(argv[4]);
      if (key_file == NULL || cert_file == NULL) {
        printf("Error reading https certificate files\n");

      } else {

         if (ulfius_start_secure_framework(&instance, key_file, cert_file) == U_OK) {
    printf("Start framework on port %d\n", instance.port);
    sleep(3);
    system("xdotool windowminimize $\(xdotool getactivewindow\)");
  pthread_create( &player, NULL, thread_info, NULL);

    printf("sarted\n");


    loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(loop);
  } else {
    fprintf(stderr, "Error starting framework\n");
     printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
  }
      }
      o_free(key_file);
      o_free(cert_file);
    } else {
       if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);
    sleep(5);
    system("xdotool mousemove 300 200 click 1");
    system("xdotool windowminimize $\(xdotool getactivewindow\)");
  pthread_create( &player, NULL, thread_player, NULL);
    // Wait for the user to press <enter> on the console to quit the application

    printf("sarted\n");


    loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(loop);
  } else {
    fprintf(stderr, "Error starting framework\n");
     printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
  }
    }


  // Start the framework

  }else{
  printf("not right");
  return 1;
  }
  if(strcmp(argv[1],"stop")==0){
  printf("stop framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
  }
  return 0;
}
