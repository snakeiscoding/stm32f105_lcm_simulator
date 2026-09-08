#ifndef __LCBSIMULATOR_H__
#define __LCBSIMULATOR_H__



#define   _ErrorListSize          13
#define   _RunMode                0
#define   _ErPActionMode          1
#define   _SelectLCBType          2
#define   _RISBatteryOFF          3
#define   _WaitButtonFree         255

#define   _AthenaMaxSize          200

#define   _LCB1_          0x07
#define   _LCB3_          0x08
#define   _LCB2_          0x0A
#define   _Climbmill_     0x0B
#define   _LCBA_          0x13
#define   _LCB1x_         0x14  //JIS 1x
#define   _ClimbmillEN_   0x17
#define   _LCBA_M_        0x18
#define   _ClimbmillR_    0x19  //JIS Retil Climbmill C50
#define   _Athena         0x1B  //Atherna Indoor Cycle
#define   _JISICR70LCB    0x1D  //JIS Indoor Cycle 70 series.
#define   _IMPULSE_LCB    0x1E  // C700E, Climbmill LCB

#define   _ATLCB2_2026    0x1F  // 20260126
#define   _CMLCB_2026     0x20  // 20260126
#define   _BKLCBA_2026    0x21  // 20260126




#define   _DCI_           0xC0  //DCI daughter board
#define   _TopTek_        0xC1  //TopTek              T70/75, TF70/75
#define   _Delta_I        0xC2  //DELTA               T70/75, TF70/75
#define   _Delta_H        0xC3  //DELTA AC 1.1KW      T70/75, TF70/75
#define   _Delta_I1       0xC4  //DELTA Hybrid
#define   _Delta_I2       0xC5  //DELTA Hybrid for VAVE T5x MCB
#define   _Delta_I3       0xC6  //DELTA P.M.S.M 2.2kW(I), for Large TM
#define   _Liteon         0xC7  //Liteon
#define   _Delta_I4       0xC8  //DELTA AC 1.5KW(H)
 
#define   _PS_PMSM        0xC9  // 20260126  
      
#define   _LiteonJHT      0xF0  //Liteon MCB manufacture by Johnson
//20210322
#define   _VAVE_A1        0xA1  // For T-PS/T-ES
#define   _VAVE_A2        0xA2  // For T600/T600E
#define   _VAVE_A3        0xA3  // For T-LS
#define   _VAVE_F1        0xF1  // For T-PP VAVE

#define   _PP_PMSM        0xF2  // 20260126

//JIS
#define   _JISECB_I       0xFF  // A30 --> 0x0B
#define   _JISEM          0x0C  // U/R/SE50I
#define   _JISDCTM        0x0E  // T30/50 TF30/50
#define   _JISECB         0x12  // U/R/SE30
#define   _JISEM_I        0x15  // A50I
#define   _JISACTM110     0xE0  // T90
#define   _JISACTM220     0xE1  // T90
#define   _JISGMCB110     0xD0  // Global DC MCB
#define   _JISGMCB220     0xD1  // Global DC MCB

#define   _Rower          0x00
#define   _IndoorCycle    0x01
#define   _SDrive         0x02  // Rxp&SxP
//20210413 Add UBCM & UBCP
#define   _UBCM           0x03  // Upper Body Cycle M
#define   _UBCP           0x04  // Upper Body Cycle Phoenix
//20251215
#define     _Universal      0x05
#define     _GTM            0x06

//20210602 Add ITC Smart Strength
#define   _ITC            0x16  // Smart Strenght Load cell LCB
//20211201 Add JIS DC LCB Type 04
#define   _JISDCLCB       0xFE  // Must be Change response machine type = 0x04
//20220216 Add Cxp Update LCB Type 03
#define   _CxpUpdate      0xFD  // Must be Change response machine type = 0x03

//---------------------------------------


#define   _SafetyKeyCheckTime_        100

// EEPROM
#define   _EEMachineType              50
#define   _EEClimbmillSetting         51
#define   _EER_PosMin                 52    // 52+53
#define   _EER_PosMax                 54    // 54+55
#define   _EEPowered                  56

// Add by Kunlung 2018114 Serial Number (Max Length = 250)
#define   _MaxLengthOfSerialNumber    250
#define   _EESerailNumberLength       100
#define   _EESerialNumber             101   // 101~1FF

// 20210602 ITC Serial Number (Max Length = 35 )
#define   _ITCMaxLengthOfSerialNumber 35



#define   _BIT0                       0x01
#define   _BIT1                       0x02
#define   _BIT2                       0x04
#define   _BIT3                       0x08
#define   _BIT4                       0x10
#define   _BIT5                       0x20
#define   _BIT6                       0x40
#define   _BIT7                       0x80

//
#define   _DEFAULT_BRAKE              330

//
typedef union {
  struct {
    unsigned short sResetOK:1 ;                  // LCB Power On Reset OK
    unsigned short sErPAction:1 ;                // LCB ErP Action
    unsigned short sDCPlugin:1 ;                 // LCB DC Plugin
    unsigned short sConsolePowerOn:1 ;           // LCB Power on for the console
    unsigned short sUpgradeMCU:1 ;               // LCB upgrade firmware
    unsigned short sTreadmillOperation:1 ;
    unsigned short sTreadMillEStop:1 ;
    unsigned short sSaveEEPROM:1 ;
    unsigned short sRISVbatOutputActive:1 ;
    unsigned short sRISVbatONSignalCheck:1 ;
    unsigned short sRISVbatOFFDelayTimeCheck:1 ;
    unsigned short sRISVbatON:1 ;
    unsigned short sRISWaitVbatONButtonRelease:1 ;
    unsigned short sRISIntoVbatPowerOFFMode:1 ;
    unsigned short sSetMachineType:1 ;
    unsigned short sSetSerialNumber:1 ;
  } B ;
  unsigned short All ;
} LCBSimulatorProcessStatusStatus ;

//
typedef union  {
  struct  {
    unsigned char Start:1 ;                     // Bit 0 : the speed control ( 1: start, 0:stop)
    unsigned char n1:1 ;                        // Bit 1 :
    unsigned char IRSensorEnable:1 ;  		// Bit 2 : IR Sensor use ( 0: not use, 1: use)
    unsigned char IRSensor:1 ;			// Bit 3 : the status of the IR Sensor is response at once
    unsigned char IRSensorSafteyStop :1 ;	// Bit 4 : IR Sensor is once touched ( 0 : not touch , 1 : touch but not unlock )
    unsigned char SafetyKeyEnable:1 ;           // Bit 5 : E-STOP use ( 0: not use, 1: use)
    unsigned char SafetyKey:1 ;                 // Bit 6 : the status of the E-STOP is response at once
    unsigned char SafetyStop :1 ;               // Bit 7 : E-STOP is once touched ( 0 : not touch , 1 : touch but not unlock)
  } Bits ;
  unsigned char  Full;   
} ClimbmillStatusByte ;                                             

// for EN2017 Climbmill                                                  
typedef union  {
  struct  {
    unsigned char EmergencySwitch:1 ;                   // Bit 8 : the status of UCB Emergency
    unsigned char EmergencySwitchSafetyStop:1 ;         // Bit 9 : UCB Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
    unsigned char HandrailEmergencySwitch:1 ;           // Bit 10: the status of Handrail Emergency
    unsigned char HandrailEmergencySwitchSafetyStop:1 ;	// Bit 11: Handrail Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
    unsigned char NoPersonUsing:1 ;	                // Bit 12: No person during using. ( 0: No, 1: Yes )
    unsigned char Rev1:1 ;                              // Bit 13: 
    unsigned char Rev2:1 ;                              // Bit 14: 
    unsigned char Rev3:1 ;                              // Bit 15: 
  } Bits ;
  unsigned char  Full;   
} ExtendClimbmillStatusByte ;
 

typedef union {
  struct {
    unsigned short Resistance:13 ;
    unsigned short Type:3 ;
  }  ;
  unsigned short Full ;
} ResistanceDataStruct ;


typedef union {
  struct {
    unsigned short Position:15 ;
    unsigned short Direct:1 ;
  } ;
  unsigned short Full ;
} InclineDataStruct ;


typedef union {
  struct {
    unsigned short RPM:15 ;
    unsigned short Direct:1 ;
  } ;
  unsigned short Full ;
} MotorDataStruct ;

typedef union {
  struct {
    unsigned short Top:8 ;
    unsigned short Bottom:8 ;
  } Member ;
  unsigned short Full ;
} InclineGapDataStruct ;


typedef union  {
  struct  {
    unsigned char AutoCalibrate:1 ;                     // Bit 0 : 
    unsigned char ManualUp:1 ;                          // Bit 1 :
    unsigned char ManualDown:1 ;  			                // Bit 2 : 
    unsigned char ManualStop:1 ;							          // Bit 3 : 
    unsigned char Manual:1 ;                            // Bit 4 : 
    unsigned char n2:1 ;                                // Bit 5 : 
    unsigned char n3:1 ;                                // Bit 6 : 
    unsigned char n4:1 ;                                // Bit 7 : 
  } Bits ;
  unsigned char  Full;
   
} InclineControlByte ;   

typedef union  {
  struct  {
    unsigned char Up:1 ;                                // Bit 0 : 
    unsigned char Down:1 ;                              // Bit 1 :
    unsigned char Stop:1 ;  			        // Bit 2 : 
    unsigned char Zero:1 ;				// Bit 3 : 
    unsigned char n1:1 ;                                // Bit 4 : 
    unsigned char n2:1 ;                                // Bit 5 : 
    unsigned char n3:1 ;                                // Bit 6 : 
    unsigned char n4:1 ;                                // Bit 7 : 
  } Bits ;
  unsigned char  Full;
   
} JISECBControlByte ; 

typedef struct {
  unsigned char NunberOfParameter ;
  unsigned char Parameter[_AthenaMaxSize] ; // _AthenaMaxSize/4 = N parameter
} AthenaParameter ;

typedef union  {
  struct  {
    unsigned long Position:24 ;
    unsigned long Mode:8 ;
  } Member ;
  unsigned char  Byte[4] ;
  unsigned long  Full;
   
} AthenaStepperMotor ; 



typedef union  {
  struct  {
    unsigned short Running:1 ;                    // Bit 0 : 
    unsigned short CalTimeout:1 ;                 // Bit 1 :
    unsigned short CalStrokeError:1 ;  		  // Bit 2 : 
    unsigned short DirveNoResponse:1 ;		  // Bit 3 : 
    unsigned short UnderValtage:1 ;               // Bit 4 : 
    unsigned short ThermalWaring:1 ;              // Bit 5 : 
    unsigned short ThermalShutdown:1 ;            // Bit 6 : 
    unsigned short OverCurrent:1 ;                // Bit 7 : 
    unsigned short n0:1 ;                         // Bit 8 : 
    unsigned short n1:1 ;                         // Bit 9 :
    unsigned short L6472BusyForInTime:1 ;  	  // Bit 10 : 
    unsigned short L6472BusyAbove5Sec:1 ;	  // Bit 11 : 
    unsigned short StepperMotorAbnormal:1 ;       // Bit 12 : 
    unsigned short LimitSwitchLowBrake:1 ;        // Bit 13 : 
    unsigned short LimitSwitchHighBrake:1 ;       // Bit 14 : 
    unsigned short n3:1 ;                         // Bit 15 :     
  } Bits ;
  unsigned short  Full;
   
} AthenaStepperMotorStatus ; 


//20210602 Load cell
typedef union  {
  struct  {
  unsigned short  Reserved ;
  unsigned short  NoMotionTimeout ;   //  default 60 (1 minute), MAXIMUM 655 seconds (10.9 minutes)
  unsigned short  SmartRepAccTrigger ;// acceleration that triggers start of E-lift mm/s^2
  unsigned short  mmPer1000count ;    // mm stack travel per 1000 encoder counts (DEFAULT 2283)
  unsigned short  BottomThreshold_mm; // bottom threshold (ADC's ignored below this value)
  unsigned short  TriggerHeight_mm ;  // height at which state machine leaves state 0 (9 MM DEFAULT)
  unsigned short  StrokeDown_mm ;     // minimum downward length that qualifies a rep (80mm DEFAULT)
  unsigned short  StrokeUp_mm ;       // minimum upward length that qualifies a partial rep (100mm DEFAULT)
  unsigned short  Direction ;         // LAST [ 1, 0 ] facing pcb (1)==CW is up , (-1)==CCW is up, DEFAULT)      
  }Cfg ;  
  unsigned char Data[18] ;
} LoadCellConfigration ;

typedef union  {
  struct  {
  unsigned char   Version ;
  unsigned short  counts_at_0v ;
  unsigned short  counts_per_mV_per_V ;
  unsigned long   gage_offset_nV_per_V ;
  unsigned short  loadcell_tare_mN ;
  unsigned short  standard_N_per_mV_per_V ;
  unsigned char   angle_loadcell_45_or_180 ;  
  unsigned short  nominal_plate_mass_g ;
  unsigned short  head_plate_assembly_mass_g ;
  unsigned short  plate_tolerance_scale_1000 ;
  unsigned short  offset_counts ;
  unsigned long   reserved ;
  unsigned long   time_stamp  ;
  
  }Cfg ;
  unsigned char Data[30] ;
}LoadCellComplementary ;


typedef union  {
  struct  {
  unsigned short  mass_method2_dag ;                  // FIRST 1 decagram = .01kg = 10 gram (based off acceleration)
  unsigned short  mass_method1_dag ;                  // 1 decagram = .01kg = 10 gram
  unsigned short  work_down_N_dm ;                    // 1 Newton-decimeter = 0.1 N-m (1 dm = 0.1m)
  unsigned short  time_down_ms ;
  unsigned short  dist_down_mm ;
  unsigned short  time_dwell_ms ;
  unsigned short  burst_peak_accel;                   //during time period burst_time. the peak mm_s2
  unsigned short  burst_work_N_dm;                    //during time period(above) Newton-decimeter work
  unsigned short  burst_time_ms;                      //duration from start to end of the e-lift up
  unsigned short  dist_up_mm;                         // total accumulated, user may have gone down and up
  unsigned short  work_up_N_dm;                       // from work and time, console can compute power
  unsigned short  peak_height_mm;                     // can determine stroke, then average velocity
  unsigned short  time_up_ms;                         //
  unsigned short  start_height_mm;                    // important for E-lift
  unsigned short  time_suspend_ms;                    // at START [[ this item is last out the port before CRC]]
  unsigned short  reserved;                           // for possibility of force or mass-related data
  unsigned short  relative_encoder_counts;            //0 after reset, positive values upward
  }Member ;
  unsigned char Data[34] ;
}LoadCellRepData ;










//


typedef struct {
  //----------------------------------------------------------------------------
  // Simulator
  unsigned char   LCBSimulatorType ;          // 1  
  unsigned char   InclineType ;               // 1  
  
  // UCB Command
  unsigned char   MachineType ;               // 1
  unsigned char	  PowerOffTime ;              // 1  
  unsigned char   GenMegPolePair ;            // 1 
  unsigned short  GearRate ;                  // 2 
  ResistanceDataStruct ResistanceType ;       // 2 
  unsigned short  LimitRpmForCharge ;         // 2 
  unsigned short  ChargeCurrnetForNoResistance ;  //2
  unsigned short  ConsolePowerConsumption ;       //2
  unsigned char   BatteryChargePercent ;      // 1
  // bike
  unsigned short  TargetPWM ;                 // 2
  unsigned short  TargetEMCurrent ;           // 2
  unsigned short  TargetWatts ;               // 2 
  // TM
  MotorDataStruct  MotorTargetSpeed ;         // 2  
  InclineDataStruct InclineTargetLocation ;   // 2
  // Stepper and Climbmill
  unsigned short  LimitRpmForResistance ;     // 2 
  
  //----------------------------------------------------------------------------
  // LCB Data
  unsigned char   BatteryCapacity ;           // 1
  unsigned char   DCBusVoltage ;              // 1  
  unsigned char   BatteryVoltage ;            // 1
  // Climbmill
  ClimbmillStatusByte ClimbmillStatus ;             // 1
  unsigned char   ClimbmillEStopValut ;             // 1
  // Climbmill EN 2017
  ExtendClimbmillStatusByte ClimbmillExtendStatus ; // 1  
  unsigned short  TargetSPM ;                       // 2
  unsigned short  SPM ;                             // 2
  unsigned short  ClimbmillBrakeONPressureValue ;   // 2  
  unsigned short  ClimbmillBrakeOFFPressureValue ;   // 2
  unsigned short  Watts ;                           // 2
  // Incline
  InclineControlByte  InclineControl ;        // 1
  unsigned short  InclineStroke ;             // 2
  unsigned short  InclinePercent ;            // 2
  unsigned short  InclineTargetPercent ;      // 2
  InclineGapDataStruct  InclineGap1 ;         // 2
  InclineGapDataStruct  InclineGap2 ;         // 2
  // TM
  unsigned char TreadmillUseStatus ;          // 1  
  MotorDataStruct  MotorSpeed ;               // 2
  InclineDataStruct InclineLocation ;         // 2
  unsigned short MotorHP ;                    // 2
  unsigned char  MotorType ;                  // 1
  
  // Bike
  unsigned short  StepRPM ;                   // 2
  
  // SerailNumber
  unsigned char ConsoleSN[50] ;
  unsigned char FrameSN[50] ;
  
  
  //Athena Indoor Cycle
  unsigned short  CrankRPM ;        //
  unsigned char   CrakeDirection ;  // 0: stop, 1: forward, 2:reverse
  unsigned short  FlywheelRPM ;     //
  unsigned short  StepperMotorStatus;
  unsigned char   ResistanceLevel ;
  AthenaStepperMotorStatus  MotorStatus ;
  AthenaParameter Calculation ;
  AthenaParameter Erg ;
  AthenaStepperMotor  MotorPosition ;   
    
  // JIS LCB 
  unsigned short  ECBPosition ;
  unsigned short  ECBCount ;
  JISECBControlByte ECBStatus ;
  JISECBControlByte ECBActionControl ;
  // Add 20210317 JIS LCB Type 0C
  unsigned short  JISLCBPWM[5] ;
  // Add 20210602
  unsigned short  AutoRepTime ;
  unsigned short  CableTension ;      // default 6620
  unsigned short  LoadCellADC ;
  unsigned short  LoadCellEncoder ;
  unsigned short  LoadCellOffset;
  unsigned short  LoadCellScale ;
  LoadCellConfigration  LoadCellConfig ; 
  LoadCellComplementary LeadCellCablibration ;
  unsigned short   RepCount ;
  LoadCellRepData RepData ;
  unsigned short  LoadCellDisMM ;
  
} LCBSystemControlDataStatus ;



typedef struct {
  unsigned char   ManchineType ;        // [0:rower, 1:s-drive, 2:indoor cycle]
  unsigned char   EntryToErPMode ;      // 0: LCB not support ERP mode function. 1: LCB can into ERP mode. 2: LCB can¡¦t into ERP mode. Please try later.
  unsigned short  ErrorCode ;
  unsigned char   RIS_SeqNumber ;
  unsigned short  BatteryVoltage ;      // 0.01V
  unsigned short  StreamRate ;
  unsigned short  BatteryOffDelayTime ;
  unsigned short  HeartRate ;           // 0 ~ 250
  // IndoorCycle
  unsigned short  Torque ;              // 0.1 Nm
  unsigned short  RPM ;                 //
  unsigned short  ResistancePosition ;  //
  unsigned short  ResistancePositionMin ;
  unsigned short  ResistancePositionMax ;
  // S-Drive
  unsigned short  LoadCell1 ;           // 0~9999
  unsigned short  LoadCell2 ;           // 0~9999
  // Rower
  unsigned short  Watts ;     
  // Add 20180111
  unsigned short  GeneratorRPM ;        //
  unsigned short  FlywheelRPM ;
  unsigned short  ElectronicsWatts ;
  unsigned char   Level ;
  unsigned short  ConsoleWatts ;
  // Add 20210413 UBCP
  unsigned char   Direction ;
  //
  unsigned char   FeatureAndDevice[9] ;
  //----------------------------------------------------------------------------
  //
} RISLCBSystemData ;


typedef union {
  struct {
    unsigned short Torque:1 ;	            // 0	0:Disable,1:Enable  , default: disable	
    unsigned short Resistanceposition:1 ;   // 1 	0:Disable,1:Enable  , default: disable	
    unsigned short ButtonToPowerOFF:1 ;	    // 2	0:Disable,1:Enable  , default: Enable
    unsigned short LoadCell1:1 ;	    // 3	0:Disable,1:Enable  , default: disable	
    unsigned short LoadCell2:1 ;	    // 4	0:Disable,1:Enable  , default: disable	
    unsigned short Rev2:11 ;                // 5	TBD
  } bit ;
  unsigned short Full ;
} RISLCBSensorStatusDataStruct ;


typedef union {
  struct{
    unsigned long P1 ;
    float         P2 ;
    float         P3 ;
    float         P4 ;
    float         P5 ;
    float         P6 ;
    float         P7 ;
    float         P8 ;
    float         P9 ;
    float         P10 ;
    float         P11 ;
    float         P12 ;
    float         P13 ;
    float         P14 ;
    float         P15 ;
  } member ;
  unsigned char All[60] ;
} PowerTable ;

extern LCBSystemControlDataStatus         LCBParameter ;
extern RISLCBSystemData                   RISLCBParameter ;
extern RISLCBSensorStatusDataStruct       RIS_SensorFunctionStatus ;
extern volatile LCBSimulatorProcessStatusStatus    LCBSystemProcessStatus ;
extern unsigned long RISBatteyOFFTime ;
extern const PowerTable  PowerTableParameters ;
extern const unsigned long ResistanceTable[26] ;
extern const unsigned short ICR70ECBcount[25] ;       // 20230314 ICR70

#ifdef  _SupportSerialNumberCommand 
extern unsigned char MachineSerialNumberLength ;
extern unsigned char MachineSerialNumber[_MaxLengthOfSerialNumber+1] ;
//20210602 ITC
extern unsigned char ITC_ConsoleSN[_ITCMaxLengthOfSerialNumber+1] ;
extern unsigned char ITC_FrameSN[_ITCMaxLengthOfSerialNumber+1] ;
#endif



/* Private function prototypes -----------------------------------------------*/
void LCBSimulator_Initial(void) ;
void LCBSimulator_SystemTime(void)  ;
unsigned char LCBSimulator_Process(unsigned char Show,unsigned char *TypeScreenPtr) ;
void LCBSimulator_ClimbmillProcess(unsigned char Button) ;
void LCBSimulator_ClimbmillSafetyProcess(void);//unsigned char Button) ;
void LCBSimulator_TreadMillProcess( unsigned char Type ) ;

unsigned char LCBSimulator_ConsolePowerControl(void) ;
void LCBMain_SetDCPluginStatus(unsigned char Mode) ;
unsigned char LCBSimulator_GetDCPluginStatus(void) ;
void LCBSimulator_SetConsolePowerStatus(unsigned char Status) ;
unsigned char LCBSimulator_GetConsolePowerStatus(void) ;
void LCBSimulator_SetDelayConsolePowerOnTimeLimit(unsigned long Time) ;
void LCBSimulator_SetErPStatus(unsigned char SetStatus) ;
unsigned char LCBSimulator_GetErPStatus(void) ;
void LCBSimulator_SetResetOKStatus(unsigned char SetStatus) ;
unsigned char LCBSimulator_GetResetOKStatus(void) ;
void LCBSimulator_SetTreadmillOperation(unsigned char SetStatus) ;
void LCBSimulator_SetsSaveEEPROMStatus(unsigned char SetStatus) ;
unsigned char LCBSimulator_GetSaveEEPROMStatus(void) ;
void LCBSimulator_SetSerialNumber(unsigned char SetStatus) ;
unsigned char LCBSimulator_GetSetSerialNumber(void) ;
unsigned char LCBSimulator_MotorPosToResistanceLevel(unsigned long Mpos) ;
unsigned long LCBSimulator_GetMotorPosMax(unsigned char Mode) ;



#endif /* __LCBSIMULATOR_H__*/


