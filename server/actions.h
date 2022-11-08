#ifndef ACTIONS_H_INCLUDED
#define ACTIONS_H_INCLUDED

#include <systemd/sd-bus.h>
#include "assets.h"

int action_stop (void);
int action_shutdown (void);
int action_subtitles (void);
int action_restart (void);
int action_playpause (sd_bus *bus);
int action_skipad (void);
int action_fullscreen (sd_bus *bus);
int volumeplus(void);
int volumeminus(void);
int set_video_position(void *current_video_info, int new_slider_position, sd_bus *bus);


#endif // ACTIONS_H_INCLUDED
