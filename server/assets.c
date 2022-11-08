#include "assets.h"

#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>


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

int write_to_file(char to_write){
            /*system("touch url.txt");
            //open the file for writing
            fp=fopen("url.txt","w");
            //write url into text file
            fprintf(fp, "%s\n", u_map_get(request->map_url, "url"));
            //closes the file
            fclose(fp);*/
    return 0;
}

struct time decode_time(long int microseconds){
    struct time time_decoded;
    time_decoded.second=((microseconds/(1000000)) %60);
    time_decoded.minute=((microseconds/(60000000)) %60);
    time_decoded.hour=((microseconds/(3600000000)) %24);
    return time_decoded;
}

int Loging(char message,int return_value){

    return 0;
}
