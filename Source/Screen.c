
#include  "pindefine.h"
#include  "Screen.h"
#include  "LCMDisplay.h"
#include  "LCBSimulator.h"
#include  "Encoder.h"
#include  "JHTLCBComm.h"

#define   _STRING                     0
#define   _DEC_CHAR                   1
#define   _DEC_INT                    2
#define   _DEC_LONG                   3
#define   _HEX_nn                     4
#define   _HEX_nnnn                   5
#define   _HEX_nnnnnnnn               6


//                           0123456789012345
const unsigned char _S0[] = "ControlZone:    " ; //12 Type: ON/OFF , LCBParameter.ClimbmillStatus.Bits.SafetyKey
const unsigned char _S1[] = "FrameIR:        " ; //8  Type: ON/OFF , LCBParameter.ClimbmillStatus.Bits.IRSensor
const unsigned char _S2[] = "Handrail:       " ; //9  Type: ON/OFF , LCBParameter.ClimbmillExtendStatus.Bits.HandrailEmergencySwitch
const unsigned char _S3[] = "Emergency:      " ; //10 Type: ON/OFF , LCBParameter.ClimbmillExtendStatus.Bits.EmergencySwitch
const unsigned char _S4[] = "NoPerson:       " ; //9  Type: ON/OFF , LCBParameter.ClimbmillExtendStatus.Bits.NoPersonUsing
const unsigned char _S5[] = "Watts:          " ; //6,12  Type: Word   , LCBParameter.Watts
const unsigned char _S6[] = "SPM:            " ; //4,10  Type: Word   , LCBParameter.SPM
const unsigned char _S7[] = "BrakeON:        " ; //8,    Type: Word   , LCBParameter.ClimbmillBrakeONPressureValue
const unsigned char _S8[] = "RPM:            " ; //4  Type: Word(nnn)
const unsigned char _S9[] = "Plug in:        " ; //8  Type: Yes/No
const unsigned char _S10[]= "Voltage:        " ; // only show
const unsigned char _S11[]= "Current:        " ; // only show
const unsigned char _S12[]= "RXD:            " ; //4  Type: long
const unsigned char _S13[]= "TXD:            " ; //4  Type: long
const unsigned char _S14[]= "ERROR No:       " ; //9  Type: Hex(nnnn)
const unsigned char _S15[]= " EMERGENCY STOP " ; // only show
const unsigned char _S16[]= "                " ; // SPACE Line
const unsigned char _S17[]= "BrakeOFF:       " ; //9,    Type: Word   , LCBParameter.ClimbmillBrakeOFFPressureValue
const unsigned char _S18[]= "Torque:         " ; //7  Type: Word(nnn.n)
const unsigned char _S19[]= "R_POS:          " ; //6  Type: Word(nnnnn)
const unsigned char _S20[]= "Battery:        " ; //8  Type: Word(nn.nn)
const unsigned char _S21[]= "LoadCell1:      " ; //10 Type: Word(nnnn)
const unsigned char _S22[]= "LoadCell2:      " ; //10 Type: Word(nnnn)
const unsigned char _S23[]= "BPM:            " ; //4, Type: Word(nnn)
const unsigned char _S24[]= "MotorRPM:       " ; //9  Type: Word(nnnnn)
const unsigned char _S25[]= "Incline:        " ; //8  Type: Word(nnnnn)
const unsigned char _S26[]= "ACT:   SPM:     " ; //4,11 Type
const unsigned char _S27[]= "GEN.RPM:        " ; //8  Type: Word(nnnnn)
const unsigned char _S28[]= "FW.RPM:         " ; //7  Type: Word(nnnnn)
const unsigned char _S29[]= "EL.Watts:       " ; //9  Type: Word(nnn.nn)
const unsigned char _S30[]= "LEVEL:          " ; //6  Type: Word(nnn) 0~255
const unsigned char _S31[]= "LimitRPM:       " ; //9  Type: Word(nnn) 0~255
const unsigned char _S32[]= "R_Value:        " ; //8  Type: Word(nnnnn)
const unsigned char _S33[]= "GearRatio:      " ; //10 Type: Word(nnnnn)
const unsigned char _S34[]= "PolePair:       " ; //9  Tyep: nnn
const unsigned char _S36[]= "SN:             " ; //3  Tyep: string
const unsigned char _S37[]= "OC0144:         " ; //7  Type: ON/OFF ,
const unsigned char _S38[]= "OC0146:         " ; //7  Type: ON/OFF ,
const unsigned char _S39[]= "ECB POS:        " ; //8  Type: Word(nnnnn)
const unsigned char _S40[]= "Crank:          " ; //6  Type: Word(nnnnn)
const unsigned char _S41[]= "FlyWheel:       " ; //9  Type: Word(nnnnn)
const unsigned char _S42[]= "POS:            " ; //4  Type: 24Bit(nnnnnnnn)
const unsigned char _S43[]= "Resistance:     " ; //11 Tyep: nn
const unsigned char _S44[]= "Rep:            " ; //4  Type:
const unsigned char _S45[]= "C_SN:           " ; //5  Tyep: string
const unsigned char _S46[]= "F_SN:           " ; //5  Tyep: string
const unsigned char _S47[]= "Plate:          " ; //6  Type: Word(nnn.nn)
const unsigned char _S48[]= "Auto Rep:       " ; //9  Type: 0=OFF,1-Nsec
const unsigned char _S49[]= "Non Used:       " ; //9  Type: ON/OFF 
//const unsigned char _S32[]= "Auto Test ES:   " ; //13 Type: Yes/No


const Parameters Climbmill[_ClimbmillParameterSize] = { 
  // 
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S0,  (unsigned short*)&LCBParameter.ClimbmillStatus.Full       , _BIT6 , _Adjustable , 12, _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S1,  (unsigned short*)&LCBParameter.ClimbmillStatus.Full       , _BIT3 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S31, &LCBParameter.LimitRpmForResistance                       , 0     , _Fixed      , 9 , _TYPE_RES_RPM ,0    ,250      ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;

const Parameters ENClimbmill[_ENClimbmillParameterSize] = { 
  // 
  { _S0,  (unsigned short*)&LCBParameter.ClimbmillStatus.Full       , _BIT6 , _Adjustable , 12, _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S1,  (unsigned short*)&LCBParameter.ClimbmillStatus.Full       , _BIT3 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S2,  (unsigned short*)&LCBParameter.ClimbmillExtendStatus.Full , _BIT2 , _Adjustable , 9 , _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S3,  (unsigned short*)&LCBParameter.ClimbmillExtendStatus.Full , _BIT0 , _Adjustable , 10, _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S4,  (unsigned short*)&LCBParameter.ClimbmillExtendStatus.Full , _BIT4 , _Adjustable , 9 , _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S5,  &LCBParameter.Watts                                       , 0     , _Adjustable , 6 , _TYPE_WATT    ,0    ,9999           ,_NORMAL  ,1},
  { _S6,  &LCBParameter.SPM                                         , 0     , _Adjustable , 4 , _TYPE_SPM     ,0    ,9999           ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255            ,_NORMAL  ,1}
//  { _S7,  &LCBParameter.ClimbmillBrakeONPressureValue               , 0     , _Adjustable , 8 , _TYPE_BRAKE   ,0    ,_DEFAULT_BRAKE ,_NORMAL},
//  { _S17, &LCBParameter.ClimbmillBrakeOFFPressureValue              , 0     , _Adjustable , 9 , _TYPE_BRAKE   ,0    ,_DEFAULT_BRAKE ,_NORMAL }
} ;


const Parameters  IndoorCycleLCB[_IndoorCycleParameterSize] = {
  { _S8,  &RISLCBParameter.RPM                                      , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S18, &RISLCBParameter.Torque                                   , 0     , _Adjustable , 7 , _TYPE_NM      ,0    ,9999     ,_NORMAL  ,1},
  { _S19, &RISLCBParameter.ResistancePosition                       , 0     , _Adjustable , 6 , _TYPE_RPOS    ,0    ,10000    ,_NORMAL  ,1},
  { _S20, &RISLCBParameter.BatteryVoltage                           , 0     , _Adjustable , 8 , _TYPE_VOLTAGE ,0    ,5000     ,_NORMAL  ,1},
  { _S5,  &RISLCBParameter.Watts                                    , 0     , _Adjustable , 6 , _TYPE_WATTX10 ,0    ,6500     ,_NORMAL  ,1},
  { _S14, &RISLCBParameter.ErrorCode                                , 0     , _Adjustable , 9 , _TYPE_ERROR   ,0    ,(_ErrorListSize-1),_RING  ,1},
  { _S27, &RISLCBParameter.GeneratorRPM                             , 0     , _Adjustable , 8 , _TYPE_MOTOR   ,0    ,6000     ,_NORMAL  ,1},
  { _S28, &RISLCBParameter.FlywheelRPM                              , 0     , _Adjustable , 7 , _TYPE_MOTOR   ,0    ,6000     ,_NORMAL  ,1},
  { _S29, &RISLCBParameter.ElectronicsWatts                         , 0     , _Adjustable , 9 , _TYPE_WATTX100,0    ,65535    ,_NORMAL  ,1},
  { _S30, (unsigned short*)&RISLCBParameter.Level                   , 0     , _Adjustable , 6 , _TYPE_RPM    ,0    ,255      ,_NORMAL  ,1}
} ;

const Parameters  SDriveLCB[_SDriveParameterSize] = {
  { _S8,  &RISLCBParameter.RPM                                      , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S20, &RISLCBParameter.BatteryVoltage                           , 0     , _Adjustable , 8 , _TYPE_VOLTAGE ,0    ,5000     ,_NORMAL  ,1},
  { _S23, &RISLCBParameter.HeartRate                                , 0     , _Adjustable , 4 , _TYPE_BPM     ,0    ,250      ,_NORMAL  ,1},
  { _S21, &RISLCBParameter.LoadCell1                                , 0     , _Adjustable , 10, _TYPE_LC      ,0    ,10000    ,_NORMAL  ,1},
  { _S22, &RISLCBParameter.LoadCell2                                , 0     , _Adjustable , 10, _TYPE_LC      ,0    ,10000    ,_NORMAL  ,1},
  { _S14, &RISLCBParameter.ErrorCode                                , 0     , _Adjustable , 9 , _TYPE_ERROR   ,0    ,(_ErrorListSize-1),_RING  ,1},
  { _S27, &RISLCBParameter.GeneratorRPM                             , 0     , _Adjustable , 8 , _TYPE_MOTOR   ,0    ,60000    ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&RIS_LCBStatus.Full                      , _BIT4 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1}
} ;

const Parameters  RowerLCB[_RowerParameterSize] = {
  { _S6 , &RISLCBParameter.RPM                                      , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,80       ,_NORMAL  ,1},
  { _S20, &RISLCBParameter.BatteryVoltage                           , 0     , _Adjustable , 8 , _TYPE_VOLTAGE ,0    ,5000     ,_NORMAL  ,1},
  { _S23, &RISLCBParameter.HeartRate                                , 0     , _Adjustable , 4 , _TYPE_BPM     ,0    ,250      ,_NORMAL  ,1},
  { _S14, &RISLCBParameter.ErrorCode                                , 0     , _Adjustable , 9 , _TYPE_ERROR   ,0    ,(_ErrorListSize-1),_RING  ,1},
  { _S27, &RISLCBParameter.GeneratorRPM                             , 0     , _Adjustable , 8 , _TYPE_MOTOR   ,0    ,60000    ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&RIS_LCBStatus.Full                      , _BIT4 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1}
} ;

const Parameters BikeLCB[_BikeParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&LCBSystemProcessStatus.All              , _BIT2 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S32, &LCBParameter.TargetPWM                                   , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S33, &LCBParameter.GearRate                                    , 0     , _Fixed      , 10, _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S34, (unsigned short*)&LCBParameter.GenMegPolePair             , 0     , _Fixed      , 9 , _TYPE_RPM     ,0    ,255      ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;


const Parameters ChopperBikeLCB[_ChopperBikeParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&LCBSystemProcessStatus.All              , _BIT2 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;

const Parameters StepperLCB[_StepperParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&LCBSystemProcessStatus.All              , _BIT2 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S31, &LCBParameter.LimitRpmForResistance                       , 0     , _Fixed      , 9 , _TYPE_RES_RPM ,0    ,250      ,_NORMAL  ,1},
  { _S33, &LCBParameter.GearRate                                    , 0     , _Fixed      , 10, _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S34, (unsigned short*)&LCBParameter.GenMegPolePair             , 0     , _Fixed      , 9 , _TYPE_RPM     ,0    ,255      ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;



const Parameters AscentTrainerLCB[_AscentTrainerParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&LCBSystemProcessStatus.All              , _BIT2 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S32, &LCBParameter.TargetPWM                                   , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S25, &LCBParameter.InclineTargetPercent                        , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S33, &LCBParameter.GearRate                                    , 0     , _Fixed      , 10, _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S34, (unsigned short*)&LCBParameter.GenMegPolePair             , 0     , _Fixed      , 9 , _TYPE_RPM     ,0    ,255      ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;



const Parameters  TreadmillLCB[_TreadmillParameterSize] = {
  { _S24, &LCBParameter.MotorSpeed.Full                             , 0     , _Fixed      , 9 , _TYPE_MOTOR   ,0    ,250      ,_NORMAL  ,1},
  { _S25, &LCBParameter.InclineLocation.Full                        , 0     , _Fixed      , 8 , _TYPE_INCLINE ,0    ,9999     ,_NORMAL  ,1},
  { _S49, &SpecControlStatus.Full                                   , _BIT0 , _Adjustable , 9 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S14, &RISLCBParameter.ErrorCode                                , 0     , _Adjustable , 9 , _TYPE_ERROR   ,0    ,(_ErrorListSize-1),_RING  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1},
  { _S37, &ErrorCodeStatus.Full                                     , _BIT1 , _Adjustable , 7 , _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1},
  { _S38, &ErrorCodeStatus.Full                                     , _BIT2 , _Adjustable , 7 , _TYPE_BIT     ,0    ,1              ,_NORMAL  ,1}
} ;


const Parameters JISLCB12[_Bike30ParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,200      ,_NORMAL  ,1},
  { _S39, &LCBParameter.ECBPosition                                 , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}
} ;

const Parameters JISLCB0B[_A30ParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,200      ,_NORMAL  ,1},
  { _S39, &LCBParameter.ECBCount                                    , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S25, &LCBParameter.InclineLocation.Full                        , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}  
} ;

const Parameters JISLCB15[_A50ParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,200      ,_NORMAL  ,1},
  { _S32, &LCBParameter.TargetPWM                                   , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1},
  { _S25, &LCBParameter.InclineLocation.Full                        , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}  
} ;

const Parameters  JISTreadmillLCB[_JISTreadmillParameterSize] = {
  { _S24, &LCBParameter.MotorSpeed.Full                             , 0     , _Fixed      , 9 , _TYPE_MOTOR   ,0    ,250      ,_NORMAL  ,1},
  { _S25, &LCBParameter.InclineLocation.Full                        , 0     , _Fixed      , 8 , _TYPE_INCLINE ,0    ,9999     ,_NORMAL  ,1}
} ;


const Parameters JISLCB0C[_URE50ParameterSize] = {
  { _S8,  &LCBParameter.StepRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,200      ,_NORMAL  ,1},
  { _S32, &LCBParameter.TargetPWM                                   , 0     , _Fixed      , 8 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}
} ;


const Parameters AthenaLCB[_AthenaParameterSize]= {
  { _S40, &LCBParameter.CrankRPM                                     , 0     , _Adjustable , 6 , _TYPE_RPM     ,0    ,500      ,_NORMAL  ,1},  
  { _S41, &LCBParameter.FlywheelRPM                                  , 0     , _Fixed      , 9 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}, 
  { _S5,  &LCBParameter.Watts                                        , 0     , _Adjustable , 6 , _TYPE_WATT    ,0    ,9999     ,_NORMAL  ,10},
  { _S43, (unsigned short*)&LCBParameter.ResistanceLevel             , 0     , _Fixed      , 11, _TYPE_RPM     ,0    ,25       ,_NORMAL  ,1},
  { _S42, (unsigned short*)&LCBParameter.MotorPosition.Full          , 0     , _Fixed      , 4 , _TYPE_MOT     ,0    ,65535    ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                   , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;


const Parameters UBCPLCB[_UBCPParameterSize]= {
  { _S40, &RISLCBParameter.RPM                                       , 0     , _Adjustable , 6 , _TYPE_RPM     ,0    ,500      ,_NORMAL  ,1},  
  { _S41, &RISLCBParameter.FlywheelRPM                               , 0     , _Fixed      , 9 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}, 
  { _S5,  &RISLCBParameter.Watts                                     , 0     , _Adjustable , 6 , _TYPE_WATT    ,0    ,9999     ,_NORMAL  ,10},
  { _S43, (unsigned short*)&RISLCBParameter.Level                    , 0     , _Fixed      , 11, _TYPE_RPM     ,0    ,25       ,_NORMAL  ,1},
  { _S42, (unsigned short*)&LCBParameter.MotorPosition.Full          , 0     , _Fixed      , 4 , _TYPE_MOT     ,0    ,65535    ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                   , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;


//
const Parameters ITCLCB[_ITCParameterSize] = {
  { _S48, &LCBParameter.AutoRepTime                                 , 0     , _Adjustable , 9 , _TYPE_Auto    ,0    ,5         ,_NORMAL  ,1},  
  { _S44, &LCBParameter.RepCount                                    , 0     , _Adjustable , 4 , _TYPE_BarX10  ,0    ,100       ,_NORMAL  ,2},  
  { _S47, &LCBParameter.RepData.Member.mass_method1_dag             , 0     , _Adjustable , 6 , _TYPE_Plate   ,1000 ,20000     ,_NORMAL  ,100},
  { _S45, (unsigned short*)&ITC_ConsoleSN[0]                        , 0     , _Fixed      , 5 , _TYPE_STRING  ,0    ,35        ,_NORMAL  ,1},
  { _S46, (unsigned short*)&ITC_FrameSN[0]                          , 0     , _Fixed      , 5 , _TYPE_STRING  ,0    ,35        ,_NORMAL  ,1}
  
} ;
//
//Add 20220216 Indoor cycle FC28/FC29
const Parameters  CxpUpdateLCB[_CxpUpdateParameterSize] = {
  { _S8,  &RISLCBParameter.RPM                                      , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,250      ,_NORMAL  ,1},
  { _S18, &RISLCBParameter.Torque                                   , 0     , _Adjustable , 7 , _TYPE_NM      ,0    ,9999     ,_NORMAL  ,1},
  { _S19, &RISLCBParameter.ResistancePosition                       , 0     , _Adjustable , 6 , _TYPE_RPOS    ,0    ,10000    ,_NORMAL  ,1},
  { _S20, &RISLCBParameter.BatteryVoltage                           , 0     , _Adjustable , 8 , _TYPE_VOLTAGE ,0    ,5000     ,_NORMAL  ,1},
  { _S5,  &RISLCBParameter.Watts                                    , 0     , _Adjustable , 6 , _TYPE_WATTX10 ,0    ,6500     ,_NORMAL  ,1},
  { _S14, &RISLCBParameter.ErrorCode                                , 0     , _Adjustable , 9 , _TYPE_ERROR   ,0    ,(_ErrorListSize-1),_RING  ,1},
  { _S27, &RISLCBParameter.GeneratorRPM                             , 0     , _Adjustable , 8 , _TYPE_MOTOR   ,0    ,6000     ,_NORMAL  ,1},
  { _S28, &RISLCBParameter.FlywheelRPM                              , 0     , _Adjustable , 7 , _TYPE_MOTOR   ,0    ,6000     ,_NORMAL  ,1},
  { _S29, &RISLCBParameter.ElectronicsWatts                         , 0     , _Adjustable , 9 , _TYPE_WATTX100,0    ,65535    ,_NORMAL  ,1},
  { _S30, (unsigned short*)&RISLCBParameter.Level                   , 0     , _Adjustable , 6 , _TYPE_RPM    ,0    ,255      ,_NORMAL  ,1},
  { _S9,  (unsigned short*)&RIS_LCBStatus.Full                      , _BIT4 , _Adjustable , 8 , _TYPE_BIT     ,0    ,1        ,_NORMAL  ,1},
  { _S36, (unsigned short*)&MachineSerialNumber[0]                  , 0     , _Fixed      , 3 , _TYPE_STRING  ,0    ,255      ,_NORMAL  ,1}
} ;

// 20221130 Add Support ICR70
const Parameters ICR70LCB[_ICR70ParameterSize]= {
  { _S8,  &LCBParameter.CrankRPM                                     , 0     , _Adjustable , 4 , _TYPE_RPM     ,0    ,500      ,_NORMAL  ,1},  
  { _S5,  &LCBParameter.Watts                                        , 0     , _Adjustable , 6 , _TYPE_WATTX10 ,0    ,65535    ,_NORMAL  ,10},
  { _S30, (unsigned short*)&LCBParameter.ResistanceLevel             , 0     , _Fixed      , 6 , _TYPE_RPM     ,0    ,99       ,_NORMAL  ,1},
  { _S42, (unsigned short*)&LCBParameter.ECBPosition                 , 0     , _Fixed      , 4 , _TYPE_PWM     ,0    ,65535    ,_NORMAL  ,1}
} ;



/**
**/
void Screen_ShowEmergency(unsigned char Status)
{
  if( Status == 0 )
      LCMDisplay_show_string(0,1,(unsigned char*)&_S16[0]) ;
  else
      LCMDisplay_show_string(0,1,(unsigned char*)&_S15[0]) ;
  
  return ;
}




/*


*/
/*
void Screen_ShowParameter(unsigned char ParaNo)
{
  LCMDisplay_show_string(Scr[ParaNo].X,Scr[ParaNo].Y,Scr[ParaNo].ScrPtr) ;
  
  
  return ;
}
*/

/**

**/
void Screen_ShowOnOffParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned char status)
{
  LCMDisplay_show_string(x,y,ptr) ;
  if( status == 1 )
      {
      LCMDisplay_show_one_char(dx,y,'O') ;
      LCMDisplay_show_one_char(dx+1,y,'N') ;
      }
  else
      {
      LCMDisplay_show_one_char(dx,y,'O') ;
      LCMDisplay_show_one_char(dx+1,y,'F') ;  
      LCMDisplay_show_one_char(dx+2,y,'F') ;       
      }
  return ;
}

/**

**/
void Screen_ShowWattsParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short tdata )
{
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_word_bcd(dx,y,sdata,4) ;
  LCMDisplay_show_word_bcd((dx+5),y,tdata,4) ;
}

/**

**/
void Screen_ShowSPMParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short tdata)
{
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_word_bcd(dx,y,sdata,6) ;
  LCMDisplay_show_word_bcd((dx+6),y,tdata,6) ;
}

/**

**/
void Screen_ShowBrakePressureValue( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_word_bcd(dx,y,sdata,7) ;
}


void Screen_ShowRPMParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short sdata1 )
{
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_word_bcd(dx,y,sdata,3) ; // xxx
  LCMDisplay_show_word_bcd((dx+6),y,sdata1,3) ; // xxx
  return ;
}

unsigned char Screen_ShowNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,3) ; // xxx
  return px ;
}


unsigned char Screen_ShowNNNDNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,6) ; // xxx.x
  return px ;
}

unsigned char Screen_ShowNNDNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,11) ; // xx.x
  return px ;
}
                  
unsigned char Screen_ShowNNDNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,10) ; // xx.xx
  return px ;
}


unsigned char Screen_ShowNNNDNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,12) ; // xxx.xx
  return px ;
}

unsigned char Screen_ShowNNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,5) ; // xxxxx
  return px ;
}


unsigned char Screen_ShowNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  px = LCMDisplay_show_word_bcd(dx,y,sdata,4) ; // xxxx
  return px ;
}

unsigned char Screen_ShowHexNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata)
{
  unsigned char px ;
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_word(dx,y,sdata) ; // xxxx
  px = dx+4 ;
  return px ;
}


void Screen_ShowNNNNNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short* sptr)
{
  unsigned long Tmp ;
  Tmp = *sptr  + ((unsigned long)*(sptr+1)&0x00FF) * 0x10000  ;
  LCMDisplay_show_string(x,y,ptr) ;
  LCMDisplay_show_long_bcd(dx,y,Tmp) ; // xxxxxxxx
  return ;
}

void Screen_ShowBarParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short* sptr,unsigned char Divx)
{
  unsigned short Tmp ;
  unsigned char i ;
  unsigned char ix ;
  unsigned char Tmp1 ;
  Tmp = *sptr ;
  ix = Tmp / Divx ;
  Tmp1 = (Tmp % Divx ) / (Divx/5) ;
  LCMDisplay_show_string(x,y,ptr) ;
  for( i = 0 ; i < ix ; i++ )
      LCMDisplay_show_one_char((dx+i),y,4) ;
  if(Tmp1 > 0)
      LCMDisplay_show_one_char((dx+i),y,(Tmp1-1)) ;
  return ;
}


//------------------- End of File ----------------------------------------------
