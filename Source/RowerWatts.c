

/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#ifdef _SupportPhoenixConsole
#include  "Operation.h"
#include  "RowerWatts.h"
//#include  "JHT_LCB_Common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


#define FALSE 0
#define TRUE  1

// Rower data for acting
RowerDataStruct RowerAction = {0};

// Parameters
unsigned char RowerWatts_8bits_Parameters[_RowerWatts_8bits_Total] = {0};
unsigned short RowerWatts_16bits_Parameters[_RowerWatts_16bits_Total] = {0};
unsigned long RowerWatts_32bits_Parameters[_RowerWatts_32bits_Total] = {0};

// RPM
#define _RowerWatts_RPMNum    10
struct
{
    unsigned long Buff[_RowerWatts_RPMNum];
    unsigned char Counter;
}RowerWatts_RPM = {0};

// Acceleration
#define _RowerWatts_AccelNum    5
#define _RowerWatts_AccelRegisterNum   10
struct
{
    unsigned long Buff[_RowerWatts_AccelNum];
    unsigned char Counter;
    unsigned long Register[_RowerWatts_AccelRegisterNum]; // for checking from down to up, the chceking RPM needs to be calculate the force/acceleration.
    unsigned char RegisterCounter;
}RowerWatts_Accel = {0};


struct
{
    unsigned char Up;
    unsigned char Up_Count;
    unsigned char Down;
    unsigned char Down_Count;
}RowerWatts_SpeedDir = {0};

//--------------------------------------------------------------------------

RpmFilterStructA RpmFilter;     //New Filter was added Joe Yang
RpmFilterStructA NmaxFilter;    //Tracks the average number of Generator signals (used, not discarded) during the return
RpmFilterStructB BrakeRatio;    //The ratio of the torque curve of the brake NOW  * 128, vs the nominal torque curve pre-programmed

unsigned short TorqueOffsetTable[no_data];//Torque table of the system with no magnets in it
unsigned short TorqueTable[no_data];      //Nominal Torque table of the brake only, to be scaleed by DragFactor

unsigned long EnergyThisStrokeBrake;      //Energy accumulated for this stroke
unsigned long EnergyTotal_Brake;
unsigned long AverageBrakeWatts;
long BrakeWattsThisStrokeX10; //BrakeWatts can be negative for one stroke if the brake setting has just been changed by the user
unsigned long timestampPowerPrevious = 0;   /*milliseconds - free-running timer of the workout*/
unsigned long  previousFilteredRPM = 0; /*for comparing changes in kinetic energy, for power of THIS stroke */
unsigned char prevent_double_entry_to_function = FALSE;

unsigned char stroke_already_counted = FALSE;//mjk
unsigned char stroke_return_started = FALSE; //mjk
unsigned char watts_calcualtion_start = FALSE; // CW
unsigned long time_stamp_at_workoutstart = 0;//mjk?  need to have a time stamp to stamp system timer tick at the workout start
unsigned long RPM_1ms_counter = 0;
unsigned char SpeedIsDown = 0;
unsigned char SpeedUpCount = 0;
unsigned char SpeedDownCount = 0;
unsigned char RPMIsDown = 0;
unsigned char RPMUpCount = 0;
unsigned char RPMDownCount = 0;
unsigned long StrokeTime = 0;
//
struct
{
    unsigned char Nmax : 1;
    unsigned char RPM : 1;
    unsigned char BrakeRatio : 1;
    unsigned char RealFirstBrakeRatio : 1;
    unsigned char RealNmax : 1;
}RPMFirstData = {0};
//
unsigned long long MajorEnergy = 0;
unsigned long long OffsetEnergy = 0;
unsigned char NumberOfPeriodsForEnergy = 0;


//--------------------------------------------------------------------------


/* Private functions ---------------------------------------------------------*/

void RowerWatts_Initial(void)
{
    __memset(&RowerAction, 0, sizeof(RowerAction));
    RowerWatts_Filter_Initial();
    BrakeRatio_Average_Initial(1);
    RowerWatts_Nmax_Filter_Initial(1);
    buildtables();//mjk  This builds the offset and the torque table 
}

void RowerWatts_ClearData(void)
{
   RowerWatts_Filter_Initial();
   BrakeRatio_Average_Initial(0);
   RowerWatts_Nmax_Filter_Initial(0);
   //__memset(&RowerAction, 0, sizeof(RowerAction));
   RowerAction.HandleState = _HandleState_Stop;
   __memset(&RowerWatts_RPM, 0, sizeof(RowerWatts_RPM));
   __memset(&RowerWatts_Accel, 0, sizeof(RowerWatts_Accel));
   //
   __memset(&RowerWatts_SpeedDir, 0, sizeof(RowerWatts_SpeedDir));
}

/*******************************************************************************
* Function Name  : RPM_Filter_Initial
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RowerWatts_Filter_Initial(void)
{
    RPMFirstData.RPM = 0;
    //  
    RpmFilter.Length = 0;
    RpmFilter.Sum = 0;
    RpmFilter.Value = 0;
    RpmFilter.Max = 0;
    RpmFilter.Min = 0;
    //
    watts_calcualtion_start = FALSE;
}

/*******************************************************************************
* Function Name  : RPM_Nmax_Filter_Initial
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RowerWatts_Nmax_Filter_Initial(unsigned char Initial)
{
    unsigned short seed;
    
    seed = Initial == 1 ? 10 : NmaxFilter.Value;
    //
    NmaxFilter.Length =  1;
    NmaxFilter.Sum = seed;
    NmaxFilter.Value = seed;
    
    NmaxFilter.Max = seed;
    NmaxFilter.Min = seed;   
    
    //
    RPMFirstData.Nmax = 0;
    RPMFirstData.RealNmax = 0;
}

/*******************************************************************************
* Function Name  : RPM_Filter_resetminmax
* Description    : Resets the Min and Max RPM values to be the current RPM
* Input          : current RPM (typically speed of one generator rotation)
* Output         : Min and Max values are reset to the current rpm
* Return         : None
*******************************************************************************/
void RowerWatts_Filter_resetminmax(){
  RpmFilter.Max = RpmFilter.Value;
  RpmFilter.Min = RpmFilter.Value;//mjk?? Present, .Min is not used, so maybe remove it?
}


/*******************************************************************************
* Function Name  : RPM_Nmax_Filter_update
* Description    : Keeping track of the number of rpm counts during spin-down
* Input          : N of generator counts from deceleration start, to deceleration end
* Output         : Filtered, smoothed average number
* Return         : None
*******************************************************************************/
void RowerWatts_Nmax_Filter_update(unsigned char NewValue)
{
  //is 255 the maximum generator counts of a spin-down?
  //WARNING
  //mjk?? June 1
    
    if(RPMFirstData.Nmax == 0)
    {// First data is not calculated.
        RPMFirstData.Nmax = 1;
        return;
    }
    
    if(RPMFirstData.RealNmax == 0)
    {// re-update, delete old brake ratio, let new brake ratio(NewValue) as the first brake ratio
        RPMFirstData.RealNmax = 1;
        //
        NmaxFilter.Sum = 0;
        NmaxFilter.Length = 0;
        NmaxFilter.Value = 0;
        NmaxFilter.Max = 0;
        NmaxFilter.Min = 0;
    }
      
    NmaxFilter.Sum += NewValue;
    if(NmaxFilter.Length >= MC_NMAX_FILTER_LENGTH)
        NmaxFilter.Sum -= NmaxFilter.Value;
    else
        NmaxFilter.Length++;
    
    NmaxFilter.Value = NmaxFilter.Sum/NmaxFilter.Length;
    if(NewValue > NmaxFilter.Max)
        NmaxFilter.Max = NewValue;
    else if(NewValue < NmaxFilter.Min)
        NmaxFilter.Min = NewValue;
  
}

/*******************************************************************************
* Function Name  : RPM_Filter_Update
* Description    : Keeping track of the smoothed RPM, only used to track end of strokes
* Input          : new RPM, typically at a period of 1 generator rotation
* Output         : Filtered, smoothed average number
* Return         : None
*******************************************************************************/
void RowerWatts_Filter_Update(unsigned short NewValue)
{
    //RPM is x10 so it easily fits in a short, mjk June 1
    if(RPMFirstData.RPM == 0)
    {// First data is not calculated.
        RPMFirstData.RPM = 1;
        return;
    }    
      
    RpmFilter.Sum = RpmFilter.Sum + NewValue;  
    if(RpmFilter.Length >= MCGENERATOR_FILTER_LENGTH)
        RpmFilter.Sum -= RpmFilter.Value;
    else
        RpmFilter.Length++;
  
    RpmFilter.Value = RpmFilter.Sum/RpmFilter.Length;
    if(NewValue > RpmFilter.Max)
        RpmFilter.Max = NewValue;
    else if(NewValue < RpmFilter.Min)
        RpmFilter.Min = NewValue;
}




/*******************************************************************************
* Function Name  : BrakeRatio_Average_Initial
* Description    :
* Input          : starting value
* Output         : First element of the moving average array is set and the index is moved to 1
* Return         : None
*******************************************************************************/
void BrakeRatio_Average_Initial(unsigned char Initial)
{
    uint16_t  i, seed;
    
    seed = Initial == 1 ? 22 : BrakeRatio.Value;
    
    BrakeRatio.Sum = 0;
    BrakeRatio.Point = 1;     /* was 0 */
    BrakeRatio.Length = 1;    /* was MC_AVERAGE_LENGTH */
    BrakeRatio.Value = seed;
   
    for (i = 0; i < BrakeRatio.Length; i++)
    {
        BrakeRatio.Buffer[i] = seed;
        BrakeRatio.Sum += seed;
    }
    //
    RPMFirstData.RealFirstBrakeRatio = 0;
}

/*******************************************************************************
* Function Name  : BrakeRatio_Average_Update
* Description    : A moderate amount of filtering, might not be necessary
* Input          : a new value
* Output         : The average value (moving average)
* Return         : none
*******************************************************************************/
void BrakeRatio_Average_Update(uint16_t  NewValue)
{
   /* if(RPMFirstData.BrakeRatio == 0)
    {// First data is not calculated.
        RPMFirstData.BrakeRatio = 1;
        return;
    } */ 
    
    if(RPMFirstData.RealFirstBrakeRatio == 0)
    {// re-update, delete old brake ratio, let new brake ratio(NewValue) as the first brake ratio
        RPMFirstData.RealFirstBrakeRatio = 1;
        //
        BrakeRatio.Sum = 0;
        BrakeRatio.Length = 0;
        BrakeRatio.Point = 0;
        BrakeRatio.Value = 0;
    }
    
    BrakeRatio.Sum += NewValue;
    
    if(BrakeRatio.Length >= MC_AVERAGE_LENGTH)
        BrakeRatio.Sum -= BrakeRatio.Buffer[BrakeRatio.Point];
    else
        BrakeRatio.Length++;
     
    BrakeRatio.Buffer[BrakeRatio.Point] = NewValue;
    BrakeRatio.Point++;
    if(BrakeRatio.Point >= MC_AVERAGE_LENGTH) 
        BrakeRatio.Point = 0;
    BrakeRatio.Value = BrakeRatio.Sum / BrakeRatio.Length;
}


//mjk **** THIS IS TEMPORARY
#define top_data 179

//mjk
//This entire function is new
void RowerWatts_everypole(uint16_t  _capt) 
{
    /* mjk - With every generator pole (4 times per revolution)
       check to see if the desired number of poles has passed
       otherwise only continue building up the time */
  
    static uint16_t _deltaT;
    static uint8_t PoleCount;
    unsigned long PreviousRPM, TempRPM;
    unsigned long long Force;
    unsigned long FinalForce;
  
    if(watts_calcualtion_start == FALSE)
    {// First trigger, Initialize
        RPM_1ms_Initial();
        //
        watts_calcualtion_start = TRUE;
        //
        timestampPowerPrevious = RPM_1ms_counter;
        EnergyThisStrokeBrake = 0;
        BrakeWattsThisStrokeX10 = 0;
        previousFilteredRPM = 0;
        prevent_double_entry_to_function = FALSE;
        MajorEnergy = 0;
        OffsetEnergy = 0;
        StrokeTime = 0;
        //
        PoleCount = 0;
        _deltaT = 0;
        
        // Clear force data
        __memset(&RowerAction, 0, sizeof(RowerAction));
        RowerWatts_Parameters(_Set, _RowerWatts_SPM, 0);
        //
        RowerWatts_SpeedDir.Up = 1;
        RowerAction.HandleState = _HandleState_Drive;
    }
    
    
    _deltaT += _capt; /* adding up the time between generator events (one revolution, nominal) */
    PoleCount += 1;
  
    /*I cannot think of a reason for this to be ">"
    it should always be less or equal but.... to be safe? */
  
    if(PoleCount >= MCGENERATOR_SIGNAL_DIVISOR) 
    {
        /* mjk - energy accumulation algorithm is done here (nominal once per revolution of generator) or
        other depending on the signal divisor, MCGENERATOR_SIGNAL_DIVISOR,
        the energy is always counting up and the spin-down algorithm adjusts scale (brake ratio)
        for energy counting */
    
        GeneratorEvent(_deltaT, PoleCount);
    
        _deltaT = 0;
        PoleCount = 0;
        //
    }
    
    //--- Calculate generator RPM
    // Old RPM
    PreviousRPM = Operation_Average_32(RowerWatts_RPM.Buff, RowerWatts_RPM.Counter);
    //
    TempRPM = (uint32_t)MCGENERATOR_RPM_SCALE;
    TempRPM /= _capt; // generator RPM, unit:0.1RPM
    if(RowerWatts_RPM.Counter < _RowerWatts_RPMNum)
        RowerWatts_RPM.Buff[RowerWatts_RPM.Counter++] = TempRPM;
    else
        Operation_FIFO_32(RowerWatts_RPM.Buff, TempRPM, _RowerWatts_RPMNum); 
    //
    // Curernt RPM
    TempRPM = Operation_Average_32(RowerWatts_RPM.Buff, RowerWatts_RPM.Counter);
    //JHTLCBComm_SetGeneratorRPM((unsigned short)(TempRPM/10));
    //
    //--- Check speed direction
    if(TempRPM > PreviousRPM)
    {// Speed up
        RowerWatts_SpeedDir.Down_Count = 0;
        RowerWatts_SpeedDir.Up_Count++;
    }
    else if(TempRPM < PreviousRPM)
    {// Speed Down
        RowerWatts_SpeedDir.Up_Count = 0;
        RowerWatts_SpeedDir.Down_Count++;
        RowerWatts_Accel.RegisterCounter = 0;
    }
    else // TempRPM = PreviousRPM
    {
        if(RowerWatts_SpeedDir.Up_Count != 0)
            RowerWatts_SpeedDir.Up_Count++;
        else if(RowerWatts_SpeedDir.Down_Count != 0)
            RowerWatts_SpeedDir.Down_Count++;
    }
    if(RowerWatts_SpeedDir.Up_Count > INSPECT_DIRECTION_COUNT)
        RowerWatts_SpeedDir.Up_Count = INSPECT_DIRECTION_COUNT;
    if(RowerWatts_SpeedDir.Down_Count > INSPECT_DIRECTION_COUNT)
        RowerWatts_SpeedDir.Down_Count = INSPECT_DIRECTION_COUNT;
    
    if(RowerWatts_SpeedDir.Up_Count >= INSPECT_DIRECTION_COUNT)
    {
        RowerWatts_SpeedDir.Up = 1;
        RowerWatts_SpeedDir.Down = 0;
        if(RowerAction.HandleState != _HandleState_Drive)
        {
            RowerAction.HandleData_Count = 0;
            __memset(RowerAction.HandleData_Force, 0, sizeof(RowerAction.HandleData_Force));
            __memset(RowerAction.HandleData_Distance, 0, sizeof(RowerAction.HandleData_Distance));
            RowerAction.HandleState = _HandleState_Drive; // Drive
        }
    }
    else if(RowerWatts_SpeedDir.Down_Count >= INSPECT_DIRECTION_COUNT)
    {
        RowerWatts_SpeedDir.Down = 1;
        RowerWatts_SpeedDir.Up = 0;
        //
        if(RowerAction.HandleState != _HandleState_Recovery)
            RowerAction.HandleState = _HandleState_Recovery; // recovery
    }
    
    //--- Calculate the "PLUS" acceleration
    // Force = inertia x angle acceleration / radius
    // Angle acceleration = (angle velocity ^ 2 - previous angle velocity ^ 2) / 2 / radian difference
    //
    Force = 0;
    if(TempRPM > PreviousRPM)
    {
        Force = TempRPM - PreviousRPM;
        Force *= (TempRPM + PreviousRPM);
        Force = Force * MCGENERATOR_POLE_PAIR * pix100 * 100 / MCGENERATOR_BELT_RATIO / 3600 / 100 / 100;
        Force = Force * 10 * MCFLYWHEEL_J * 100 / MCPITCH_RADIUS_SPOOL_MM / MCFLYWHEEL_SPOOL_BELT_RATIO_X100 / 1000;  // 0.1 kgf
    }
    
    if(RowerWatts_Accel.Counter < _RowerWatts_AccelNum)
        RowerWatts_Accel.Buff[RowerWatts_Accel.Counter++] = (uint32_t)Force;
    else
        Operation_FIFO_32(RowerWatts_Accel.Buff, (uint32_t)Force, _RowerWatts_AccelNum);
    
    if(RowerWatts_SpeedDir.Up_Count != 0 && RowerWatts_SpeedDir.Up == 0)
    {// Direction is down, but current RPM > previous RPM
        FinalForce = Operation_Average_32(RowerWatts_Accel.Buff, RowerWatts_Accel.Counter);
        if(RowerWatts_Accel.RegisterCounter < _RowerWatts_AccelRegisterNum)
            RowerWatts_Accel.Register[RowerWatts_Accel.RegisterCounter++] = FinalForce;
        else
            Operation_FIFO_32( RowerWatts_Accel.Register, FinalForce, _RowerWatts_AccelRegisterNum);
    }

    if(RowerAction.HandleState == _HandleState_Drive)
    {
        FinalForce = Operation_Average_32(RowerWatts_Accel.Buff, RowerWatts_Accel.Counter);
        //--- Fill in the force of the checing data which is to check the direction.
        if(RowerWatts_Accel.RegisterCounter != 0)
        {
            for(int i=0; i<RowerWatts_Accel.RegisterCounter; i++)
            {
                if(RowerAction.HandleData_Count >= (uint8_t)_RowerHandleData_Max_Extension)
                {
                    Operation_FIFO_16( RowerAction.HandleData_Force, (uint16_t)RowerWatts_Accel.Register[i], (uint8_t)_RowerHandleData_Max_Extension);
                    Operation_FIFO_16( RowerAction.HandleData_Distance, _LengthPerPulseX10, (uint8_t)_RowerHandleData_Max_Extension);
                }
                else
                {
                    RowerAction.HandleData_Force[RowerAction.HandleData_Count] = RowerWatts_Accel.Register[i];
                    RowerAction.HandleData_Distance[RowerAction.HandleData_Count] = _LengthPerPulseX10;
                    RowerAction.HandleData_Count++;
                }
            }
            RowerWatts_Accel.RegisterCounter = 0;
        }

        //---
        if(RowerAction.HandleData_Count >= (uint8_t)_RowerHandleData_Max_Extension)
        {
            Operation_FIFO_16( RowerAction.HandleData_Force, (uint16_t)FinalForce, (uint8_t)_RowerHandleData_Max_Extension);
            Operation_FIFO_16( RowerAction.HandleData_Distance, _LengthPerPulseX10, (uint8_t)_RowerHandleData_Max_Extension);
        }
        else
        {
            RowerAction.HandleData_Force[RowerAction.HandleData_Count] = (uint16_t)FinalForce ;
            RowerAction.HandleData_Distance[RowerAction.HandleData_Count] = _LengthPerPulseX10;
            RowerAction.HandleData_Count++;
        }

    }
}
/*******************************************************************************
* Function Name  : update_Watts
* Description    : Calculate workout watts and This stroke Watts
* Input          : present flywheel speed
* Output         : Watts of this stroke, Watts of the workout
* Return         : none
*******************************************************************************/
void update_Watts (unsigned short nowRPM) {

  /*mjk - This is where the Watts Calculation takes place (as opposed to only accumulating energy in the counter
  mjk - it should not be called very often.  Only when you  need to update the display
  Ideally, the display would be triggered to update the watt value immediately after
  this completes */

  uint32_t overallKineticAndBrakeEnergy;

  int64_t tempW = 0; /*signed because some intermediate steps could be negative */

  uint32_t timestampPower; /* */


  if (prevent_double_entry_to_function) {
    return;
  }

  prevent_double_entry_to_function = TRUE;


#ifdef MOTEINO
  timestampPower = millis(); /*_AVR */
#else
  timestampPower = RPM_1ms_counter; /*_ARM */
#endif

  /*(1 kgf X 1 mm )/ 1 ms = 9.807 Watts
   That is the basis for this calculation below
   It means the units of Energy are in units of (kgf X mm / 100 )
   
   (meaning 99 kgf X mm is represented as 9900 inside this software in the overallKineticAndBrakeEnergy register)
   */


  /*Also include a change in max  RPM as the power for this stroke (as kinetic energy)

   watts of this stroke is = 9.8 X (energy of this stroke + conversion factor * (rpm0^2 - rpm1^2)) / (delta time)
    - the work AND the change in kinetic energy

  arranged this way to avoid dividing by 4 multiple times and to keep the register size low
  dividing RPM by 4 because otherwise these values are too big. RPM is a large integer so
  the resolution will still be OK*/

  tempW = (int64_t)previousFilteredRPM * previousFilteredRPM;  /* (already had been divided by 4) */

  previousFilteredRPM = (nowRPM / 4 ); /*for next time through this function */

  tempW = (-tempW) + (int64_t)(previousFilteredRPM * previousFilteredRPM); /* (- previous squared rpm) + (current squared rpm ) */

  /*at this point it is PLUS or MINUS resulting from (RPM0x10/4)^2 - (RPM1x10/4)^2

  NOTICE, there is an assumption that RPM will not change drastically between measurements.  If RPM between adjacent calls
  to this function changes a lot, there will be overflow
  */

  tempW = tempW * (int64_t)160 ; /*begin scalar conversion */
  tempW = tempW  / (int64_t)MCINVERSEENERGYCONVERSION_X10;

  /*consider putting limits on tempW related to overflow.  There is a practical plus/minus reasonable limit of flywheel speed between two strokes after all. */

  //tempW = tempW + EnergyThisStrokeBrake; /*add energy of the brake on this stroke */
  EnergyThisStrokeBrake = 0; /*reset the energy accumulator as soon as you can (or buffer it?) */  

  MajorEnergy = MajorEnergy * BrakeRatio.Value / 64;
  MajorEnergy += OffsetEnergy;
  MajorEnergy = MajorEnergy * MC_ROTATED_ANGLE_PERPULSE * NumberOfPeriodsForEnergy * 10;
  MajorEnergy  /= (uint32_t)MCINVERSEENERGYCONVERSION_X10;

  tempW = tempW + MajorEnergy; /*add energy of the brake on this stroke */
   
  MajorEnergy = 0;
  OffsetEnergy = 0;
  
  StrokeTime = timestampPower - timestampPowerPrevious;
  if(timestampPowerPrevious == 0)
      StrokeTime = StrokeTime*100/55; // First stroke is 55% of real stroke.
  
  if(tempW < 0) 
      tempW = 0;
  else
  {
      tempW = tempW * 981;  /*9.807, done with integers */
      tempW = tempW / 100 ; /*scale conversion */
      tempW = (tempW * MCCALIBRATION_FOR_LOSSESX100 + 5000) / 1000; // 0.1 Watt
      tempW = ((int64_t)StrokeTime/2 + tempW) / (int64_t)StrokeTime; /*Convert to Power */
  }
  
  BrakeWattsThisStrokeX10 = (uint16_t )tempW;
  RowerWatts_Parameters(_Set, _RowerWatts_SPM, ((unsigned short)((unsigned long)(60000 + (StrokeTime/2))/StrokeTime)));
  
  //
  RowerWatts_Parameters(_Set, _RowerWatts_WattsX10, BrakeWattsThisStrokeX10);
  RowerWatts_Parameters(_Set, _RowerWatts_DragFactor, MatrixDragFactor());
  
  /*
  WARNING:  values here are close to requiring (float).
  double check all values with MAXIMUM assumptions
  */

  /* next, calculating the average watts for this workout */

  overallKineticAndBrakeEnergy = (uint32_t)nowRPM * 10 / (uint32_t)MCINVERSEENERGYCONVERSION_X10;

  overallKineticAndBrakeEnergy = overallKineticAndBrakeEnergy * nowRPM;
  /*NOW includes the kinetic energy of the current state (assuming workout stared with 0 rpm) */

  /*shrink this register to go along with the other one*/

  overallKineticAndBrakeEnergy =  (overallKineticAndBrakeEnergy / MCENERGY_ACCUMULATOR_REDUCTION) + EnergyTotal_Brake;
  /*NOW includes the brake energy so far of this workout */


  /* early tuesday June 2 version 6,162
  AverageBrakeWatts = overallKineticAndBrakeEnergy / ((timestampPower - time_stamp_at_workoutstart)); //These three lines accomplish the unit conversion
  AverageBrakeWatts = 981 * AverageBrakeWatts;
  AverageBrakeWatts = AverageBrakeWatts / (100 / MCENERGY_ACCUMULATOR_REDUCTION);
  AverageBrakeWatts = AverageBrakeWatts * MCCALIBRATION_FOR_LOSSESX100 / 10000;
  */

  /*The following lines are a condensed version of the above potentially? taking advantage of bit-shift operations
    Version2, not as human-readable
  */

  /*Energy to power */
  /*     ((981/10000)/100)   ~== ((1029/8192)/128)*/

  AverageBrakeWatts = overallKineticAndBrakeEnergy / ((timestampPower - time_stamp_at_workoutstart));
  AverageBrakeWatts = 1029 * AverageBrakeWatts; /*Scale*/
  AverageBrakeWatts = AverageBrakeWatts / (128 / MCENERGY_ACCUMULATOR_REDUCTION); /* Scale up*/
  AverageBrakeWatts = (MCCALIBRATION_FOR_LOSSESX100 * AverageBrakeWatts) / 8192; /* Calibration for drive losses and un-Scale down*/


  /* wrapping up....*/

  /*reset the time stamps*/

  timestampPowerPrevious = timestampPower;

  prevent_double_entry_to_function = FALSE;

}



/*******************************************************************************
* Function Name  : GeneratorEvent
* Description    : Accumulate the amount of energy absorbed by the flywheel over N angle
* Input          : Angle of rotation of the flywheel, elapsed time, and torque curves
* Output         : Accumulates energy to overall workout and for this stroke
* Return         : none
*******************************************************************************/
unsigned long spin_down_start_RPM = 0; // for observing
unsigned long spin_down_end_RPM = 0;   // for observing
unsigned long StartDown_RPM = 0;       // for observing
void GeneratorEvent(uint16_t deltaT, uint8_t _number_of_periods)
{
  static uint16_t  LatestRPM;
  static uint16_t  Previous_RPM;
  static uint16_t  spin_down_high_RPM;

  /*This is the number of timer captures that we used (excluding what we skipped)
  it is counted from the point that the generator begins to decelerate
  until it accelerates again*/

  static uint16_t  Ndown = 0;
  static uint16_t  spin_down_start = 1; /* n of gen signal at which to START timing*/
  static uint16_t  spin_down_end = 2; /* n of gen signal to END timing (unless generator accelerates first)*/

  uint32_t newbrakeratio = 0;
  uint32_t tempbuffer = 0;

  int16_t spin_down_middle = 1;/*n at the center of desired spin-down generator signal range*/
  int16_t spin_down_span = 0;  /*n plus and minus from the middle*/

  int32_t energynow = 0;

  //uint32_t torquetable = 0;

  /*

  This function will need to track the number of occurrences
  Every time a Generator event occurs, it represents a certain number of radians of rotation.

  The difference in Generator speed between subsequent events is indicative of a certain amount
  of energy loss (kinetic energy difference)
  and we divide by the number of radians and so the output is proportional to torque

  However, the reflected mass of the Generator and the unit conversions are not taken into account
  The values are accumulated as (RPM0^2 - RPM1^2 )/ radians

  Validate the deltaT is within the range we like */

  if (deltaT == 0) {
    deltaT = 1;
  }



  /****************************************************************************

   RPM is the generator RPM, NOT the flywheel RPM
   RPM is magnified by 10X so 200RPM is a value of 2000
   To scale differently, change the GENERATOR_RPM_SCALE constant*/

  uint32_t temp;

  temp = (uint32_t)MCGENERATOR_RPM_SCALE;
  temp *= _number_of_periods;
  temp /= deltaT;
  LatestRPM = (uint16_t )temp;
  /***************************************************************************/


  /* These statements constrain the RPM to the MIN or the MAX.  Would it be better
     to leave the RPM unchanged ? */

  if (LatestRPM > (uint16_t )MCMAX_VALID_GENERATOR_SPEED) {
    LatestRPM = (uint16_t )MCMAX_VALID_GENERATOR_SPEED;
  }
  else if (LatestRPM < MCLOWEST_RPM) {
    LatestRPM = MCLOWEST_RPM;
  }


  RowerWatts_Filter_Update(LatestRPM);

  /*Find the lookup index of a particular Generator RPMx10 with appropriate rounding
    assumes the values are constrained between the valid lowest and highest RPM*/

  int16_t torque_table_index = ((LatestRPM - MCLOWEST_RPM + (MCRPM_TABLE_RESOLUTION / 2)) / MCRPM_TABLE_RESOLUTION);

  /*constrain the index to stay within the size of the array */

  if (torque_table_index >  (no_data - 1) ) {
    torque_table_index =  (no_data - 1) ;
  }
  else if (torque_table_index < 0) {
    torque_table_index = 0;
  }


  //if (Previous_RPM > LatestRPM) {
  if(RowerAction.HandleState == _HandleState_Recovery) // if(SpeedIsDown == 1)
  {
    /*Generator is DECELERATING which does NOT guarantee that there is no force on the rope/chain*/

    stroke_return_started = FALSE;


    if (!stroke_already_counted)// && (LatestRPM < RpmFilter.Value) && ((LatestRPM + MCRPM_DEADBAND) < RpmFilter.Max)) 
    {
      StartDown_RPM = LatestRPM; // for observing
      /*calculateSPM(fw_time); Currently SPM is left to be calculated by other code.  SPM does not contribute to watts */

      RowerWatts_Parameters(_Set, _RowerWatts_Strokes, RowerWatts_Parameters(_Get, _RowerWatts_Strokes, 0)+1);

      stroke_already_counted = TRUE;  /*for this stroke */

      /********************************************
      mjk??  consider: optionally, checking to see if

      EnergyThisStrokeBrake is above some minimal number
      like 100,000 for example
      before calling update_Watts()
      if the value is really low you will not get good
      resolution in the answer anyway.  This result
      can be low if a stroke is triggered incorrectly
      after too little time has passed.
	  
	  by waiting longer the results will be more accurate although
	  the user will not see a new value as often.  
	  
	     i.e.
	    if (energynow > 100000){
			update_Watts();
		}
		
	   Furthermore, if there is a way to set a flag for updating the watts later, in the loop
	   it should be done that way.  This will avoid excess computations at the point of recieving
	   a signal....?
      *********************************************/
	  
      update_Watts(LatestRPM);

      RowerWatts_Filter_resetminmax();

    }


    if (Ndown == spin_down_start) 
    {
      spin_down_high_RPM = LatestRPM;
      //
      spin_down_start_RPM = LatestRPM; // for observing
    }
    else if (Ndown == spin_down_end) 
    {
        
      int16_t rpm_at_middle = (int)((spin_down_high_RPM + LatestRPM) / 2);
      int16_t im = ((rpm_at_middle - MCLOWEST_RPM + (MCRPM_TABLE_RESOLUTION / 2)) / MCRPM_TABLE_RESOLUTION);

      spin_down_end_RPM = LatestRPM; // for observing
      //
      if (im < 0) {
        im = 0;
      } else if (im > (no_data - 1) ) {
        im = (no_data - 1) ;
      }

      /*This is where the spin-down occurs, hopefully near the top half of the deceleration rang
       except not including any point at which the user was still pulling back on the handle
       Change in Kinetic Energy DIVIDED by the angle that the Generator has rotated through
       The offset is removed before scaling the curve (per the flowchart).  It is added back in later.
      */

      /*Nominal torque at this RPM from the lookup table (includes magnetic brake curve AND the offset curve)*/
     
      //torquetable = (uint32_t) TorqueTable (im); AVR code
      
      /*Previous kinetic energy*/
      tempbuffer = (uint32_t)((uint32_t)spin_down_high_RPM * (uint32_t)spin_down_high_RPM);

      /*Present kinetic energy*/
      newbrakeratio = (uint32_t)((uint32_t)LatestRPM * (uint32_t)LatestRPM);

      /*difference in kinetic energy*/
      newbrakeratio = (uint32_t)(tempbuffer - newbrakeratio);

      /*Convert energy to torque*/
      newbrakeratio = newbrakeratio / (MC_ROTATED_ANGLE_PERPULSE * (_number_of_periods * (spin_down_end - spin_down_start)));

      /*Subtract the offset because we want to scale ONLY the amount of magnetic braking*/
      newbrakeratio = newbrakeratio - TorqueOffsetTable[im];

      /*Divide by the nominal value of this RPM*/
      newbrakeratio = (newbrakeratio * 64) / (TorqueTable[im]) ;

      /*Constrain to reasonable values, eliminates some unreasonable results*/
      if(LatestRPM < spin_down_high_RPM)
      {
          if ((newbrakeratio < MCBRAKE_RATIO_UPPER_LIMIT) && (newbrakeratio > MCBRAKE_RATIO_LOWER_LIMIT)) {
    
            /*enter this reasonable value into the moving average filter*/
            BrakeRatio_Average_Update(newbrakeratio);
          }
      }
    }
    Ndown ++;

  }/*slowing down*/
  else 
  {  /* speeding up */

    /*Generator is ACCELERATING, and that DOES guarantee that someone is tugging on the rope
      reset this particular flag*/

    stroke_already_counted = FALSE;

    if (!stroke_return_started) 
    {

      RowerWatts_Nmax_Filter_update(Ndown);/*keep track of the average number of counts from MAX Generator speed to MIN Generator speed*/

      Ndown = 0;

      /*Set up a span for computing the drag factor next time.  this should be around the middle or later of the return Plus / Minus a little bit */

      spin_down_middle = ((NmaxFilter.Value) * MCSPIN_DOWN_FRACTION) / 128;

      spin_down_span = (NmaxFilter.Value * MCBILATERAL_SPAN_SPINDOWN) / 128;

      if (spin_down_span == 0) {
        spin_down_span = 1;
      }
      
      spin_down_start = 1;
      
      spin_down_end = spin_down_middle + spin_down_span;

      if(spin_down_end <= spin_down_start)
          spin_down_end = spin_down_start + 1;
      
      stroke_return_started = TRUE;

    }
  }


  /*The offset, PLUS the curve * the drag factor  THEN times the angle rotated through

    Angle * Torque = Energy
    Angle * (Torque of system + Torque of brake) = Energy
    Power = Calibration scalar * Energy / (change in time)

  */

  /*Nominal torque at this RPM from the lookup table (includes magnetic brake curve AND the offset curve)*/
  //torquetable = (uint32_t)pgm_read_dword_near(torque_lookup + torque_table_index);//AVR code

  /*Scale the magnetic nominal torque by the brake ratio*/
  energynow = (TorqueTable[torque_table_index] * BrakeRatio.Value);
  MajorEnergy += TorqueTable[torque_table_index];
  /*divide by 64 because we multiplied the drag factor by 64*/
  energynow = energynow / 64;

  /*Add the system torque offset back in*/
  energynow = energynow + TorqueOffsetTable[torque_table_index];
  OffsetEnergy += TorqueOffsetTable[torque_table_index];
  /*Multiply torque * angle to get total energy*/
  energynow = energynow * MC_ROTATED_ANGLE_PERPULSE * (_number_of_periods) * 10;
  NumberOfPeriodsForEnergy = _number_of_periods;
  /*Convert torque table values to energy (unit scalar)*/
  energynow /= (uint32_t)MCINVERSEENERGYCONVERSION_X10; /*inverse energy conversion value is X10 to correspond with above X10 */

  /*Add energy the accumulator for this stroke*/

  EnergyThisStrokeBrake += energynow;  /*kgf *mm/100  (Meaning that if the energy is 99 kgf*mm, this quantity is 9900) */

  /*Add energy to the accumulator for the entire workout */

  /*kgf *mm/(100/MCENERGY_ACCUMULATOR_REDUCTION)  (Meaning that if the energy is 99 kgf*mm, this quantity is 9900/MCENERGY_ACCUMULATOR_REDUCTION) */

  EnergyTotal_Brake = EnergyTotal_Brake + (energynow / MCENERGY_ACCUMULATOR_REDUCTION); /*kgf *mm (reduced scale to keep in 32 bits) */

  /*now that we have verified this is not a glitch, let us record it for the next time stamp period. */
  Previous_RPM = LatestRPM;

  if (EnergyThisStrokeBrake > 2000000000) {

    /*NOTICE, this is experimental
    Normally, each stroke the energy accumulator is cleared.
    What if.... a stroke is such high power the value gets too big
    this could trigger the watt calculation to run and clear this
    ...
    
    */
  }

}




void buildtables(){
  
  //There is an OFFSET brake table and an ECB brake table
  //These could be hard-coded but since I built them from a curve fit of EXCEL
  //I found it easist to make a function to call once and build them out
  //Should these be placed into program memory?
 
    int32_t temp;
   
    for (int jj = 0; jj < no_data; jj = jj+1) {
        
        /*These values were obtained on February 10, 2016 by Mark K.  Using the JHTNA TR rower sample
         * without any magnets installed, and using the PR flywheel with 3.6 pulley ratio
        */
        
       temp = (uint32_t)(jj*MCRPM_TABLE_RESOLUTION+MCLOWEST_RPM)/MCSYSTEM_TORQUE_CURVE_INV_X1;
       TorqueOffsetTable[jj] = (uint16_t)temp + MCSYSTEM_TORQUE_CURVE_X0;
    }

    for (int jj = 0; jj < no_data; jj = jj+1) {

        int32_t rpm = jj*MCRPM_TABLE_RESOLUTION+MCLOWEST_RPM;
        
        /*These values were obtained in January 2016 and evaluated February 10 2016
          This is the shape of a torque curve, 2 notches above the lowest resistance
          The offset curve is subtracted from this curve.  This curve needs to (and does) intercept at 0,0
        */
        
        temp = rpm * MCMAGNETIC_TORQUE_CURVE_X2_NUMERATOR;
        temp = temp / MCMAGNETIC_TORQUE_CURVE_X2_DENOMINATOR_2;
        temp = rpm * temp;
        temp = temp / MCMAGNETIC_TORQUE_CURVE_X2_DENOMINATOR_2;
        
        TorqueTable[jj] = temp;

        temp = rpm * MCMAGNETIC_TORQUE_CURVE_X1_NUMERATOR;
        temp = temp / MCMAGNETIC_TORQUE_CURVE_X1_DENOMINATOR;
        
        TorqueTable[jj] += temp;
          
    }
}


void RowerWatts_1ms_Int(void)
{
    RPM_1ms_counter++;
}

void RPM_1ms_Initial(void)
{
    //mjk?? This seems to indicate the beginning of a workout
    //it resets a workout
    //note, the drag factor is not being reset because
    //we don't know if the user changed the brake
    
    RPM_1ms_counter = 0;
    time_stamp_at_workoutstart = RPM_1ms_counter;
    EnergyTotal_Brake = 0;
    EnergyThisStrokeBrake = 0;
    //
    SpeedIsDown = 0;
    SpeedUpCount = 0;
    SpeedDownCount = 0;
    //
    RPMIsDown = 0;
    RPMUpCount = 0;
    RPMDownCount = 0;
}

unsigned short RPM_Get_WattsX10(void)
{
    return BrakeWattsThisStrokeX10;/* AverageBrakeWatts */
}

unsigned short RPM_Get_AvgWatts(void)
{
    return AverageBrakeWatts;
}


unsigned long RPM_Get_StokeTime(void)
{// ms
    return StrokeTime;
}

unsigned short MatrixDragFactor(void){
    
    /* ONLY use this for display to human, if the user never shows the drag factor screen, this is never called */
    
    /* results currently fit in a 8 bit unsigned but future changes might make this change so leave as 16 bit */
    
    /* There is no direct comparison of Concept2 Drag Factor to the brake on the Matrix rower.
     In order to have a portable factor that we can use, I found something that seems close the same
     magnitude, other things being equal.
     
     Define: Drag Factor as the kgf*10 rope tension that comes from 1600mm/s rope speed.  This rope speed
     chosen because the numbers come out close to the correct magnitude.  This is based on spin-down
     tests completed on a Concept2 rower.
    */
    
    uint32_t torque = 0;
    
    /* MCDRAG_FACTOR_REFERENCE_SPEED is the closest to 1600 mm/s rope speed
    Which is the rope speed referenced for the Matrix Drag Factor calculation
    */
    
    uint16_t torque_table_index = (((uint16_t)MCDRAG_FACTOR_REFERENCE_SPEED - MCLOWEST_RPM + (MCRPM_TABLE_RESOLUTION / 2)) / MCRPM_TABLE_RESOLUTION);

    /*constrain the index to stay within the size of the array */
    
    if (torque_table_index >  (no_data - 1) ) {
      torque_table_index =  (no_data - 1) ;
    }
         
    /*Nominal torque at this RPM from the lookup table (includes magnetic brake curve AND the offset curve)*/

    torque = TorqueTable[torque_table_index];//((torquetable >> 16) & 0xFFFF); //the lookup table nominal ECB torque value */
    torque = torque * BrakeRatio.Value ;
    torque = torque / 64; /* brake Ratio ratio is scaled by 64X in the algorithm so we'll undo that here */
    torque = torque + TorqueOffsetTable[torque_table_index]; /* find the offset and nominal torque in the lookup table, compute total torque */
    torque = (torque * 100 ) / MCINVERSEENERGYCONVERSION_X10; /* convert torque values from lookup table units to kgf*mm (NOT CONFIRMED) */
   
    /*
    Refer to Machine.h for conversion calculations from torque to tension
    */
  
    torque = torque * MCCALIBRATION_FOR_LOSSESX100;  
    
    /* related to the belt and spool drive ratios (RPM of this system is based on generator RPM) */
    torque = torque * MCDRAG_FACTOR_ROPESPEED_SCALE;  
    
    torque = torque / 10000000; /* shift decimal */
    
    return torque; /* This is the drag Factor as the user would see it on a display screen */
    
}

/*******************************************************************************
* Function Name  : RPM_ForceCurve
* Description    : Fill in data to curve buffer
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RowerWatts_ForceCurve(unsigned char Initial)
{

}


//---
unsigned long RowerWatts_Parameters(unsigned char Set_Get, unsigned char Index, unsigned long Data)
{
    if(Index < _RowerWatts_8bits_End)
    {
        if(Set_Get == _Set)
            RowerWatts_8bits_Parameters[Index] = Data;
        return RowerWatts_8bits_Parameters[Index];
    }
    else if(Index < _RowerWatts_16bits_End)
    {
        if(Set_Get == _Set)
            RowerWatts_16bits_Parameters[Index - _RowerWatts_8bits_End] = Data;
        return RowerWatts_16bits_Parameters[Index - _RowerWatts_8bits_End]; 
    }
    else
    {
        if(Set_Get == _Set)
            RowerWatts_32bits_Parameters[Index - _RowerWatts_16bits_End] = Data;
        return RowerWatts_32bits_Parameters[Index - _RowerWatts_16bits_End]; 
    }
}

#endif