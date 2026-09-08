#ifndef __MACHINE_H__
#define __MACHINE_H__

//NOTE: mjk, although these are float values, they are only used locally in one function
//to build tables of integers.  it can be done another way.  example, the table could be
//hard-coded
#define MCSYSTEM_TORQUE_CURVE_X0 710       //system torque at 0 RPM
#define MCSYSTEM_TORQUE_CURVE_X1 0.3982      //Scale of system torque versus RPM
#define MCSYSTEM_TORQUE_CURVE_INV_X1 34

/*This curve was built from the results of two rower dyno tests.
 *  The first test established the no-magnet torque curve
 *  The next included some level of magnet involvement
 *    The first was subtracted from the second.
 *    A 2nd order polynomial curve fit was done in EXCEL
 *    forcing the intercept to be 0,0 (as magnets would do)
 *    The constants were converted to fractions to avoid
 *    using floating point in this code
 */

/* -2.56542E-05 */
#define MCMAGNETIC_TORQUE_CURVE_X2_NUMERATOR (-108)
#define MCMAGNETIC_TORQUE_CURVE_X2_DENOMINATOR_2 2048 //needs to be squared

/* 1.368258979 */
#define MCMAGNETIC_TORQUE_CURVE_X1_NUMERATOR  175
#define MCMAGNETIC_TORQUE_CURVE_X1_DENOMINATOR 127

/*This can be adjusted so that it represents a common user level
It relates to the MAGNETIC curves above. One can make the magnitude
of the curve NUMERATORS larger and make this smaller for a similar net result
*/
#define BRAKE_RATIO_INITIAL_VALUE 64

/*inefficiency losses; fleXonic belt and rope stretch and slip
  this also accounts for the fact that the algorithm has
  approximations and simplifications in the model */
#define MCCALIBRATION_FOR_LOSSESX100  100// 111 //120//114  


    #define MCGENERATOR_TIMER_PRESCALAR 100 //480  //1440
    #define MCCPU_CLOCK_SPEED 48000000 //72000000


#define no_data 256 /* number of elements in the torque curves */

/*
RPM values are referenced from the generator.  Because of this the RPM range
would need to change if the generator pulley ratio is changed.
*/
#define MCLOWEST_RPM  2000
#define MCRPM_TABLE_RESOLUTION 200 /* RPMx10 */

#define MCMAX_VALID_GENERATOR_SPEED (MCRPM_TABLE_RESOLUTION *(no_data-1) + MCLOWEST_RPM)

/* double check this ratio because power is proportional to speed squared */
#define MCGENERATOR_BELT_RATIO    286  //217 /* WAS 240 == 2.40 */

/* Real pulley ratio is 3.60 
   Pitch radius of the rope on the spool is 63mm
   The following two values are only meaningful in the on-screen display
   of Matrix Drag Factor
*/
#define MCFLYWHEEL_SPOOL_BELT_RATIO_X100    360
#define MCPITCH_RADIUS_SPOOL_MM             63


/* Generator has 4N and 4S poles, so a falling edge occurs 4x per rotation */
#define MCGENERATOR_POLE_PAIR     5

/*two pi radians per generator rotation*/
#define MC_ROTATED_ANGLE_PERPULSE (628 / MCGENERATOR_POLE_PAIR)

/*Interval before triggering energy accumulation event (lower for more detail
  higher for reduced noise)*/
#define MCGENERATOR_SIGNAL_DIVISOR 10  //6

/* This parameter is to inspect speed direction, up or down */
#define INSPECT_DIRECTION_COUNT   8//(25/MCGENERATOR_SIGNAL_DIVISOR)
#define INSPECT_DIRECTTON_NONCHANGE_COUNT 4//((INSPECT_DIRECTION_COUNT*75)/100)

/*the full-workout energy register had to shrink to make room for high watt long workouts*/
#define MCENERGY_ACCUMULATOR_REDUCTION   4

/*of 128, the fraction at which to center the spin-down measurment starting from the peak speed of the stroke ( 64 would be middle )*/
#define MCSPIN_DOWN_FRACTION  56

/*of a fraction of 128, but cannot be greater than (MCSPIN_DOWN_FRACTION-1)*/
#define MCBILATERAL_SPAN_SPINDOWN  26

#define MCGENERATOR_FILTER_LENGTH (MCGENERATOR_BELT_RATIO * 20 / 100)

/* 2016 02-16 M Kannel suggestion:
 *  800 mm/s rope speed is converted to generator speed in order to initialize the filtered RPM
   This speed was chosen because it is about 1/2 of the peak rope speed for a moderate stroke.
   This number can be adjusted based on experience though it is wise to leave it
   as a parametric constant in case future designs have different belt ratios

  2016 02-16 However, to avoid de-stabilizing exising code I placed 200mm/s
   into this formula in order to drive the intial RPM really low to match
   what the code had been prior to this define being changed
  
*/

#define MCGENERATOR_INITIALIZED_RPM ((uint32_t)200 * 60 / MCPITCH_RADIUS_SPOOL_MM * MCFLYWHEEL_SPOOL_BELT_RATIO_X100 / 10 * MCGENERATOR_BELT_RATIO  / pix100 /2)

/*Controls how quickly the spin down span will adjust to different stroke return times */
#define MC_NMAX_FILTER_LENGTH 5

/*NOTE: Belt ratio is X100, RPM is X10*/
#define MCRPM_DEADBAND   ( 10 * MCGENERATOR_BELT_RATIO)
#define MCGENERATOR_RPM_SCALE ((MCCPU_CLOCK_SPEED/MCGENERATOR_TIMER_PRESCALAR)/(1 * MCGENERATOR_POLE_PAIR)) * 10 * 60

#define MC_AVERAGE_LENGTH 3

#define MCBRAKE_RATIO_UPPER_LIMIT 320  /*The maximum that I think the brakeratio will get to*/

/****************************************************************************************************************
NOTE: Brake Ratio; originally this number had been a floating point and was the ratio of a nominal curve to the
 actual curve of the moment.  It turned out that 2.0 was a common ratio.  later I scaled up by x64 so a nominal
 value at a middle resistance range would be 128 and it would be 64 if the brake is set exactly where it was when
 I recorded the reference curve
 ***************************************************************************************************************/

#define MCBRAKE_RATIO_LOWER_LIMIT 1 /*The minimum that I think the brakeratio will get to (NOTICE:  this is not confirmed)*/

/*This value will represent the inertia of the flywheel and entire system, reflected AT the flywheel axis*/

#define MCFLYWHEEL_J 60517 // 63735 // 37821 /* (kgf * mm * mm) */


/*NOTICE - if you change the MCJJJ then the (uint32_t)MCINVERSEENERGYCONVERSION_X10 has to be checked and change
This calculation string is a little bit delicate*/

#define MCJJJ  (MCFLYWHEEL_J * 100 / MCGENERATOR_BELT_RATIO * 100 / MCGENERATOR_BELT_RATIO)

#define pix100 314  /*Pi*/

/*Unit conversion
1/2m *v^2
9806 is gravity constant
600 is seconds,minutes
600 is seconds, minutes
2 is (1/2)
pix100 is PI
pix100 is PI

except converted to integers and inverted, etc.  confusing, sorry!

MCJJJ * 2 * PI * 2 * PI
--------------------  unit conversion
2 * 9.8 * 60 * 60 * 10,000
 
 
2 * 9.8 * 60 * 60 * 10,000
--------------------  inverse unit conversion
MCJJJ * 2 * PI * 2 * PI

*/

/*should resolve to 1388 for 60518 flywheel inertia and 2.17 pulley ratio
*/
#define MCINVERSEENERGYCONVERSION_X10   ((uint32_t)9806 * 600  / (uint32_t)MCJJJ * 600 /2  /pix100   * 1000  /pix100)

/* evaluates to 9430 for 1.93 generator ratio, 63mm rope spool, 3.08 primary ratio
   evaluates to 11730 for 2.40 generator ratio....
   2016 02-15 Should be 12600 for 2.17, 3.60, 63
*/
#define MCDRAG_FACTOR_ROPESPEED_SCALE (((uint32_t)MCFLYWHEEL_SPOOL_BELT_RATIO_X100 * MCGENERATOR_BELT_RATIO) / MCPITCH_RADIUS_SPOOL_MM * 10) 

/*Transforms rope speed of 1600mm/s to generator RPM
   evaluated to 14415 for 1.93 pulley ratio
   evaluates to 17926 for 2.40 generator ratio
   should evaluate to around 18946
*/
#define MCDRAG_FACTOR_REFERENCE_SPEED ((uint32_t)1600 * 60 / MCPITCH_RADIUS_SPOOL_MM * MCFLYWHEEL_SPOOL_BELT_RATIO_X100 / 10 * MCGENERATOR_BELT_RATIO  / pix100 /2)


#endif