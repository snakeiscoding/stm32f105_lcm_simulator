#include "stm32f10x.h"
#include "VAComm.h"
#include "va_device.h"
#include "VADefines.h"
#include "VAPacket.h"

#define CHECK_SUCCESS ((_LastResult == VACR_SUCCESS) && (VAD_GetResponseResult() == RSLT_SUCCESS))


long _LastResult;
long _MaxBufferSize;
long _Version;	

// ==============================================
// Get version
// ==============================================
int VAComm_GetVersion()
{
	if(_Version > 0)
		return _Version;

  VAD_SetCommand(CMD_GET_VERSION,0,0,0,NULL) ;
  _LastResult = VAD_SendPacket() ;
	if(!CHECK_SUCCESS)
      {
      return 0;
      }

	_Version = VAD_GetResponseParameter1();
	return _Version;
}

// ==============================================
// Get max buffer size
// ==============================================
int VAComm_GetMaxBufferSize()
{
	if(_MaxBufferSize > 0)
		return _MaxBufferSize;

  VAD_SetCommand(CMD_GET_MAX_BUFFER_SIZE,0,0,0,NULL) ;
  _LastResult = VAD_SendPacket() ;
  
	if(!CHECK_SUCCESS)
      {
      return 0;
      }

	_MaxBufferSize = VAD_GetResponseParameter1();
	return _MaxBufferSize;
}


// ==============================================
// Reset
// ==============================================
BOOL VAComm_Reset()
{

  VAD_SetCommand(CMD_RESET,0,0,0,NULL) ;
  _LastResult = VAD_SendPacket() ;
  
	if(!CHECK_SUCCESS)
      {
      return 0 ;		
      }

	return CHECK_SUCCESS;
}

// ****************************************************************************
// FILE TRANSFER COMMANDS
// ****************************************************************************

// ==============================================
// Get directory listing
// ==============================================
CVAStringMap VAComm_GetDirectoryListing(BOOL GetDirectories)
{
	CVAStringMap Result;

	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_DIRECTORY_LISTING);
	Packet.SendPacket.SetParameter1(GetDirectories ? PARAM_GET_DIRECTORY_NAMES : PARAM_GET_FILE_NAMES);
	_LastResult = _VADevice.SendPacket(Packet);
	if(CHECK_SUCCESS)
	{
		CVAString Listings = Packet.ReceivePacket.GetString();
		CVAString Temp;
		int TokenIndex = 0;
		BOOL Continue = TRUE;
		while(Continue)
		{
			Temp = Listings.GetTokenString(';', TokenIndex);
			if(Temp.GetLength() > 0)
			{
				Result.Add(TokenIndex, Temp);
			}
			else
			{
				Continue = FALSE;
			}
		}
	}

	if(!CHECK_SUCCESS)
	{
		CJHTNATrace::LogSoftwareError(FUNC_NAME,
			L"Failed to get %s! CommError=0x%08X, PacketError=0x%08X",
			GetDirectories ? L"Directories" : L"Files",
			_LastResult,
			Packet.ReceivePacket.GetResult());
	}

	return Result;
}

// ==============================================
// Change directory
// ==============================================
BOOL VAComm_ChangeDirectory(LPCTSTR pDirectory)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_CHANGE_DIRECTORY);
	Packet.SendPacket.SetString(pDirectory);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
      {
      }

	return CHECK_SUCCESS;
}

// ==============================================
// Create directory
// ==============================================
BOOL VAComm_CreateDirectory(LPCTSTR pDirectory)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_CREATE_DIRECTORY);
	Packet.SendPacket.SetString(pDirectory);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Delete directory
// ==============================================
BOOL VAComm_DeleteDirectory(LPCTSTR pDirectory)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_DELETE_DIRECTORY);
	Packet.SendPacket.SetString(pDirectory);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Go up directory
// ==============================================
BOOL VAComm_GoUpDirectory()
{
  VAD_SetCommand(CMD_GO_UP_DIRECTORY,0,0,0,NULL) ;
  _LastResult = VAD_SendPacket() ;
	if(!CHECK_SUCCESS)
	{
		// Only record if error other than already at root
		if(VAD_GetResponseResult() != RSLT_ALREADY_AT_ROOT_DIRECTORY)
		{
		}
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Create file
// ==============================================
int VAComm_CreateFile(LPCTSTR pFileName, int Parameter)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_CREATE_FILE);
	Packet.SendPacket.SetParameter1(Parameter);
	Packet.SendPacket.SetString(pFileName);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		CJHTNATrace::LogSoftwareError(FUNC_NAME,
			L"Failed to create file [%s]! CommError=0x%08X, PacketError=0x%08X",
			pFileName,
			_LastResult,
			Packet.ReceivePacket.GetResult());
		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Close file
// ==============================================
BOOL VAComm_CloseFile(int FileHandle)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_CLOSE_FILE);
	Packet.SendPacket.SetParameter1(FileHandle);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		CJHTNATrace::LogSoftwareError(FUNC_NAME,
			L"Failed to close file! CommError=0x%08X, PacketError=0x%08X",
			_LastResult,
			Packet.ReceivePacket.GetResult());
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get file size
// ==============================================
__int64 VAComm_GetFileSize(LPCTSTR pFileName)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_FILE_SIZE);
	Packet.SendPacket.SetString(pFileName);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		CJHTNATrace::LogSoftwareError(FUNC_NAME,
			L"Failed to get file size! CommError=0x%08X, PacketError=0x%08X",
			_LastResult,
			Packet.ReceivePacket.GetResult());
		return 0;
	}

	return Packet.ReceivePacket.GetInt64();
}

// ==============================================
// Read file
// ==============================================
int VAComm_ReadFile(int FileHandle, LPBYTE pBuffer, int BytesToRead)
{
	int BytesRead = 0;
	CVAPacket Packet;
	LPBYTE pData;
	int DataSize;
	int ReadSize;
	BOOL Continue = TRUE;

	if(_MaxBufferSize == 0)
	{
		CJHTNATrace::LogSoftwareError(FUNC_NAME,
			L"Can't read file because _MaxBufferSize is zero!");
		return 0;
	}

	_VADevice.EnableRetry(FALSE);

	// We need to respect the video board max buffer size
	// and send requests in chunks.
	while(Continue)
	{
		Packet.SendPacket.Reset();
		Packet.ReceivePacket.Reset();

		ReadSize = BytesToRead - BytesRead;
		if(ReadSize > _MaxBufferSize)
			ReadSize = _MaxBufferSize;
		
		Packet.SendPacket.SetCommand(CMD_READ_FILE);
		Packet.SendPacket.SetParameter1(FileHandle);
		Packet.SendPacket.SetParameter2(ReadSize);
		_LastResult = _VADevice.SendPacket(Packet);

		if(!CHECK_SUCCESS)
		{
			CJHTNATrace::LogSoftwareError(FUNC_NAME,
				L"Failed to read file! CommError=0x%08X, PacketError=0x%08X",
				_LastResult,
				Packet.ReceivePacket.GetResult());
			_VADevice.EnableRetry(TRUE);
			return BytesRead;
		}

		pData = Packet.ReceivePacket.GetData(DataSize);
		if(pData && DataSize)
		{
			memcpy(pBuffer + BytesRead, pData, DataSize);
			BytesRead += DataSize;
		}
		else
		{
			Continue = FALSE;
		}

		if(BytesRead >= BytesToRead)
			Continue = FALSE;
	}

	_VADevice.EnableRetry(TRUE);

	return BytesRead;
}

// ==============================================
// Write file
// ==============================================
int VAComm_WriteFile(int FileHandle, LPBYTE pBuffer, int BytesToWrite)
{
	int BytesWritten = 0;
	CVAPacket Packet;
	int Writes;
	int WriteBytes;
	int WriteSize;

#ifdef PAD_PACKETS
	WriteSize = 44;
#else
	WriteSize = _MaxBufferSize;
#endif

	if(WriteSize == 0)
	{
		return 0;
	}

	// Calculate amount of writes
	Writes = BytesToWrite / WriteSize;
	if((BytesToWrite % WriteSize) != 0)
		Writes++;

	// We need to respect the video board max buffer size
	// and send requests in chunks.
	for(int i=0; i<Writes; i++)
	{
		Packet.SendPacket.Reset();
		Packet.ReceivePacket.Reset();
		
		Packet.SendPacket.SetCommand(CMD_WRITE_FILE);
		Packet.SendPacket.SetParameter1(FileHandle);

		if(i == (Writes - 1))
			WriteBytes = BytesToWrite - BytesWritten;
		else
			WriteBytes = WriteSize;
		Packet.SendPacket.SetData(pBuffer + BytesWritten, WriteBytes);
		
		_LastResult = _VADevice.SendPacket(Packet);

		if(!CHECK_SUCCESS)
		{
			break;
		}

		BytesWritten += WriteBytes;
	}

	return BytesWritten;
}

// ==============================================
// Delete file
// ==============================================
BOOL VAComm_DeleteFile(LPCTSTR pFileName)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_DELETE_FILE);
	Packet.SendPacket.SetString(pFileName);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get file store total size
// ==============================================
__int64 VAComm_GetFileStoreTotalSize()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_FILE_STORE_TOTAL_SIZE);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	
		return 0;
	}

	return Packet.ReceivePacket.GetInt64();
}

// ==============================================
// Get file store available size
// ==============================================
__int64 VAComm_GetFileStoreAvailableSize()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_FILE_STORE_AVAILABLE_SIZE);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

		return 0;
	}

	return Packet.ReceivePacket.GetInt64();
}

// ==============================================
// Get directory delete status
// ==============================================
int VAComm_GetDirectoryDeleteStatus()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_DIRECTORY_DELETE_STATUS);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Format the storage card
// ==============================================
BOOL VAComm_FormatStorage()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_FORMAT_STORAGE);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get the status of the format
// ==============================================
int VAComm_GetStorageFormattingStatus()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_STORAGE_FORMATTING_STATUS);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Get file store state
// ==============================================
int VAComm_GetStorageState()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_STORAGE_STATE);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	
		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ****************************************************************************
// VIDEO CONTROL COMMANDS
// ****************************************************************************

// ==============================================
// Play
// ==============================================
BOOL VAComm_PlayVideo(LPCTSTR pFileName)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_PLAY_VIDEO);
	Packet.SendPacket.SetString(pFileName);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

	}

	return CHECK_SUCCESS;
}

// ==============================================
// Pause
// ==============================================
BOOL VAComm_PauseVideo()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_PAUSE_VIDEO);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

	}

	return CHECK_SUCCESS;
}

// ==============================================
// Resume
// ==============================================
BOOL VAComm_ResumeVideo()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_RESUME_VIDEO);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Stop
// ==============================================
BOOL VAComm_StopVideo()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_STOP_VIDEO);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get video state
// ==============================================
int VAComm_GetVideoState()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_VIDEO_STATE);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		return 0;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Get video playback time
// ==============================================
int VAComm_GetVideoPlaybackTime()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_VIDEO_PLAYBACK_TIME);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

		return 0;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Get video playback frame
// ==============================================
int VAComm_GetVideoPlaybackFrame()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_VIDEO_PLAYBACK_FRAME);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

		return 0;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Set video playtback rate
// ==============================================
BOOL VAComm_SetVideoPlaybackRate(int Percent)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_SET_VIDEO_PLAYBACK_RATE);
	Packet.SendPacket.SetParameter1(Percent);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

	}

	return CHECK_SUCCESS;
}

// ****************************************************************************
// AUDIO CONTROL COMMANDS
// ****************************************************************************

// ==============================================
// Play audio
// ==============================================
BOOL VAComm_PlayAudio(LPCTSTR pFileName, int AudioSource)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_PLAY_AUDIO);
	Packet.SendPacket.SetString(pFileName);
	Packet.SendPacket.SetParameter1(AudioSource);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

	}

	return CHECK_SUCCESS;
}

// ==============================================
// Stop
// ==============================================
BOOL VAComm_StopAudio(int AudioSource)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_STOP_AUDIO);
	Packet.SendPacket.SetParameter1(AudioSource);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get the audio state
// ==============================================
int VAComm_GetAudioState(int AudioSource)
{
	int AudioState = 0;
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_AUDIO_STATE);
	Packet.SendPacket.SetParameter1(AudioSource);
	_LastResult = _VADevice.SendPacket(Packet);

	// Version 8 or greater
	if(_Version >= 8)
	{
		if(!CHECK_SUCCESS)
		{
			return 0;
		}

		AudioState = Packet.ReceivePacket.GetParameter1();
	}
	else
	{
		AudioState = Packet.ReceivePacket.GetResult();
	}

	return AudioState;
}

// ==============================================
// Mute playback audio
// ==============================================
BOOL VAComm_MutePlaybackAudio(BOOL Mute)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_MUTE_PLAYBACK_AUDIO);
	Packet.SendPacket.SetParameter1(Mute ? 1 : 2);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{

	}

	return CHECK_SUCCESS;
}

// ****************************************************************************
// USB CONTROL COMMANDS
// ****************************************************************************

// ==============================================
// Set USB switch direction
// ==============================================
BOOL VAComm_SetUSBSwitch(BOOL IsConsole)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_SET_USB_SWITCH);
	Packet.SendPacket.SetParameter1(IsConsole ? PARAM_USB_TRAFFIC_CONSOLE : PARAM_USB_TRAFFIC_VIDEO);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get USB switch direction
// ==============================================
int VAComm_GetUSBSwitch()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_USB_SWITCH);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ==============================================
// Copy USB directory from USB drive to SD card
// ==============================================
BOOL VAComm_CopyUSBDirectory(LPCTSTR pDirectory)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_COPY_USB_DIRECTORY);
	Packet.SendPacket.SetString(pDirectory);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get USB copy status
// ==============================================
int VAComm_GetUSBCopyStatus()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_USB_COPY_STATUS);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
		return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}

// ****************************************************************************
// FIRMWARE UPDATE COMMANDS
// ****************************************************************************

// ==============================================
// Update firmware
// ==============================================
BOOL VAComm_UpdateFirmware(LPCTSTR pFileName)
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_UPDATE_FIRMWARE);
	Packet.SendPacket.SetString(pFileName);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	}

	return CHECK_SUCCESS;
}

// ==============================================
// Get firmware update status
// ==============================================
int VAComm_GetFirmwareUpdateStatus()
{
	CVAPacket Packet;
	Packet.SendPacket.SetCommand(CMD_GET_FIRMWARE_UPDATE_STATUS);
	_LastResult = _VADevice.SendPacket(Packet);

	if(!CHECK_SUCCESS)
	{
	return -1;
	}

	return Packet.ReceivePacket.GetParameter1();
}