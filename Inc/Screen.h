#ifndef __SCREEN_H__
#define __SCREEN_H__

#define   _Adjustable                 1
#define   _Fixed                      0

#define   _TYPE_BIT                   0
#define   _TYPE_WATT                  1
#define   _TYPE_SPM                   2
#define   _TYPE_BRAKE                 3
#define   _TYPE_RPM                   4 // xxx
#define   _TYPE_CURRENT               5 // x.xxxA
#define   _TYPE_VOLTAGE               6 // xx.xV
#define   _TYPE_NM                    7 // xxx.xNm
#define   _TYPE_RPOS                  8 // xxxxx
#define   _TYPE_LC                    9 // xxxxx
#define   _TYPE_BPM                   10 // xxx, 0~250
#define   _TYPE_WATTS                 11 // xxxx
#define   _TYPE_ERROR                 12 // xxxx hex
#define   _TYPE_MOTOR                 13 // xxxxx
#define   _TYPE_INCLINE               14 // XXXXX
#define   _TYPE_WATTX100              15 // xxx.xx
#define   _TYPE_RES_RPM               16 // xxx
#define   _TYPE_RPM1                  17
#define   _TYPE_PWM                   18 // xxxxx
#define   _TYPE_STRING                19
#define   _TYPE_MOT                   20 // xxxxx
#define   _TYPE_WATTX10               21 // xxx.x
#define   _TYPE_BarX10                22 // count/10
#define   _TYPE_BarX50                23 // count/50
#define   _TYPE_Plate                 24 // xxx.xxkg
#define   _TYPE_Auto                  25 // 0=oFF,1-N sec.

#define   _ClimbmillParameterSize     5
#define   _ENClimbmillParameterSize   8
#define   _IndoorCycleParameterSize   10
#define   _CxpUpdateParameterSize     12
#define   _SDriveParameterSize        8   // 7->8 20220216 add plug in
#define   _RowerParameterSize         6   // 5->6 20220216 add plug in
#define   _BikeParameterSize          6
#define   _ChopperBikeParameterSize   3
#define   _StepperParameterSize       6
#define   _TreadmillParameterSize     7
#define   _AscentTrainerParameterSize 7
// for JIS LCB
#define   _Bike30ParameterSize        2
#define   _URE50ParameterSize         2
#define   _A30ParameterSize           3
#define   _A50ParameterSize           3
#define   _JISTreadmillParameterSize  2
#define   _AthenaParameterSize        6
#define   _UBCPParameterSize          6
#define   _ITCParameterSize           5
#define   _ICR70ParameterSize         4   // 20230314 Add Support ICR70


typedef struct {
  unsigned char const *ScrPtr ;       // Show message memory address
  unsigned short *ParameterPtr ; // Parameter memory address
  unsigned char BITMASK ;
  unsigned char EditAvailable ; // 0: Disable , 1: Enable
  unsigned char ShowAddr ;      // Start Addrress
  unsigned char ShowType ;      // show data type
  unsigned short MinLimit ;     // Minuum value
  unsigned short MaxLimit ;     // Maxmun value
  unsigned char  AdjMode ;      // _RING/_NORMAL
  // Add Scale of adjustment
  unsigned char Scale ;         // 1-255
} Parameters ;
  

typedef struct {
  unsigned char *ScrPtr ;
  unsigned char X ;
  unsigned char Y ;
  unsigned char *ScrDataPtr ;
  unsigned char DataType ;
  unsigned char dX ;
  unsigned char dY ;
} ScreenType ;

/*
typedef struct {
  unsigned char *ScrDataPtr ;
  unsigned char DataType ;
  unsigned char X ;
  unsigned char Y ;
} ScreenDataType ;
*/


extern const Parameters Climbmill[_ClimbmillParameterSize] ;
extern const Parameters ENClimbmill[_ENClimbmillParameterSize] ;
extern const Parameters IndoorCycleLCB[_IndoorCycleParameterSize] ;
extern const Parameters SDriveLCB[_SDriveParameterSize] ;
extern const Parameters RowerLCB[_RowerParameterSize] ;
extern const Parameters BikeLCB[_BikeParameterSize] ;        // for LCB Type 0x07, EP/Bike
extern const Parameters ChopperBikeLCB[_ChopperBikeParameterSize] ; // Add by Kunlug 20190410 for Chopper
extern const Parameters StepperLCB[_StepperParameterSize] ;  // for LCB Type 0x07, Stepper
extern const Parameters TreadmillLCB[_TreadmillParameterSize] ;
extern const Parameters AscentTrainerLCB[_AscentTrainerParameterSize] ;
extern const Parameters JISLCB12[_Bike30ParameterSize] ;  // JIS Type 12
extern const Parameters JISLCB0B[_A30ParameterSize] ;     // JIS Type 0B
extern const Parameters JISTreadmillLCB[_JISTreadmillParameterSize] ;
extern const Parameters JISLCB15[_A50ParameterSize] ;
extern const Parameters JISLCB0C[_URE50ParameterSize] ;
extern const Parameters AthenaLCB[_AthenaParameterSize] ;
extern const Parameters UBCPLCB[_UBCPParameterSize] ;
extern const Parameters ITCLCB[_ITCParameterSize] ;
extern const Parameters CxpUpdateLCB[_CxpUpdateParameterSize] ; // for Cxp LCB Type 03
extern const Parameters ICR70LCB[_ICR70ParameterSize] ; // for ICR70 Type 1D

void Screen_ShowEmergency(unsigned char Status) ;
//void Screen_ShowParameter(unsigned char ParaNo) ;
void Screen_ShowOnOffParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned char status) ;
void Screen_ShowWattsParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short tdata) ;
void Screen_ShowSPMParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short tdata) ;
void Screen_ShowBrakePressureValue( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata) ;
void Screen_ShowRPMParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata, unsigned short sdata1 );
unsigned char Screen_ShowNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowNNNDNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowNNDNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowNNDNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata) ;
unsigned char Screen_ShowNNNDNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowNNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata);
unsigned char Screen_ShowHexNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short sdata) ;
void Screen_ShowNNNNNNNNParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short* sptr) ;
void Screen_ShowBarParameter( unsigned char *ptr, unsigned char x, unsigned char y, unsigned char dx, unsigned short* sptr,unsigned char Divx) ;

#endif /* __SCREEN_H__*/
