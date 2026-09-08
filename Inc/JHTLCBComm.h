#ifndef __JHTLCBCOMM_H__
#define __JHTLCBCOMM_H__



//------------------------------------------------------------------------------
#define _RelaseESTOP70    0x70
#define _RelaseESTOPF7    0xF7
#define _RelaseESTOP      0xAA



typedef union {
  struct {
    unsigned short EB0140:1 ; // 0 Incline motor operation fail 
    unsigned short EB0144:1 ; // 1 Motor over loading
    unsigned short EB0146:1 ; // 2 Motor the current exception  
    unsigned short EB014A:1 ; // 3 The battery charge over-current or short-circuit side.    
    unsigned short EB0247:1 ; // 4 LCB Fail    
    unsigned short EB0248:1 ; // 5 Battery disconnection or fail
    unsigned short EB0440:1 ; // 6 Timeout receive packet
    unsigned short EB0441:1 ; // 7 Correct packet but LCB without the function
    unsigned short EB0442:1 ; // 8 the received command code from the console is correct and is supported, but it has less or more data arguments
    unsigned short EC01AC:1 ; // 9 Resistance Over Current
    unsigned short EC01AF:1 ; // 10 Resistance or solenoid circuit.
    unsigned short EC01B4:1 ; // 11 Battery connector reverse    
    unsigned short EC02AB:1 ; // 12 Machine Type Error
    unsigned short EC02B2:1 ; // 13   
    unsigned short EC02B4:1 ; // 14 Resistance Type Error    
    unsigned short EC04A0:1 ; // 15 UCB Communiaction Disconnection
    //
  } bit ;
  unsigned short Full ;
} ErrorStatusDataStruct ;

//Add 20250731 No user
typedef union {
  struct {
    unsigned short NoUser:1 ; // Treadmill No User Control 0: User , 1: No User
    unsigned short F1:1 ; // 
    unsigned short F2:1 ; //   
    unsigned short F3:1 ; //     
    unsigned short F4:1 ; //    
    unsigned short F5:1 ; //  
    unsigned short F6:1 ; // 
    unsigned short F7:1 ; // 
    unsigned short F8:1 ; //     
    unsigned short F9:1 ; // 
    unsigned short F10:1 ; // 
    unsigned short F11:1 ; // 
    unsigned short F12:1 ; // 
    unsigned short F13:1 ; //     
    unsigned short F14:1 ; // 
    unsigned short F15:1 ; // 
    //
  } bit ;
  unsigned short Full ;
} SpecControlDataStruct ;


typedef union {
  struct {
    unsigned char DnInclineStatus:1 ;	  // 0	0:Stop,1:Down Action		
    unsigned char UpInclineStatus:1 ;     // 1 	0:Stop,1:Up Action		
    unsigned char MainMotorStatus:1 ;	  // 2	0:Stop,1:Action
    unsigned char ACPluginStatus:1 ;	  // 3	0:Stop,1:Action
    unsigned char BatteryLowStatus:1 ;	  // 4	0:Normal,1:Low Battery
    unsigned char CommandErrorStatus:1 ;  // 5	0:Normal,1:Error
    unsigned char McbErrorStatus:1 ;	  // 6	0:Normal,1:Error
    unsigned char InitialStatus:1 ;       // 7	0:Final Initial,1:Initial
  } bit ;
  unsigned char Full ;
} LCBStatusDataStruct ;

typedef union {
  struct {
    unsigned short MCU_A_Loader ;
    unsigned short MCU_A_API ;
    unsigned short MCU_B_Loader ;
    unsigned short MCU_B_API ;
  } Ver ;
  unsigned char VerData[8] ;
} LCBVersionInfo ;

                                 
                                                  
                                                  

typedef union {
  struct{
    //
    unsigned long TxAction:1 ;
    unsigned long RxEndWait5msReturn:1 ;
    unsigned long RxTimeoutCheckStart:1 ;
    unsigned long DisconnectionTimeoutCheckStart:1 ;
    unsigned long StatusLED:1 ;
    unsigned long DirectorRXD:1 ;
    //
    unsigned long DisableErPOnCmd:1 ;
    unsigned long EntryErPMode:1 ;
    unsigned long CheckErPCommandReturn:1 ;
    unsigned long UCBOnline:1 ;
    unsigned long UCBOffline:1 ;
    unsigned long DelayToSendErrorCode:1 ;
    // Incline
		unsigned long InclineAction:1 ;
    unsigned long InclineTurnRun:1 ;
    unsigned long InclineDirectorInvert:1 ;
    unsigned long InclineAutoCalibration:1 ;
    // Climbmill    
    unsigned long ClimbmillStart:1 ;              
    unsigned long ClimbmillStop:1 ;
    unsigned long ClimbmillUnlockSafetyKey ;
    // TM Motor
    unsigned long MotorAction:1 ;
    unsigned long MotorTurnRun:1 ;	
    //    
    unsigned long RIS_LCBMode:1 ;
    unsigned long RIS_MasterMode:1 ;
    unsigned long Rev:10 ;
  } Bits ;
  unsigned long Full ;
} CommControl ;


typedef union {
  struct {
    unsigned char Start ;
    unsigned char Status ;
    unsigned char Command ;
    unsigned char Length ;
    unsigned char Data[256] ;
  } member ;
  unsigned char Buffer[260] ;
} TxDataStruct ;

typedef union {
  struct {
    unsigned char Start ;
    unsigned char Address ;
    unsigned char Command ;
    unsigned char Length ;
    unsigned char Data[256] ;
  } member ;
  unsigned char Buffer[260] ;
} RxDataStruct ;



// for 2017 RIS LCB

typedef union{
    struct {
      unsigned short  StartWord ;
      unsigned char   Status ;
      unsigned char   SeqNumber ;
      unsigned char   MsgID ;
      unsigned char   DateLength ;  
      unsigned char   Data[256] ;
    } member ;
    unsigned char Buffer[262] ;
  } RISPacketFileType ;

typedef union {
  struct {
    unsigned char CTSStatus:1 ;	          // 0	0:Stop,1:Down Action		
    unsigned char LCBError:1 ;            // 1 	0:Normal,1:Error
    unsigned char HeartRateStatus:1 ;	  // 2	0:No Heart Rate Signal Dertect,1:Heart Rate Signal Dertect
    unsigned char BatteryLowStatus:1 ;	  // 3	0:Normal,1:Low Battery
    unsigned char ACPluginStatus:1 ;	  // 4	AC/DC plug-in status, 0:Self-powered , 1:AC/DC plug-in
    unsigned char Calibration:1 ;         // 5	Calibration 0:stop, 1:start
    unsigned char Source:2 ;	          // 6,7 Source, 0x00 == from LED Console (CXM),0x01 == from main LCB,0x02 == tbd,0x03 == from GUI console (Phoenix)
  } bit ;
  unsigned char Full ;
} RISLCBStatusDataStruct ;


//20210602 ITC LCB
typedef union {
  struct {
    unsigned char Quarter_rep:1 ;	  // 0	it has not been computed and tallied yet	
    unsigned char Home:1 ;                // 1 	weight stack is below a preset height	
    unsigned char E_Lift:1 ;	          // 2	cleared when rep is completed or if distance of burst is too short
    unsigned char Moveing:1 ;	          // 3	motion speed above 4.5 mm/s
    unsigned char Helf_rep:1 ;	          // 4	has reached top of a rep or full extension E-lift measure complete
    unsigned char CommandErrorStatus:1 ;  // 5	0:Normal,1:Error
    unsigned char McbErrorStatus:1 ;	  // 6	0:Normal,1:Error
    unsigned char InitialStatus:1 ;       // 7	0:Final Initial,1:Initial
  } bit ;
  unsigned char Full ;
} ITCLCBStatusDataStruct ;



extern LCBVersionInfo  LCBVersion ;
extern LCBStatusDataStruct   LCBStatus ;
extern volatile CommControl  CommControlFlag ;
extern ErrorStatusDataStruct ErrorCodeStatus ;
extern SpecControlDataStruct SpecControlStatus ; //Add 20250731 No user
extern RISLCBStatusDataStruct    RIS_LCBStatus ;
extern unsigned char TMReleaseEstopStatus ; // Add by Kunlung 20180402
//------------------------------------------------------------------------------
//20210602 ITC
extern ITCLCBStatusDataStruct  ITCStatus ;



//------------------------------------------------------------------------------
/* Private function prototypes -----------------------------------------------*/
void JHTLCBComm_Initial(void) ;
void JHTLCBComm_HW_Initial(void) ;
void JHTLCBComm_TxRxInterrupt(void) ;
void JHTLCBComm_RxProcess(void) ;
void JHTLCBComm_LCBRxProcess(void) ;
void JHTLCBComm_Timeout(void) ;
void JHTLCBComm_ResetTimeoutStatus(void) ;
void JHTLCBComm_ErrorMessageProcess(void) ;
void JHTLCBComm_ClearAllErrorMessage( unsigned char ClearB ) ;
unsigned char JHTLCBComm_GetUCBOfflineStatus(void)  ;
unsigned char JHTLCBComm_GetUCBOnlineStatus(void)  ;
void JHTLCBComm_SaveErrorCode( unsigned short ErrType , unsigned short ErrCode ) ;
unsigned short JHTLCBComm_GetErrorCode(void) ;
void JHTLCBComm_SkipErrorCode(void) ;
//void JHTLCBComm_SkipErrorCode04A0(void) ;
void JHTLCBComm_SkipAnyErrorCode(unsigned short ECode) ;
// for 2017 RIS LCB
void JHTLCBComm_RIS_HW_Initial(void) ;
void JHTLCBComm_RIS_RxProcess(void) ;
void JHTLCBComm_SystemTime(void) ;
unsigned short JHTLCBComm_CalculatorTimeTick( unsigned short last_TimeTick ) ;

unsigned char DebugOut( void ) ;
#endif /* __JHTLCBCOMM_H__*/


