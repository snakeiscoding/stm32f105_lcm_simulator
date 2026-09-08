#ifndef __ERRORCODE_H__
#define __ERRORCODE_H__



//#############################################################################
//
//#############################################################################

#define	  _CLASS_A_                     0
#define	  _CLASS_B_		        1
#define	  _CLASS_C_		        2 
#define   _NoErrorCode_                 3


// Class A
// 0x02
#define	  _LowBattery_				0x0201			


// Class B
// 0x00
#define	  _EncoderError_                        0x02A0
#define	  _UnderSpeed_				0x0041
#define	  _OverSpeed_				0x0042
// 0x01
#define	  _InclineActionError_			0x0140
#define	  _MotorOverTempture_			0x0141
#define	  _MotorSyncError_			0x0142
// 0x02
#define	  _LCBError_				0x0247
#define	  _BatteryError_			0x0248
// 0x03
// 0x04
#define	  _ReceiveTimeout_			0x0440
#define	  _NoFunction_				0x0441
#define	  _DigitalError_			0x04A0




// Class C
// 0x00
#define   _InclineCabError_			0x00A0		
// 0x01
#define	  _InclineVRError_			0x01A0
#define	  _InclineCab1Error_			0x01A1
#define	  _InclineVRDirError_			0x01A2

#define	  _InclineOverCurrent_			0x01A7
#define	  _EM_OC_				0x01AC
#define	  _EM_NC_				0x01AF
// 0x02

#define	  _MachineTypeError_			0x02AB
#define	  _ResistanceTypeError_			0x02B4


//
#define	  _ESTOP_Error_				0x024A
#define	  _FanError_				0x0249
#define	  _InPosError_				0x024B
#define	  _DCBrakeError_			0x02BE
#define	  _DCBrakeOC_				0x02BF
#define	  _DCBrakeManual_	        	0x02C0
#define	  _SpeedTrackingError_			0x02C1

//===========================================================================
// End of file.
//===========================================================================
#endif  /* __ERRORCODE_H__*/

