#ifndef __ROWER_WATTS_H__
#define __ROWER_WATTS_H__
//------------------------------------------------------------------------------
#include  "machine.h"

//--------- define ---------//
#define _RopeLength           10000  // 0.1 mm // 100cm
#define _RopeToGearCircleX10  100   // 10 circles
#define _LengthPerPulseX100   (_RopeLength*10000/_RopeToGearCircleX10/MCGENERATOR_BELT_RATIO/MCGENERATOR_POLE_PAIR)  // 0.01mm
#define _LengthPerPulseX10    ((_LengthPerPulseX100+5)/10) // 0.1mm

// Rower data for acting
#define _RowerHandleData_Max      255
#define _RowerHandleData_Max_Extension  _RowerHandleData_Max


//--------- Parameter index ---------//
enum{_Get = 0, _Set};
enum
{
    //--- 8 bits parameter -------------------------------
    // index
    _RowerWatts_SPM = 0,
    
    // end
    _RowerWatts_8bits_End,
    
    //--- 16 bits parameter ------------------------------
    // index
    _RowerWatts_WattsX10 = _RowerWatts_8bits_End,
    _RowerWatts_DragFactor,
    
    // end
    _RowerWatts_16bits_End,
    
    //--- 32 bits parameter ------------------------------
    // index
    _RowerWatts_Strokes = _RowerWatts_16bits_End,
    
    // end
    _RowerWatts_32bits_End,
    //----------------------------------------------------
};
#define _RowerWatts_8bits_Total      _RowerWatts_8bits_End
#define _RowerWatts_16bits_Total     (_RowerWatts_16bits_End - _RowerWatts_8bits_End)
#define _RowerWatts_32bits_Total     (_RowerWatts_32bits_End - _RowerWatts_16bits_End)


//--------- Type define ---------//
typedef struct
{
  /*For 1st order filter */
  unsigned short  Length;
  unsigned long Sum;
  unsigned short  Value;
  unsigned short  Max;
  unsigned short  Min;
} RpmFilterStructA;


typedef struct
{
  /*For moving average filter*/
  unsigned short  Length;
  unsigned long Sum;
  unsigned short  Value;
  unsigned short  Point;       /*FIFO point */
  unsigned short  Buffer[MC_AVERAGE_LENGTH];
} RpmFilterStructB;

//
enum{_HandleState_Stop = 0, _HandleState_Drive, _HandleState_Recovery};
typedef struct
{
    unsigned char HandleState;  // 0:Stop, 1:Drive, 2:Recovery
    //
    unsigned short HandleData_Count;
    unsigned short HandleData_Force[_RowerHandleData_Max_Extension];
    unsigned short HandleData_Distance[_RowerHandleData_Max_Extension];
    //
    unsigned long PreviousGenRPM;
}RowerDataStruct;


//--------- Private funtion ---------//
void RowerWatts_Initial(void);
void RowerWatts_ClearData(void);

void RowerWatts_Filter_Initial(void);
void RowerWatts_Filter_Update(unsigned short NewValue);
void RowerWatts_Filter_resetminmax();

void BrakeRatio_Average_Initial(unsigned char Initial);
void BrakeRatio_Average_Update(uint16_t  NewValue); //The ratio of the torque curve of the brake NOW, vs the nominal torque curve pre-programmed X128

void RowerWatts_Nmax_Filter_Initial(unsigned char Initial);//Tracks the average number of Generator signals (used, not discarded) during the return
void RowerWatts_RPM_Nmax_Filter_update(unsigned char NewValue); 

void buildtables();//Called once per console boot-up

void RowerWatts_everypole(unsigned short _capt);//Called 4 times per Generator rotation, from the Timer input capture interrupt
void GeneratorEvent(unsigned short LatestTimerValue, unsigned char _number_of_periods);//Perhaps once per Generator rotation

void update_Watts (unsigned short nowRPM); //NOTE, mark prefers this to be called ONLY when a stroke is counted (about halfway through the return)
void RowerWatts_1ms_Int(void);
void RPM_1ms_Initial(void);
unsigned short RPM_Get_WattsX10(void);
unsigned short MatrixDragFactor(void);
//
enum{ _ForceCurve_Initial, _ForceCurve_Normal};
void RowerWatts_ForceCurve(unsigned char Initial);

unsigned long RowerWatts_Parameters(unsigned char Set_Get, unsigned char Index, unsigned long Data);
//--------- Extern variables ---------//
extern RowerDataStruct RowerAction;


//------------------------------------------------------------------------------
#endif /* __ROWER_WATTS_H__ */

