#ifndef __JHTCOMMAND_H__
#define __JHTCOMMAND_H__


//############################################################################
// Johnson RS-485/422 Communication Protocol 
//	Version: 20070511
//	Release Date: 20070720
//----------------------------------------------------------------------------
// Description:
// <UCB->LCB>
// Start(1Byte)+Address(1Byte)+Command(1Byte)+Length(1Byte)+Data(nByte)+CRC(1Byte)
// #Start:>0x00
// #Address:>0xFF
//
// <LCB->UCB>
// Start(1Byte)+Status(1Byte/4Byte)+Command(1Byte)+Length(1Byte)+Data(nByte)+CRC(1Byte)
//	#Start:>0x01
//	#Data:>nByte, 1Word:>(High Byte,Low Byte)
//
//	Timeout : 150ms
//	Rx/Tx Change Delay: 5ms
//
//
//
//
//
//----------------------------------------------------------------------------
//	Include File Version:
// Release Date: 20070831	
//############################################################################





/* ====================================================================== */
// Register Define
// for uart package
/*
  uart package
    1st          2nd             3rd        4th      5th           6th
   ____________________________________________________________________
  | start | Address / Status | Commmand | Length | Data(0~ 255  | CRC8 |
  |_______|__________________|__________|________|_byte )_______|______|
*/

// UCB
#define   UCBStartByte  		      0
#define   StartByte           		      0  	// first byte in package
#define   AddrStatusByte      		      1  	// 2nd byte in package
#define   CommandByte         	              2  	// 3rd byte in package
#define   LengthByte          		      3  	// 4th byte in package
#define   DataMsbLocation      		      4  	// 5th byte in package
#define   LengthPacketExcludeData 	      5		//EndPackageLocation   5  

// LCB
#define	  LCBStartByte			      1


// Command Define
// <Common commands>----------------------------------<UCB>-<LCB>- 
#define	  CmdInitial      		      0x70  // 	0     0			
#define   CmdGetStatus    		      0x71  //	0     0			
#define   CmdGetErrorCode 		      0x72  //	0     2			
#define   CmdGetVersion   		      0x73  //	0     2			
#define   CmdCalibrate 			      0x74  //	0     0			
#define	  CmdUpdateProgram		      0x75  //	2     1				
#define	  CmdSkipErrorCode 	              0x76  //	0     0			
#define	  CmdSpecialExtCommand		      0x77  //	1     1		
#define   CmdGetDCIVersion                    0x78  //  0     27
#define   CmdGetDCIEnvironment                0x79  //  0     84
#define	  CmdEUPsMode			      0x7A  //	1     1	      UCB-> 0: EUPs OFF,FF: EUPs ON  LCB-> 1:Command action	
#define   CmdSetConsolePower                  0x7B  //  2     0       0~65535 x 0.1Watt
#define   GetProtocolVersion                  0x7C  //  0     2       100 => 1.00
// UCB/LCB Serial Number 
#define   CmdSerialNumber                     0x7D  //  N     N       N <= 255
#define   CmdEraseSerialNumber                0x00  //  3     3
#define   CmdWriteSerialNumber                0x01  //  3+N   3       N <= 252
#define   CmdReadSerialNumber                 0x02  //  3     3+N     N <= 252
#define   CmdGetMemoryLength                  0x03  //  3     3
#define   CmdReadLCBSerialNumber              0x04  //  3     3+N     N <= 252
// for ITC serial Number
#define   CmdSetITCConsoleSN                  0x01  //  3+N   3
#define   CmdGetITCConsoleSN                  0x02  //  3     3+N
#define   CmdSetITCFrameSN                    0x05  //  3+N   3
#define   CmdGetITCFrameSN                    0x06  //  3     3+N


// < Commands of TM >
#define	  CmdSetMotorRpmAndReturn	      0xF0  //	2     2						
#define	  CmdSetMotorRpm		      0xF1  //	2     0	
#define	  CmdSetPwmAddStep		      0xF2  //	2     0	
#define	  CmdSetPwmDecStep		      0xF3  //	2     0	
#define	  CmdSetPwmStopStep		      0xF4  //	1     0	
#define   CmdSetInclineAction   	      0xF5  // 	1     0	
#define   CmdSetInclineLocation 	      0xF6  // 	2     0									
#define   CmdSetWorkStatus      	      0xF7  //	1     0						
#define   CmdGetRollerRpm       	      0xF8  //	0     2	
#define   CmdGetMotorRpm        	      0xF9  //	0     2
#define   CmdGetInclineLocation 	      0xFA  // 	0     2
#define   CmdSetGapVrCalibrateIncline 	      0xFB  // 	2     0
#define   CmdSetGapVrCalibrateIncline2 	      0xFC  //	2     0
#define   CmdSetInclineStroke  		      0xFD  //	2     0
#define   CmdSetCompensationVoltage	      0xFE  //	2     0     0~65535 x 0.01V

#define	  CmdSetDriveMotorHP		      0x90  //	2     0     0~65535 x 1Watt
#define   CmdGetMotorType                     0x91  //  0     1
#define   CmdGetMaxDataLength                 0x92  //  0     1
#define   CmdSetMotorPowerOff                 0x93  //  1     0
#define   CmdDCIForceInclineOperation         0x94  //  1     0     
#define   CmdGetTreadmillInUse                0x95  //  0     1     0xFF: Use, 0x55: not Use
#define   CmdGetMainMotorInfo                 0x96  //  0     10    0+1: Freq.(0.01Hz)
                                                    //              2+3: Current (0.1A)
                                                    //              4+5: Voltage (0.1V)
                                                    //              6+7: DC Bus Voltage (0.1V)
                                                    //              8+9: IGBT Temp (0.1C)
#define   CmdGetInclineRange                  0xA0  //  0     4
#define   CmdGetLoadCellADC                   0xA0  //  0     4     // for ICR70
#define   CmdSetInclineRange                  0xA1  //  4     0
#define   CmdGetRpmSpeedRange                 0xA6  //  0     4
#define   CmdSetRpmSpeedRange                 0xA7  //  8     0
#define   CmdUniversal                        0xAA  //  n     2
#define   CmdManualCalibration                0xAB  //  0     0



// < Commands of bike >
#define   CmdSetEcbInit     		      0x61  // 	0     0
#define   CmdSetEcbLocation 		      0x62  // 	2     0
#define   CmdGetRpm         		      0x63  //	0     2
#define   CmdGetEcbLocation 		      0x64  // 	0     2
#define   CmdSetPwm         		      0x65  //	2     0
#define	  CmdGetBatteryStatus		      0x66  //	0     2
#define	  CmdSetWatts			      0x67  //	2     0
#define   CmdSetRpmGearRatio    	      0x68  //	2     0
#define   CmdSetGenMegPolePair  	      0x69  //	1     0
#define   CmdSetLimitRpmForCharge 	      0x6A  //	2     0	
#define   CmdSetLimitRpmForResis 	      0x6B  //	2     0
#define   CmdSetMachineType  		      0x6C  //	1     0
#define   CmdSetPowerOff 		      0x6D  //	1     0
#define   CmdSetBatteryCharge  	              0x6E  //	1     0
#define   CmdSetZeroResisWhenInclineWorking   0x6F  //	1     0
	
	
#define   CmdSetInclinePercent  	      0x80  //	2     0
#define   CmdGetInclinePercent  	      0x81  //	0     2
#define	  CmdSetEcbAction		      0x82  // 	1     0
#define   EcbActionUP                         0x01
#define   EcbActionDown                       0x80
#define   EcbActionStop                       0x11
#define	  CmdGetEcbStatus		      0x83  // 	0     1
#define	  CmdGetEcbCount		      0x84  // 	0     2
#define	  CmdSetEMagnetCurrent		      0x85  //	2     0
#define	  CmdSetResistanceTypeAndResistance   0x86  //	2     0
#define	  CmdSetBeginBatteryCharge	      0x87  //  2     0     
#define   CmdGetBatteryCapacity               0x88  //  0     2      0~100%

// Climbmill
#define	  CmdSetClimbmillStatus		      0x8A  //  1     0
#define	  LCB_Workout_Start		      0x00  // 
#define	  LCB_Workout_Stop		      0x01  //
#define	  LCB_EStopAndIR_Disable	      0xA0  //              E-STOP Disable and IR Disable
#define	  LCB_EStopAndIR_Enable		      0xA1  //              E-STOP Enable and IR Enable
#define   LCB_EStopDisable_IREnable           0xA2  //              E-STOP Disable and IR Enable
#define   LCB_EStopEnable_IRDisable           0xA3  //              E-STOP Enable and IR Disable
#define	  LCB_EStop_Unlock		      0xA5  //
#define   LCB_NoUserDectectDisable            0xB0  //
#define   LCB_NoUserDectectEnable             0xB1  //
#define	  CmdGetClimbmillStatus		      0x8B  //  0     1 , EN2017 : 2
                                                // Bit 0 : the speed control ( 1: start, 0:stop)
                                                // Bit 1 : 
                                                // Bit 2 : IR Sensor use ( 0: not use, 1: use)
                                                // Bit 3 : the status of the IR Sensor 
                                                // Bit 4 : IR Sensor is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                // Bit 5 : Control Zone use ( 0: not use, 1: use)
                                                // Bit 6 : the status of the Control Zone
                                                // Bit 7 : Control Zone is once touched ( 0 : not touch , 1 : touch but not unlock)
                                                // EN 2017
                                                // Bit 8 : the status of UCB Emergency
                                                // Bit 9 : UCB Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                // Bit 10: the status of Handrail Emergency
                                                // Bit 11: Handrail Emergency is once touched ( 0 : not touch , 1 : touch but not unlock )
                                                // Bit 12: No person during using. ( 0: No, 1: Yes )
                                                // Bit 13: 
                                                // Bit 14: 
                                                // Bit 15: 
#define	  CmdSetEStopActionValue	      0x8C  //	1     0
// add 20161101
#define   CmdGetBrakePressureValue            0x8D  //  0     2
#define   CmdGetWatts                         0x8E  //  0     2
#define   CmdSetSPM                           0x4A  //  2     0   // unit: x0.1spm , ex: 10 = 1.0 spm  // Load Cell Get Samrt Rep
#define   CmdGetSPM                           0x4B  //  0     2   // unit: x0.1spm , ex: 100 = 10.0 spm

// add 20210602 ITC Load Cell command
#define   CmdGetRawData                       0x40  //  0     4   //Get Raw Data, ADC & Encoder
#define   CmdSetLoadCellOffset                0x41  //  2     0   //Set the offset for the load cell.
#define   CmdGetLoadCellOffset                0x42  //  0     2   //Get the offset for the load cell.
#define   CmdSetLoadCellScale                 0x45  //  2     0   //Set the scale of the load cell.
#define   CmdGetLoadCellScale                 0x46  //  0     2   //Get the scale of the load cell.
#define   CmdSetComplementary                 0x47  //  30    0
#define   CmdGetCalibration                   0x48  //  0     40  //Get the current in-use calibration table from the LCB.  Note, this table persists between software updates
#define   CmdFlush                            0x49  //  0     2   //Flush the load cell of any cached reps or any incoming reps prior to the flush call.

// LCB Data 0x50 Command
#define	  CmdLCBDeviceData		      0x50  //  3
#define	  GetEEPromMemorySizes		      0x01  // 	3     2
#define	  GetEEPromMemoryData		      0x02  //	3     2
#define	  GetECBCurrent			      0x03  //  3     4
#define	  GetEStopCapacitance		      0x04  //  3     4
#define   GetDCBusStatus                      0x05  //  3     4
#define   GetLCBVersion                       0x06  //  3     8
// Updata Loader
#define   LCBSoftwareReset                    0x70  //  1
#define   EraseLoaderMemory                   0x71  //  1
#define   WriteWordLoaderData                 0x72  //  6+50(Data) 1
#define   ReadByteFlashData                   0x73  //
//20210317 JIS OC MCB
#define   SavePWM                             0x80  //  4     2
                                                    // Console [80][Group][DA 2Byte], MCB[80][Status] Status:0:OK,1:Fail
#define   ReadPWM                             0x81  //  2     3
                                                    // Console [81][Group] Group 1-5, MCB[81][DA 2Byte]
// Add Athena Version Command
#define   GetAthenaVerion                     0xF0  //  1     6
                                                    //        0xF0,0x1,LCB Type,00,Formal version,Beta Version
// 20230522
#define   SetMCBParameter                     0x90  //  3+n   2
#define   GetMCBParameter                     0x91  //  2     3+n
//------------------------------------------------------------------------------
// Add 20200309 Athera
#define   GetSensorData                       0x51  //  0     Varies
#define   SetStepMotorPosition                0x52  //  4     0
#define   SetParameter                        0x53  //  1+4*N 0
#define   GetParameter                        0x54  //  0     1+4*N
#define   SetActionSpinDown                   0x55  //  1+N   1
#define   GetActionSpinDown                   0x56  //  0     15
#define   SetErgModeSettings                  0x57  //  1+4*N 0
#define   GetErgModeSettings                  0x59  //  0     1+4*N
#define   SetResistanceLevel                  0x58  //  1     0


// Machine Type Define Data
#define	  _MBikeEP_			      0
#define	  _MSetpper_			      1
#define	  _MRamp_IncluneEP_		      2
#define	  _MSwingInclineEP_		      3
#define	  _MTreadmill_			      4
#define	  _MClimbmill_			      5
#define   _MAtheraIC                          7     // Add 20200309


// Incline Manual Action
#define   ManualInclineUp                     0x01
#define   ManualInclineDown                   0xFF
#define   ManualInclineStop                   0x00

// Resistance Type Define Data
#define	  _ElectroMagnet_Device_	      0
#define	  _Resistor_Device_		      1


//------------------------------------------------------------------------------
// for 2017 RIS LCB , 
// 2021 Add for UBC(Upper Body Cycle) LCB 
#define   _RIS_START_WORD                     0xAA55    // for Memory Structure modify 0x55AA -> 0xAA55
#define   _RIS_UpBodyCycle                    3         // Add 20210413
#define   _RIS_IndoorCycle                    2
#define   _RIS_SDrive                         1
#define   _RIS_Rower                          0
// Command Define
// <Common commands>----------------------------------<UCB>-<LCB>-                                     
#define   _CmdGetLCBInfo                      0x01  //  0     4       Get LCB type and version
                                                                      // LCB Type : TODO: Create LCB type table?
                                                                      // Machine Type: This is the machine type, [0:rower, 1:s-drive, 2:indoor cycle, 3:Upper Body Cycle] 
                                                                      // LCB Version Major : This is the major version of the LCB software
                                                                      // LCB Version Minor : This is the minor version of the LCB software  
#define   _CmdGetSensorData                   0x02  //  0     Varies  This is used to manually get the sensor data in Slave mode.          
                                                                      // @@@Indoor Cycle
                                                                      // Machine Type         1 unsigned char This is the LCB Type and the value will be 2 [0 : rower, 1: s-drive, 2: indoor cycle]
                                                                      // Error Code	        2 unsigned int	This is the error code detected. 0x0000 will be return if no error occured .. 20160608 GUI: 0.0.1.2 After
                                                                      // Torque               2 unsigned int This is the raw torque value
                                                                      // RPM                  2 unsigned int This is the rpm
                                                                      // Resistance Position  2 unsigned int This is the resistance position of the resistance sensor
                                                                      // Battery Voltage      2 unsigned int This is the voltage of battery in voltage.
                                                                      // Watts                2 unsigned int Watts in .01 watts for the last stroke // 20171115 Add
                                                                      // 20180111 Add
                                                                      // Generator RPM        2 unsigned int in RPM 
                                                                      // Flywheel RPM         2 unsigned int in RPM
                                                                      // Electronics Watts    2 unsigned int 1.This watts is machine watts, which is the part which electronics watts converts to machine.2. unit: 0.01watts
                                                                      // Level                1 unsigned int This level is calculated by ECB torque, not ECB position. unit: level range: 0 - 255
                                                                      //
                                                                      // @@@S-Drive
                                                                      // Machine Type         1 unsigned char This is the LCB Type and the value will be 1 [0 : rower, 1: s-drive, 2: indoor cycle]
                                                                      // Error Code	        2 unsigned int	This is the error code detected. 0x0000 will be return if no error occured .. 20160608 GUI: 0.0.1.2 After
                                                                      // RPM                  2 unsigned int This is the rpm
                                                                      // Load Cell 1          2 unsigned int This is the load cell 1 sensor value
                                                                      // Load Cell 2          2 unsigned int This is the load cell 2 sensor value
                                                                      // Battery Voltage      2 unsigned int This is the voltage of battery in voltage.
                                                                      // Heart Rate           1 unsigned char This is the heart rate read from the 5khz board. Return 0 if not available
                                                                      // 20180111 Add
                                                                      // Generator RPM        2 unsigned int in RPM 
                                                                      //
                                                                      // @@@Rower
                                                                      // Machine Type         1 unsigned char This is the LCB type. This value will be 0 [0 : rower, 1: s-drive, 2: indoor cycle]
                                                                      // Error Code	        2 unsigned int	This is the error code detected. 0x0000 will be return if no error occured .. 20160608 GUI: 0.0.1.2 After
                                                                      // Heart Rate           1 unsigned char This is the heart rate read from the 5khz board. Return 0 if not available
                                                                      // Battery Voltage      2 unsigned char This is the voltage of battery in voltage.
                                                                      // Handle State         1 unsigned char This is the state of the rower handle [0: Stopped,1: Drive - nominally accelerating,2: Recovery - nominally decelerating]
                                                                      // Watts                2 unsigned int Watts in .1 watts for the last stroke
                                                                      // Drag Factor          2 unsigned int This is the drag factor computed for the last stroke. (TDB units)
                                                                      // Handle Data Count    1 unsigned char This is the number of handle data points pair (X * 4 bytes)
                                                                      //                        Note: the force and distance should be the actual x,y coordinate that will be used to plot the point on a
                                                                      //                        force curve and not just the amount produced in some time. So the distance is the total rope pulled and not the difference.
                                                                      // Handle Force n       2 unsigned int This is the acceleration computed in increments of .1 kgf (ie in the first 25ms period)
                                                                      // Handle Distance n    2 unsigned int This is the accumulated distance of the rope traveled in .1 mm occurred (ie in the first 25ms period.)
                                                                      // 20180111 Add
                                                                      // Generator RPM        2 unsigned int in RPM 

#define   _CmdSetDataStreamRate               0x03  //  2     0       This is used to modify the stream rate of for the Stream Sensor Data message. The default is 50ms.
#define   _CmdInitialize                      0x04  //  0     0       Initialize the LCB
#define   _CmdGetStatus                       0x05  //  0     0       This message will be used to return the status byte
#define   _CmdTurnOffBattery                  0x06  //  2     0       This message will be used to turn off the battery
#define   _CmdEnterOrCheckLCBErpMode          0x07  //  1     1       This message will be used to enter ERP mode or check the LCB ERP status
#define   _NoSuuprtErp                        0x00
#define   _IntoErp                            0x01
#define   _NotIntoErp                         0x02
#define   _WakeupOK                           0x01
// Response 
// 0x00: LCB not support ERP mode function.
// 0x01: LCB can into ERP mode
// 0x02: LCB can¡¦t into ERP mode. Please try later.
#define   _CmdGetPositionSensorCalibration    0x08  //  0     4       This message will return the min and max calibration values for the position sensor
// Min Calibration Value 2 unsigned int Minimum calibration value for the position sensor
// Max Calibration Value 2 unsigned int Maximum calibration value for the position sensor
#define   _CmdSetPositionSensorCalibration    0x09  //  4     0       This message will set the min and max calibration values for the position sensor
// Min Calibration Value 2 unsigned int Minimum calibration value for the position sensor
// Max Calibration Value 2 unsigned int Maximum calibration value for the position sensor
#define   _CmdStreamSensorData                0x12  //  N/A   Varies  This is the sensor data message sent in Master mode. The data should be the same as message 0x02 but with a different message ID to differentiate between the two messages.
#define   _CmdGetErrorCode                    0x20  //  0     2       Get the LCB most recent error code
#define   _CmdClearErrorCode                  0x21  //  0     0       Clear the LCB most recent error code
#define   _CmdEnterLCBUpdateMode              0x80  //  1     1       UCB sent 0x01 (LCB app block) to LCB. Then LCB shall return 0x01 (success) or 0x00 (fail) if the LCB supports the program update function
#define   _CmdUnknownCommand                  0x90  //  Any   1       This is return when the LCB does not know how to process a valid command
#define   _CmdSetSensorFunctionSetting        0x0A  //  2     0       Enable/disable sensor and LCB functionality.
#define   _CmdGetSensorFunctionSetting        0x0B  //  0     2       Get the sensor/function setting for message 0x0A
// Add 20180111
#define   _CmdSetCalibrateSensors             0x0C  //  2     1       Set the calibration of sensors
                                                                      /*
                                                                      UCB LCB Data
                                                                      Data Field # Bytes Type Description
                                                                      Setting 2 unsigned int
                                                                      Bit 0 ==> Calibrate torque sensor.
                                                                      Bit 1 ~ Bit15 ==> Reserved.
                                                                      LCB UCB Data
                                                                      Data Field # Bytes Type Description
                                                                      Response 1 unsigned char 0x00: No this sensor.
                                                                      0x01: Calibrate
                                                                      0x02: Can not calibrate.
                                                                      */
#define   _CmdSetConsoleWatts                 0x0D  //  2     0       Let LCB know the power consumption of currently connected console
#define   _CmdSetPowerTableParameters         0x81  //  60    0       Set the parameters of power table, which the machine is calibrated by dyno machine.
#define   _CmdGetPowerTableParameters         0x82  //  0     60      Get the parameters of power table, which the machine is calibrated by dyno machine,from LCB.
//Add 20210413
#define   _CmdSerialNumber                      0x0E  //  3~N, N=0~255  To save serial number from the console
                                                                      /*
                                                                      byte 1: handling way
                                                                                 0x00 = erase serial number, 0x01 = write, 0x02 = read, 0x03 = get max length of serial 
                                                                                 number, which LCB can save into memory.
                                                                      byte 2: handling status
                                                                                 UCB --> 0x00
                                                                                 LCB --> 0x00: success/none, 0x01: failure(EEPROM failure or other), 
                                                                                              0x02: space is not enough.
                                                                      byte 3: the length of serial number (bytes)
                                                                                 UCB --> erase(0x00): 0, write(0x01):  1~ , read(0x02): 0, max length(0x03): 0
                                                                                 LCB  --> erase: 0, write: 0, read: 1~ ( If handling status is not 0x00(success/none),
                                                                                                it is 0.), max length: 1 ~
                                                                      byte 4 - N: the data of serial number. If the length is 0, there is no data. 
                                                                      */
#define   _CmdSetFeatureAndDevice             0x0F  //  10    10      To designate the requested features and devices to LCB.
                                                                      /*
                                                                      UCB send: 
                                                                      Byte 1: Processing method?: 
                                                                                   0x00 read data, 0x01: write
                                                                      Byte 2: the watts method select. 
                                                                                  If processing method? = 0x00 (read)
                                                                                     0x00
                                                                                  If processing method? = 0x01 (write)
                                                                                     0x00: CXM watts method (no torque), (default)
                                                                                     0x01: CXP watts method (use torque)
                                                                                     0x02: CXP XR watts method(no torque but more accurate than CXM mode)
                                                                      Byte 3: the generator model select
                                                                                  If processing method? = 0x00 (read)
                                                                                     0x00
                                                                                  If processing method? = 0x01 (write)
                                                                                    0x00: Sunup G6 generator,  (default)
                                                                                    0x01: JIS generator(Temporarily called)
                                                                      Byte 4: the position sensor model select
                                                                                  If processing method? = 0x00 (read)
                                                                                     0x00
                                                                                  If processing method? = 0x01 (write)
                                                                                    0x00: KMA210 (default)
                                                                                    0x01: KMZ80
                                                                      Byte 5: the level of precision set
                                                                                  If processing method? = 0x00 (read)
                                                                                     0x00
                                                                                  If processing method? = 0x01 (write)
                                                                                    0x00: passes level in unit of [Level x 1],  (default)
                                                                                    0x01: passes level in unit of [Level x 10]
                                                                      Byte 6-10: Reserved (TBD)
                                                                                    all set 0x00
                                                                      
                                                                      LCB return: 
                                                                      Byte 1: Processing method?: (same as UCB send)
                                                                                   0x00 read data, 0x01: write
                                                                      Byte 2: the current/requested watts method
                                                                                     0x00: CXM watts method (no torque), 0x01: CXP watts method (use torque)
                                                                                     0x02 CXP XR watts method(no torque but more accurate than CXM mode), 0xFF: not supported
                                                                      Byte 3: the current/requested generator model
                                                                                    0x00:  Sunup G6 generator, 0x01: JIS generator(Temporarily called), 
                                                                                    0xFF: not supported
                                                                      Byte 4: the current/requested position sensor model
                                                                                    0x00: KMA210, 0x01: KMZ80, 
                                                                                    0xFF: not supported
                                                                      Byte 5: the current/requested level precision
                                                                                    0x00: unit = [Level x1], range: 1 - 25
                                                                                    0x01: unit = [Level x10], range: 5 - 254
                                                                                    0xFF: not supported
                                                                      Byte 6-10: Reserved (TBD)
                                                                                    all set 0x00
                                                                      */
#define   _CmdSetResistanceLevel              0x10  //  1       0  
#define   _CmdSetStepMotorPosition            0x11  //  4       0
#define   _CmdSetWattes                       0x13  //  2       0
#define   _CmdSetParameter                    0x83  //  Varies  0
#define   _CmdGetParameter                    0x84  //  0       Varies
#define   _CmdSetActionSpinDown               0x85  //  1~5     1
#define   _CmdGetActionSpinDown               0x86  //  0       19
#define   _CmdSetErgModeSettings              0x87  //  Varies  0
#define   _CmdGetErgModeSettings              0x88  //  0       Varies




#endif /* __JHTCOMMAND_H__ */
