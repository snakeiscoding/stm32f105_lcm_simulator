#ifndef _VA_PACKET_H_
#define _VA_PACKET_H_


typedef struct tagCommandPacketHeader
{
	long Command;
	long Parameter1;
	long Parameter2;
	long DataLength;
} CommandPacketHeader;

typedef struct tagResponsePacketHeader
{
	long Command;
	long Result;
	long Parameter1;
	long DataLength;
} ResponsePacketHeader;


typedef enum tagReceivePacketStatus
{
	RPS_Complete,
	RPS_NotComplete,
	RPS_InvalidChecksum,
	RPS_InvalidSize,
} ReceivePacketStatus;






#endif