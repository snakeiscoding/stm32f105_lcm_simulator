#ifndef _VA_DEFINES_H_
#define _VA_DEFINES_H_

// ==============================================
// VA communication result
// ==============================================
typedef enum tagVACommResult
{
	VACR_SUCCESS = 0,
	VACR_READ_TIMEOUT,
	VACR_READ_ERROR,
	VACR_INVALID_CHECKSUM,	
	VACR_INVALID_SIZE,
	VACR_GENERIC_FAILURE,
	VACR_DEVICE_NOT_OPEN,
	VACR_OUT_OF_MEMORY
} VACommResult;

// ==============================================
// VA language
// ==============================================
typedef enum tagVALanguage
{
	VAL_Default = 0,
	VAL_English,
	VAL_German,
    VAL_French,
    VAL_Italian,
    VAL_Spanish,
    VAL_Dutch,
    VAL_Portuguese,
    VAL_Chinese_Simplified,
    VAL_Chinese_Traditional,
    VAL_Japanese
} VALanguage;

// ==============================================
// VA state
// ==============================================
typedef enum tagVAState
{
	VAS_Stop,
	VAS_Run,
	VAS_Pause,
} VAState;

// ==============================================
// VA video speed type
// ==============================================
typedef enum tagVAVideoSpeedType
{
	VAVST_SpeedMph,
	VAVST_RPM,
} VAVideoSpeedType;

// ==============================================
// VA frame value
// ==============================================
typedef struct tagVAFrameValue
{
	int Frame;
	int Value;
} VAFrameValue;

// ==============================================
// VA event subscription
// ==============================================
#define SUB_INCLINE 0x00000001
#define SUB_RESISTANCE 0x00000002
#define SUB_MESSAGE 0x00000004
#define SUB_SFX 0x00000008
#define SUB_VIDEO_NAME 0x00000010
#define SUB_SFX_NAME 0x00000020

// ==============================================
// Messages
// ==============================================
#define MESSAGE_VIRTUAL_ACTIVE WM_USER + 1111

#define VA_INCLINE_CHANGE 1
#define VA_MESSAGE 2
#define VA_START 3
#define VA_STOP 4
#define VA_RESISTANCE_CHANGE 5
#define VA_START_COOLDOWN 6
#define VA_ERROR 7
#define VA_VIDEO_NAME 8
#define VA_SFX_NAME 9

// ==============================================
// Defines
// ==============================================
#define VA_COOLDOWN_SECONDS 120
#define VA_TOTAL_LEVELS 20

#define VA_MIN_VIDEO_PLAYBACK_SPEED_PERCENT 50
#define VA_MAX_VIDEO_PLAYBACK_SPEED_PERCENT 200
#define VA_DEFAULT_MIN_VIDEO_PLAYNACK_SPEED 66
#define VA_DEFAULT_MAX_VIDEO_PLAYNACK_SPEED 133
#define VA_DEFAULT_START_SPEED 400
#define VA_DEFAULT_STOP_SPEED 800
#define VA_DEFAULT_START_RPM 40
#define VA_DEFAULT_STOP_RPM 120

// ==============================================
// Profile type
// ==============================================
typedef enum tagProfileType
{
	PT_Incline,
	PT_Resistance
} ProfileType;

// ==============================================
// Parameter definition for direct calls to the video board
// ==============================================
typedef struct tagSendParams
{
	long Parameter1;
	long Parameter2;
	char* pData;
	long DataLength;
	int SendResult;
} SendParams;

typedef struct tagReceiveParams
{
	long Command;
	long Result;
	long Parameter1;
	char* pData;
	long DataLength;
	int ReceiveResult;
} ReceiveParams;

// ==============================================
// Video board commands
// ==============================================
#define CMD_RESET                           1
#define CMD_GET_MAX_BUFFER_SIZE             2
#define CMD_CHANGE_DIRECTORY                10
#define CMD_GO_UP_DIRECTORY                 11
#define CMD_DELETE_DIRECTORY                12
#define CMD_CREATE_DIRECTORY                13
#define CMD_GET_DIRECTORY_LISTING           14
#define CMD_CREATE_FILE                     20
#define CMD_CLOSE_FILE                      21
#define CMD_WRITE_FILE                      22
#define CMD_READ_FILE                       23
#define CMD_SET_FILE_INDEX                  24
#define CMD_DELETE_FILE                     25
#define CMD_GET_FILE_SIZE                   26
#define CMD_GET_FILE_STORE_TOTAL_SIZE       40
#define CMD_GET_FILE_STORE_AVAILABLE_SIZE   41
#define CMD_GET_DIRECTORY_DELETE_STATUS     60
#define CMD_FORMAT_STORAGE                  70
#define CMD_GET_STORAGE_FORMATTING_STATUS   71
#define CMD_GET_STORAGE_STATE               72
#define CMD_PLAY_VIDEO                      100
#define CMD_PAUSE_VIDEO                     101
#define CMD_RESUME_VIDEO                    102
#define CMD_STOP_VIDEO                      103
#define CMD_GET_VIDEO_STATE                 104
#define CMD_GET_VIDEO_PLAYBACK_TIME         110
#define CMD_GET_VIDEO_PLAYBACK_FRAME        111
#define CMD_SET_VIDEO_PLAYBACK_RATE         120
#define CMD_MUTE_PLAYBACK_AUDIO             200
#define CMD_PLAY_AUDIO                      201
#define CMD_SET_AUDIO_MIX_LEVEL             202
#define CMD_STOP_AUDIO                      203
#define CMD_GET_AUDIO_STATE                 204
#define CMD_SET_USB_SWITCH                  500
#define CMD_GET_USB_SWITCH                  501
#define CMD_COPY_USB_DIRECTORY              510
#define CMD_GET_USB_COPY_STATUS             511
#define CMD_GET_VERSION                     600
#define CMD_UPDATE_FIRMWARE                 800
#define CMD_GET_FIRMWARE_UPDATE_STATUS      801

// ==============================================
// Video board results
// ==============================================
#define RSLT_SUCCESS                              0
#define RSLT_GENERIC_FAIL                         1
#define RSLT_FILE_NOT_FOUND                       2
#define RSLT_OUT_OF_MEMORY                        3
#define RSLT_ALREADY_AT_ROOT_DIRECTORY            4
#define RSLT_DIRECTORY_NOT_FOUND                  5
#define RSLT_INVALID_FILE_HANDLE                  6
#define RSLT_INVALID_CHECKSUM                     7
#define RSLT_INVALID_COMMAND                      8
#define RSLT_INVALID_USB_DEVICE_REMOVED           9
#define RSLT_INVALID_INVALID_VIDEO_PLAYBACK_RATE  10

#endif