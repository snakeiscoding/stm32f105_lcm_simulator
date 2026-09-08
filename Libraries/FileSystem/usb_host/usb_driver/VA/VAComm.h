#ifndef _VA_COMM_H_
#define _VA_COMM_H_


//#include "VAStringMap.h"
//#include <vector>

#define MUSIC_DIRECTORY_NAME                      "music"

#define PARAM_GET_DIRECTORY_NAMES                 0
#define PARAM_GET_FILE_NAMES                      1
#define PARAM_CREATE_NEW_FILE                     0
#define PARAM_OPEN_EXISTING_FILE                  1
#define PARAM_CREATE_NEW_FILE_OVERWRITE           2
#define PARAM_FILE_NAME                           0
#define PARAM_FILE_PATH                           1
#define PARAM_PLAY                                1
#define PARAM_PAUSE                               2
#define PARAM_STOP                                3
#define PARAM_AUDIO_SOURCE_1                      1
#define PARAM_AUDIO_SOURCE_2                      2
#define PARAM_NO_FADE                             0
#define PARAM_FADE_IN                             1
#define PARAM_FADE_OUT                            1
#define PARAM_USB_TRAFFIC_CONSOLE                 1
#define PARAM_USB_TRAFFIC_VIDEO                   2
#define PARAM_COPY_STATUS_COPYING                 1
#define PARAM_COPY_STATUS_COMPLETED               2
#define PARAM_COPY_STATUS_ERROR                   3
#define PARAM_FORMAT_STATUS_FORMATTING            1
#define PARAM_FORMAT_STATUS_COMPLETED             2
#define PARAM_FORMAT_STATUS_ERROR                 3
#define PARAM_DIRECTORY_DELETE_STATUS_DELETING    1
#define PARAM_DIRECTORY_DELETE_STATUS_COMPLETED   2
#define PARAM_DIRECTORY_DELETE_STATUS_ERROR       3
#define PARAM_STORAGE_STATE_OK                    0
#define PARAM_STORAGE_STATE_NEEDS_FORMATTING      1

#define RESULT_AUDIO_STATE_PLAY                   1
#define RESULT_AUDIO_STATE_PAUSE                  2
#define RESULT_AUDIO_STATE_STOP                   3

#define RESULT_VIDEO_STATE_PLAY                   1
#define RESULT_VIDEO_STATE_PAUSE                  2
#define RESULT_VIDEO_STATE_STOP                   3

#endif

