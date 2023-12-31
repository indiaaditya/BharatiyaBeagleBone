/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ethercat.h"
#include <math.h>
//#pragma comment(lib,"ws2_32.lib") //Winsock Library

/*
#ifdef _WIN32
#include <Windows.h>
#endif
*/

typedef signed char         INT8, * PINT8;
typedef signed short        INT16, * PINT16;
typedef signed int          INT32, * PINT32;
//typedef signed __int64      INT64, * PINT64;
typedef unsigned char       UINT8, * PUINT8;
typedef unsigned short      UINT16, * PUINT16;
typedef unsigned int        UINT32, * PUINT32;
//typedef unsigned __int64    UINT64, * PUINT64;

#define stack64k (64 * 1024)

#define NSEC_PER_SEC 1000000000

#define EC_TIMEOUTMON 500

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define ENCODER_RESOLUTION_IN_BITS                              20
#define GEAR_RATIO                                              10
#define MOTOR_TORQUE_RATING                                     5.46

 //There is some confusion regarding this value.
 //Used in 2 places:
 //OSAL Sleep Cycle: Here the value works as it is
 //for calculation: the value needs to be 1/10th

//Max value at which it worked (when multiple was 10000): 8
//7: 9.22
//6: 8.02
//5: 6.59
//4: 5.51
//3: 4.41
//1: 1.51

#define SCAN_INTERVAL_IN_MSEC                                   2 //FBUS.SAMPLEPERIOD
#define SOCKET_SCAN_CYCLES                                      20

#define SOCKET_SERVER_APP_START_INIT							1
#define SOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT		2
#define SOCKET_SERVER_CREATE_SOCKET_FAILURE_ACTION				3
#define SOCKET_SERVER_SETUP_LISTENING_SOCKET					4
#define SOCKET_SERVER_LISTENING_SOCKET_SETUP_FAILURE_ACTION		5
#define SOCKET_SERVER_LISTEN_FOR_CLIENT_SOCKET					6
#define SOCKET_SERVER_LISTEN_FOR_CLIENT_SOCKET_FAILURE_ACTION	7
#define SOCKET_SERVER_ACCEPT_CLIENT_SOCKET						8
#define SOCKET_SERVER_GET_DATA_FROM_CLIENT						9
#define SOCKET_SERVER_ANALYZE_QUERY								10
#define SOCKET_SERVER_SEND_STATUS                               11
#define SOCKET_SERVER_CLOSE_SOCKET								12


#define STATE_MACHINE_STAT_UNKNOWN					0
#define STATE_MACHINE_STAT_NOT_RDY_TO_SWITCH_ON		1
#define STATE_MACHINE_STAT_SWITCH_ON_DISABLED		2
#define STATE_MACHINE_STAT_RDY_TO_SWITCH_ON			3
#define STATE_MACHINE_STAT_SWITCHED_ON				4
#define STATE_MACHINE_STAT_OPERATION_ENABLED		5
#define STATE_MACHINE_STAT_FAULT					6
#define STATE_MACHINE_STAT_FAULT_REACTION_ACTIVE	7
#define STATE_MACHINE_STAT_QUICK_STOP_ACTIVE		8
#define STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_VE 9
#define STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_INTERNAL_LIMIT_ACTIVE 10


#define CTL_WD_SHUT_DOWN							1
#define CTL_WD_SWITCH_ON							2
#define CTL_WD_DISABLE_VTG							3
#define CTL_WD_QUICK_STOP							4
#define CTL_WD_DISABLE_OPN							5
#define CTL_WD_ENABLE_OPN							6
#define CTL_WD_ENABLE_IP							7
#define CTL_WD_FAULT_RESET							8
#define CTL_WD_STOP									9
#define CTL_WD_RESET_VE 							10
#define CTL_WD_NEW_SET_POINT                        11
#define CTL_WD_PP_MODE                              12



#define BIT0		1
#define BIT1		2
#define BIT2		4
#define BIT3		8
#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80
#define BIT8		0x100
#define BIT9		0x200
#define BIT10		0x400
#define BIT11		0x800
#define BIT12		0x1000
#define BIT13		0x2000
#define BIT14		0x4000
#define BIT15		0x8000




#define	OPN_MODE_PROFILE_VELOCITY					1   //Available for Panasonic
#define	OPN_MODE_INTERPOLATED_POSN					2   //Not Available for Panasonic
#define	OPN_MODE_HOMING_MODE						3   //Available for Panasonic
#define	OPN_MODE_PROFILE_POSN						4   //
#define	OPN_MODE_PROFILE_TORQUE						5
#define	OPN_MODE_CYCLIC_SYNCH_POSN					6
//Modes added for Panasonic
#define OPN_MODE_VELOCITY                           7
#define	OPN_MODE_CYCLIC_SYNCH_VELOCITY              8
#define	OPN_MODE_CYCLIC_SYNCH_TORQUE                9


#define SERVO_OPN_EXPECTED							1
#define SERVO_OPN_NOT_EXPECTED						0

#define CMD_STAT_UNKNOWN							0
#define CMD_STAT_LOADED								1
#define CMD_STAT_ACCEPTED							2
#define CMD_STAT_COMPLETED_POSN						3
#define CMD_STAT_COMPLETED_TQ						4
#define CMD_STAT_ERR								5
#define CMD_STAT_REJECTED							6


 //Register Definitions

 //Mode setting
#define REG_DRV_OPMODE									0x35B4
#define REG_MODE_OF_OPN									0x6060

//Registers Associated with Profile Position Mode
#define REG_PROF_POSN_MODE_TARGET_POSN					0x607A	
#define REG_PROF_POSN_MODE_SW_POSN_LIMIT				0x607D
#define REG_PROF_POSN_MODE_VELOCITY						0x6081
#define REG_PROF_POSN_MODE_ACCELERATION					0x6083
#define REG_PROF_POSN_MODE_DECELRATION					0x6084


#define DRV_OPMODE_CURRENT_MODE							 0
#define DRV_OPMODE_VELOCITY_MODE						 1
#define DRV_OPMODE_POSITION_MODE						 2

//Profile Position Mode SPecific
//#define PPMNEW_SETPOINT_ENABLE								*(ec_slave[0].outputs + 1) |= (BIT4)
//#define PPMNEW_SETPOINT_ENABLE_RELEASE						*(ec_slave[0].outputs + 1) &= ~(BIT4)
//#define PPMNEW_CHANGE_SET_IMMEDIATELY						*(ec_slave[0].outputs + 1) |= (BIT5)

//#define PPMNEW_RELATIVE_MODE								*(ec_slave[0].outputs + 1) |= (BIT6)
//#define PPMNEW_ABSOLUTE_MODE								*(ec_slave[0].outputs + 1) &= ~(BIT6) 

#define CW					1	//Full Marks for being right handed
#define ACW					0	//Zero Marks for being left handed.... Indian Preconcieved notions become predefined definitions!!

#define MOTION_TIMEOUT_VAL		90000
#define BLOCK_ROTOR_DECLARE_CNT	1000	//This constant will be used for the number of samples for which the rotor consequtively has not achieved its desired position!

#define MANUFACTURER_ID     0x0000066F
#define PRODUCT_ID          0x60380008      //This ID is product specific and can be found in the specific device datasheet.

#define INPUT_OFFSET_ERRCODE                0
#define INPUT_OFFSET_STATUSWORD             2
#define INPUT_OFFSET_MODE_OF_OPN_DISP       4
#define INPUT_OFFSET_POSN_ACTUAL_VALUE      5
#define INPUT_OFFSET_VEL_ACTUAL_VALUE       9
#define INPUT_OFFSET_TQ_ACTUAL_VALUE        11


#define OUTPUT_OFFSET_CTLWD                 0
#define OUTPUT_OFFSET_MODE_OF_OPN           2
#define OUTPUT_OFFSET_TARGET_TQ             3         
#define OUTPUT_OFFSET_MAX_TQ                5
#define OUTPUT_OFFSET_TARGET_POSN           7
#define OUTPUT_OFFSET_MAX_MOTOR_SPEED       11


//For integration of the function: clock_gettime
//Source: https://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows
//Solution by: jws

#define MS_PER_SEC      1000ULL     // MS = milliseconds
#define US_PER_MS       1000ULL     // US = microseconds
#define HNS_PER_US      10ULL       // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
#define NS_PER_US       1000ULL

#define HNS_PER_SEC     (MS_PER_SEC * US_PER_MS * HNS_PER_US)
#define NS_PER_HNS      (100ULL)    // NS = nanoseconds
#define NS_PER_SEC      (MS_PER_SEC * US_PER_MS * NS_PER_US)






char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;

//IANET: 261122
volatile int rtcnt;

uint16 uiGFlag = 0;
uint32 uiLoopCntr = 0;
uint32 uiDly = 0;
uint32 uiStepsReqd = 0;

uint32 uiInitStatus = 0;

uint32 uiRdyToSwitchOn = 0;	//Bit 0
uint32 uiSwitchedOn = 0;//Bit 1
uint32 uiOpnEnabled = 0;//Bit 2
uint32 uiFault = 0;//Bit 3
uint32 uiVtgEnabled = 0;//Bit 4
uint32 uiQuickStop = 0;//Bit 5
uint32 uiSwitchOnDisable = 0;//Bit 6
uint32 uiWarning = 0;//Bit 7
uint32 uiRemote = 0;//Bit 9
uint32 uiTargetReached = 0;//Bit 10
uint32 uiInternalLimitActive = 0;//Bit 11
uint32 uiInterpolationActive = 0;//Bit 12



uint32 u32val;
uint16 slave;
uint16 wc;

//Operational Variables
int16 uiSelectedMode = 0;
uint16 uiErrorFlag = 0;
uint16 uiSetPtAck = 0;
uint16 uiDriveStatus = 0;
uint16 uiDriveEnableStat = 0;
uint16 uiFaultFlag = 0;
uint16 uiPosnSetFlag = 0;
uint8 uiDriveStatWd;
uint16 uiProfileParamSetFlag = 0;
uint32 uiInterpolationEnableFlag = 0;
uint32 uiTargetReachedFlag = 0;
uint32 uiSetMotionFlag = 0;
uint16 uiStatusResetCntr = 0;
union {
    uint16 hl;
    struct {
        uint8 l;
        uint8 h;
    }split;
}uiMaxTorque, uiStatusWd, uiTqOffset, uiTqActual;

union {
    uint32 hl;
    struct {
        uint8 ll;
        uint8 lh;
        uint8 hl;
        uint8 hh;
    }split;
}uiSecondFeedbackPosn, uiVelocity, uiVelCmdValue;

union {
    int32 hl;
    struct {
        uint8 ll;
        uint8 lh;
        uint8 hl;
        uint8 hh;
    }split;
}iPosActualValue, iDesiredPositionVal, iFinalDesiredPosnVal;

union {
    uint16 hl;
    struct {
        uint8 l;
        uint8 h;
    }split;
}uiCtlWd;

union {
    uint16 hl;
    struct {
        uint8 l;
        uint8 h;
    }split;
}iErrCode;



uint8 ui8ModesOfOpnDisplay;

int32 iBuffPosValue;
uint32 uiBlockedRotorCntr;
uint32 uiBlockedRotorFlag;

uint8 opPDO[19];


int64 i64BuffVal;

uint64 uiEncoderCntsToMove;


//Application Specific
//uint8 ccwEnable = 0;
uint16 u16DirnCntr = 0;
uint16 u16MotionTimeOutCntr = 0;

FILE* fpIn;
FILE* fpOut;
uint32 uiDesiredDegreeOfRtn;
uint32 uiDesiredDirectionOfRtn;
uint32 uiDesiredRPM;
float fDesiredTq;
uint32 uiDesiredStatus;
uint32 uiActualPosn;
uint32 uiActualTq;
int32 iFinalPosnDesired;

uint32 uiRotationOffset;

uint32 uiModifyCmdStatusFlag = 0;
uint32 ui2MsecCntr = 0;
uint32 uiCyclesCntr = 0;
uint64 ui64Cntr = 0;

uint32 ui32StepsExecuted = 0;
uint32 ui32StepsProgramed = 0;


uint32 ui32RtThreadSpeedCntr = 0;
uint32 ui32RtThreadOvFlowCntr = 0;


//Socket Server related variables
uint32 uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;

//WSADATA wsaData;



int iResult;
u_long iMode = 0;
//UINT_PTR ListenSocket = INVALID_SOCKET;	//Changed to UINT_PTR from SOCKET because compiler was throwing an error
//UINT_PTR ClientSocket = INVALID_SOCKET;

//struct addrinfo* result = NULL;
//struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

bool sktConnectedToClient = false;
uint8_t transferArr[10];

int iSocketElapsedCntr = 0;
int iMotionCompletedFlag = 0;

int getReasonForLossOfOperationalMode = 0;
int updatePrintCntr = 0;
int scanCntr = 0;

int64 gl_delta;
int64 toff;
int dorun = 0;

uint16 uiDelMeStatus = 0;
void fillMotionParams(uint32 uirDesiredPosnVal, uint16 uirTqFeedFwd, uint16 uirMaxTq);
uint16 setLimitingTorqueValue(float frDesiredTorque, float frMotorMaxTorque, uint16 uirGearRatio);
void updateStatus(uint16 uirStatus);
void printStatus(uint16 uirDriveStat);
void modifyControlWord(/*uint16 uirSlave,*/ uint16 uirDesiredStat/*, uint8 uiOffset*/);
//void setProfilePositionParameters(uint16 uirSlave, int32 irTargetPosn, uint32 uirProfileVelocity, uint32 uirProfileAcceleration, uint32 uirProfileDeceleration);
//void modifyInterpolatedPositionCommandValue(int32 irDesiredPosn, float frMaxTq, int32 irTargetVel);
void modifyLatchControlWordValue(uint16 uirLatchCtlWd/*, uint8 uirOffset*/);
void modifyTorqueFeedForwardValue(uint16 uirTqFeedFwd/*, uint8 uirOffset*/);
//void modifyDigitalOutputValue(uint16 uirDigitalOp, uint8 uirOffset);
void modifyMaxTorqueValue(uint16 uirMaxTorque/*, uint8 uirOffset*/);
void modifyMaxTorqueValueRegister(uint16 uirSlave, uint16 uirMaxTorque);
void DriveEnable();
void StopDrive();
void ShutDownDrive();
void setOutputPDO(uint16 slave, uint16 uirOutputPDO);
void setInputPDO(uint16 slave, uint16 uirInputPDO);
void setLimitValues(uint16 uirSlave);
void setDCModeSetup(uint16 uirSlave);
void setFlexibleOutputPDO(uint16 uirSlave);
void setFlexibleInputPDO(uint16 uirSlave);
unsigned char checkInterPolationDone();
void drv_SetOperationMode(uint16 slave, int32 irSelectedMode);
void resetDesiredTqAndDegOfRtn();
void GetDesiredTqAndDegOfRtn(void);
unsigned int ReadInteger(FILE* fp, unsigned int uiOffsetFromBegin);
void StoreInteger(FILE* fp, unsigned int IntegerToStore);
float ReadFloat(FILE* fp, unsigned int uiOffsetFromBegin);
void StoreFloat(FILE* fp, float floatToStore);
UINT32 ValidateInteger(UINT32* uirIntegerToValidate, UINT32 uirIntegerMinVal, UINT32 uirIntegerMaxVal, UINT32 uirFillValIfInvalid);
UINT32 ValidateFloat(float* frFloatToValidate, float frFloatMinVal, float frFloatMaxVal, float frFillValIfInvalid);
void SetActualTqAndPosn();
//void SetCommandStatus(UINT32 uirCmdStatus);
void ConvertDegreeOfRotationToCount(uint32 uirRPM, uint32 uirScanIntervalInMsec, uint32 uirDegreesToRotate);
void socketServerAction();
char** str_split(char* a_str, const char a_delim);
void SocketSendResponse(char*);
int32 calculateFinalDesiredPosn(uint32 ui32rDesRtn);
int clock_gettime_monotonic(struct timespec* tv);
void add_timespec(struct timespec* ts, int64 addtime);
void ec_sync(int64 reftime, int64 cycletime, int64* offsettime);

//Code taken from https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
//by user hmjd
/*
char** str_split(char* a_str, const char a_delim)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    // Count how many elements will be extracted. 
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    // Add space for trailing token. 
    count += last_comma < (a_str + strlen(a_str) - 1);

    // Add space for terminating null string so caller
    //   knows where the list of returned strings ends. 
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = _strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
*/

//IANET: 261122
void fillMotionParams(uint32 uirDesiredPosnVal, uint16 uirTqFeedFwd, uint16 uirMaxTq)
{
    union {
        uint32 hl;
        struct {
            uint8 ll;
            uint8 lh;
            uint8 hl;
            uint8 hh;
        }split;
    }uiDesPosnVal;

    union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiMaxTq, uiFeedFwdTq;


    uiDesPosnVal.hl = uirDesiredPosnVal;
    uiMaxTq.hl = uirMaxTq;
    uiFeedFwdTq.hl = uirTqFeedFwd;


    *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_POSN) = uiDesPosnVal.split.hh;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 1)) = uiDesPosnVal.split.hl;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 2)) = uiDesPosnVal.split.lh;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 3)) = uiDesPosnVal.split.ll;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_TQ)) = uiFeedFwdTq.split.h;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_TQ + 1)) = uiFeedFwdTq.split.l;

    *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ) = uiMaxTq.split.h;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_MAX_TQ + 1)) = uiMaxTq.split.l;

}

uint16 setLimitingTorqueValue(float frDesiredTorque, float frMotorMaxTorque, uint16 uirGearRatio)
{
    uint16 uilclValToSet = 0;
    float flclValToSet;
    float flclMaxTorque = frMotorMaxTorque * (float)uirGearRatio;
    //printf("\n flclMaxTorque: %f\n ", flclMaxTorque);
    if (frDesiredTorque <= flclMaxTorque)	//Indicates that a valid torque value has been entered
    {
        flclValToSet = (frDesiredTorque * 1000) / flclMaxTorque;
        if (ui32StepsExecuted > 2500) {
            printf("\tTQ Val Set: %f", flclValToSet);
        }
        uilclValToSet = (uint16)flclValToSet;

    }
    //printf("\n uilclValToSet: %d\n ", uilclValToSet);
    return uilclValToSet;

}

void updateStatus(uint16 uirStatus)
{
    //uint16 lclStat;
    //lclStat = 0;
    int32 statUpdated = 0;

    if ((uirStatus & (BIT6 + BIT3 + BIT2 + BIT1 + BIT0)) == 0) {
        uiDriveStatus = STATE_MACHINE_STAT_NOT_RDY_TO_SWITCH_ON;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT3 + BIT2 + BIT1 + BIT0)) == BIT6) {
        uiDriveStatus = STATE_MACHINE_STAT_SWITCH_ON_DISABLED;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT5 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT5 + BIT0)) {
        uiDriveStatus = STATE_MACHINE_STAT_RDY_TO_SWITCH_ON;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT5 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT5 + BIT1 + BIT0)) {
        uiDriveStatus = STATE_MACHINE_STAT_SWITCHED_ON;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT5 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT5 + BIT2 + BIT1 + BIT0)) {
        uiDriveStatus = STATE_MACHINE_STAT_OPERATION_ENABLED;
        statUpdated = 1;
        //printf(" OE:%x inop:%d", uirStatus, inOP);

        //Delete this section later
        uiDelMeStatus = uirStatus;
    }

    if ((uirStatus & (BIT6 + BIT5 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT2 + BIT1 + BIT0)) {
        printf("QS:%x", uirStatus);
        uiDriveStatus = STATE_MACHINE_STAT_QUICK_STOP_ACTIVE;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT5 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT3 + BIT2 + BIT1 + BIT0)) {
        uiDriveStatus = STATE_MACHINE_STAT_FAULT_REACTION_ACTIVE;
        statUpdated = 1;
    }

    if ((uirStatus & (BIT6 + BIT3 + BIT2 + BIT1 + BIT0)) == (BIT3)) {
        uiDriveStatus = STATE_MACHINE_STAT_FAULT;
        statUpdated = 1;
    }

    if (statUpdated == 0) {
        uiDriveStatus = STATE_MACHINE_STAT_UNKNOWN;
    }
















    //switch (uiLclLowerNibbleStat)
    //{
    //case 0:
    //    /*if (((uirStatus & (BIT6 + BIT4)) == (BIT6 + BIT4))) {
    //        uiDriveStatus = STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_VE;
    //    }*/
    //    //else {
    //        if ((uirStatus & BIT6) == BIT6)
    //        {
    //            /*if ((uirStatus & BIT11) == BIT11) {
    //                uiDriveStatus = STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_INTERNAL_LIMIT_ACTIVE;
    //            }
    //            else {*/
    //                uiDriveStatus = STATE_MACHINE_STAT_SWITCH_ON_DISABLED;
    //                uiOpnEnabled = 0;
    //            //}
    //        }
    //        else
    //        {
    //            uiDriveStatus = STATE_MACHINE_STAT_NOT_RDY_TO_SWITCH_ON;
    //            uiOpnEnabled = 0;
    //        }
    //    //}

    //    break;

    //case 1:
    //    if ((uirStatus & (BIT6 + BIT5)) == BIT5)
    //    {
    //        uiDriveStatus = STATE_MACHINE_STAT_RDY_TO_SWITCH_ON;
    //        uiOpnEnabled = 0;
    //    }

    //    break;

    //case 3:
    //    if ((uirStatus & (BIT6 + BIT5)) == BIT5)
    //    {
    //        uiDriveStatus = STATE_MACHINE_STAT_SWITCHED_ON;
    //        uiOpnEnabled = 0;
    //    }
    //    break;

    //case 7:
    //    if ((uirStatus & (BIT6 + BIT5)) == BIT5)
    //    {
    //        uiDriveStatus = STATE_MACHINE_STAT_OPERATION_ENABLED;
    //        uiOpnEnabled = 1;
    //    }
    //    else
    //    {

    //        if ((uirStatus & (BIT6 + BIT5 + BIT3)) == 0)
    //        {
    //            uiDriveStatus = STATE_MACHINE_STAT_QUICK_STOP_ACTIVE;
    //            uiOpnEnabled = 0;
    //        }

    //    }
    //    break;

    //case 8:
    //    if ((uirStatus & BIT6) == 0)
    //    {
    //        uiDriveStatus = STATE_MACHINE_STAT_FAULT;
    //        uiOpnEnabled = 0;
    //    }
    //    break;

    //case 0x0F:
    //    if ((uirStatus & BIT6) == 0)
    //    {
    //        uiDriveStatus = STATE_MACHINE_STAT_FAULT_REACTION_ACTIVE;
    //        uiOpnEnabled = 0;
    //    }
    //    break;

    //default:
    //    uiOpnEnabled = 0;
    //    break;
    //}


    if ((uirStatus & BIT7) == BIT7)
        uiErrorFlag = 1;
    else
        uiErrorFlag = 0;


    if ((uirStatus & BIT12) == BIT12)
        uiSetPtAck = 1;
    else
        uiSetPtAck = 0;

    if (uiInterpolationEnableFlag == 1)
    {
        if ((uirStatus & BIT12) == BIT12)
            uiInterpolationActive = 1;
        else
            uiInterpolationActive = 0;
    }

    if ((uirStatus & BIT10) == BIT10)
        uiTargetReachedFlag = 1;
    else
        uiTargetReachedFlag = 0;
}

void printStatus(uint16 uirDriveStat)
{
    unsigned int uiDesiredStat;
    if (uiErrorFlag == 1) {
        printf(" Error Detected  ");
        uiDesiredStat = CTL_WD_FAULT_RESET;
        modifyControlWord(/*slave,*/ uiDesiredStat/*, 0*/);

    }
    //printf("Stat: ");

    switch (uirDriveStat)
    {
    case STATE_MACHINE_STAT_UNKNOWN:
        //printf("Unknown               \t");
        break;
    case STATE_MACHINE_STAT_NOT_RDY_TO_SWITCH_ON:
        //printf("Not Ready to Switch On\t");
        break;
    case STATE_MACHINE_STAT_SWITCH_ON_DISABLED:
        //printf("Switch On Disabled    \t");
        break;
    case STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_INTERNAL_LIMIT_ACTIVE:
        //printf("Switch On Disabled  IntLimAct \t");
        break;
    case STATE_MACHINE_STAT_RDY_TO_SWITCH_ON:
        //printf("Ready to Switch On   \t");
        break;
    case STATE_MACHINE_STAT_SWITCHED_ON:
        //printf("Switched On          \t");
        break;
    case STATE_MACHINE_STAT_OPERATION_ENABLED:
        //printf("OE\t");
        break;
    case STATE_MACHINE_STAT_FAULT:
        //printf("Fault Detected       \t");
        break;
    case STATE_MACHINE_STAT_FAULT_REACTION_ACTIVE:
        //printf("Fault Reaction Active\t");
        break;
    case STATE_MACHINE_STAT_QUICK_STOP_ACTIVE:
        //printf("Quick Stop Active    \t");
        break;
    case STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_VE:
        //printf("Switch ON disabled with VE\t");
        break;
    default:
        //printf("Unknown              \t");
        break;
    }
    //tq:%d  , uiTqActual.hl
    scanCntr++;
    updatePrintCntr++;
    if (updatePrintCntr == 100000) {
        updatePrintCntr = 0;
        printf("PAV: %d MdOfOpn:%d status:%x Pde:%d ipa: %d SC: %d ErCd: %X\n", iPosActualValue.hl, ui8ModesOfOpnDisplay, uiStatusWd.hl, iDesiredPositionVal.hl, uiInterpolationActive, scanCntr, iErrCode.hl/*uiTargetReachedFlag*/);
    }
}

//uint8 shutdownDone = 0;

void modifyControlWord(/*uint16 uirSlave,*/ uint16 uirDesiredStat/*, uint8 uiOffset*/)
{
   
    //uiCtlWd.hl = 0;
   uint16 uiLclUpdateProfileVelocity = 0;

    switch (uirDesiredStat)
    {
    case CTL_WD_SHUT_DOWN:
        //uiCtlWd.split.h = 0;
        //uiCtlWd.split.l |= (BIT2 + BIT1);
        //uiCtlWd.split.l &= ~BIT0;
        uiCtlWd.hl = 6;
        uiOpnEnabled = 0;
        //printf("\nAA:%d", uiCtlWd.hl);
        break;
        
    case CTL_WD_SWITCH_ON:
        uiCtlWd.hl = 0x07;
        //printf("\nBB");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_DISABLE_VTG:
        //uiCtlWd.split.h = 0;
        //uiCtlWd.split.l &= ~BIT1;
        uiCtlWd.hl = 0;
        //printf("\nCC");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_QUICK_STOP:
        uiCtlWd.split.h = 0;
        uiCtlWd.split.l |= BIT1;
        uiCtlWd.split.l &= ~BIT2;
        //printf("\nDD");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_DISABLE_OPN:
        //uiCtlWd.split.h = 0;
        //uiCtlWd.split.l |= (BIT2 + BIT1 + BIT0);
        //uiCtlWd.split.l &= ~BIT3;
        //printf("\nEE");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_ENABLE_OPN:
        //        uiCtlWd.split.h = 0;
        //        uiCtlWd.split.l |= (BIT3 + BIT2 + BIT1 + BIT0);
        uiCtlWd.hl = 0x0F;
        //printf("\nFF");
        if(uiOpnEnabled == 0)
            uiOpnEnabled = 1;
        break;

    case CTL_WD_PP_MODE:
        uiCtlWd.hl = 0x1F;
        break;
    case CTL_WD_ENABLE_IP:
        uiCtlWd.split.h = 0;
        uiCtlWd.split.l |= (BIT4 + BIT3 + BIT2 + BIT1 + BIT0);
        printf("\nGG");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_FAULT_RESET:
        uiCtlWd.split.h = 0;
        uiCtlWd.split.l |= BIT7;
        //uiCtlWd.hl = 0x0F;
        //printf("\n\n\n\n\nFAULT RESET....");
        //printf("\nHH");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_STOP:
        uiCtlWd.split.h = 0;
        uiCtlWd.split.l |= BIT8;
        //printf("\nII");
        uiOpnEnabled = 0;

        break;

    case CTL_WD_NEW_SET_POINT:
        uiCtlWd.hl = (uiCtlWd.hl | BIT4);
        uiLclUpdateProfileVelocity = 1;
        //Remove the following statements later! Done for compilation only!
        if (uiLclUpdateProfileVelocity == 1)
            uiLclUpdateProfileVelocity = 0;
        //uiOpnEnabled = 1;
        break;
    default:
        //uilclModifyFlag = 0;
        //printf("\nJJ: %d", uirDesiredStat);

        break;

    }

    opPDO[OUTPUT_OFFSET_CTLWD] = uiCtlWd.split.l;
    opPDO[OUTPUT_OFFSET_CTLWD + 1] = uiCtlWd.split.h;
    opPDO[OUTPUT_OFFSET_TARGET_TQ] = 0; //            3
    opPDO[OUTPUT_OFFSET_TARGET_TQ + 1] = 0xFF; //            3
    opPDO[OUTPUT_OFFSET_MAX_TQ] = 0x00;
    opPDO[OUTPUT_OFFSET_MAX_TQ + 1] = 0xFF;
    //printf("\nOpPDO: %d,%d", opPDO[0], opPDO[1]);

    if (uiOpnEnabled < 2) {
        //printf("@");
        //ec_SDOwrite(uirSlave, 0x6040, 0x00, FALSE, sizeof(uiCtlWd.hl), &(uiCtlWd.hl), EC_TIMEOUTRXM);
    }
    if (uiOpnEnabled == 1)
        uiOpnEnabled = 2;
/*    if (uiLclUpdateProfileVelocity == 1) {
        uiLclUpdateProfileVelocity = 0;
        ec_SDOwrite(uirSlave, 0x6081, 0x00, FALSE, sizeof(uiCtlWd.hl), &(uiCtlWd.hl), EC_TIMEOUTRXM);
    }
    */
}


void setSoftwarePositionLimit(uint16 uirSlave, int32 irSW_PosnLimit1, int32 irSW_PosnLimit2) {
    //Software position limit
    uint32 uilclRetval;
    uint8 uiLclint8;
    uilclRetval = 0;

    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_SW_POSN_LIMIT, 0x01, FALSE, sizeof(irSW_PosnLimit1), &irSW_PosnLimit1, EC_TIMEOUTRXM);  //Make this and the following command 0 to disable sw position limit, refer pg. 95 of the Ethercat document
    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_SW_POSN_LIMIT, 0x02, FALSE, sizeof(irSW_PosnLimit2), &irSW_PosnLimit2, EC_TIMEOUTRXM);
    uiLclint8 = 2;
    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_SW_POSN_LIMIT, 0x00, FALSE, sizeof(uiLclint8), &uiLclint8, EC_TIMEOUTRXM);
}


/*
void setProfilePositionParameters(uint16 uirSlave, int32 irTargetPosn,uint32 uirProfileVelocity, uint32 uirProfileAcceleration, uint32 uirProfileDeceleration)
{
    uint32 uilclRetval;
    uilclRetval = 0;
    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_TARGET_POSN, 0x00, FALSE, sizeof(irTargetPosn), &irTargetPosn, EC_TIMEOUTRXM);
    //uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_VELOCITY, 0x00, FALSE, sizeof(uirProfileVelocity), &uirProfileVelocity, EC_TIMEOUTRXM);
    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_ACCELERATION, 0x00, FALSE, sizeof(uirProfileAcceleration), &uirProfileAcceleration, EC_TIMEOUTRXM);
    uilclRetval += ec_SDOwrite(uirSlave, REG_PROF_POSN_MODE_DECELRATION, 0x00, FALSE, sizeof(uirProfileDeceleration), &uirProfileDeceleration, EC_TIMEOUTRXM);
}
*/

/*
void modifyInterpolatedPositionCommandValue(int32 irDesiredPosn, float frMaxTq, int32 irTargetVel)
{

    int retval = 0;

    union {
        int32 hl;
        struct {
            uint8 ll;
            uint8 lh;
            uint8 hl;
            uint8 hh;
        }split;
    }iSplitVar;


    
        //uiInterPolTimeInSec = (uint8) uirInterpolationTimeInSec;
        //uiInterPolTimeInSec = 12;
        //iInterPolTimeIndex = 1;

        //retval += ec_SDOwrite(slave, 0x60C2, 0x01, FALSE, sizeof(uiInterPolTimeInSec), &uiInterPolTimeInSec, EC_TIMEOUTRXM);	//Interpolated Time is set
        //retval += ec_SDOwrite(slave, 0x60C2, 0x02, FALSE, sizeof(iInterPolTimeIndex), &iInterPolTimeIndex, EC_TIMEOUTRXM);	//Interpolated Time is set
        //printf("\n\n\n\n\n\n\n\n\n\n Function CALLLED!!!!!\n\n\n\n\n");
        
    uiCyclesCntr++;
    //printf("\n\n\n\n**********************************************************************MIPCV CALLED*****************************: %d\n\n\n\n\n", uiCyclesCntr);
    //printf("\tIP");
    iSplitVar.hl = irDesiredPosn;
    *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_POSN) = iSplitVar.split.ll;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 1)) = iSplitVar.split.lh;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 2)) = iSplitVar.split.hl;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 3)) = iSplitVar.split.hh;


    iSplitVar.hl = setLimitingTorqueValue(frMaxTq, (float)MOTOR_TORQUE_RATING, GEAR_RATIO);
    *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ) = iSplitVar.split.ll;
    *(ec_slave[0].outputs + (OUTPUT_OFFSET_MAX_TQ + 1)) = iSplitVar.split.lh;
}
*/
void modifyLatchControlWordValue(uint16 uirLatchCtlWd/*, uint8 uirOffset*/)
{
    union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiSplitVar;

    uiSplitVar.hl = uirLatchCtlWd;

    *(ec_slave[0].outputs + OUTPUT_OFFSET_CTLWD) = uiSplitVar.split.l;
    *(ec_slave[0].outputs + OUTPUT_OFFSET_CTLWD + 1) = uiSplitVar.split.h;
}

void modifyTorqueFeedForwardValue(uint16 uirTqFeedFwd/*, uint8 uirOffset*/)
{
    union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiSplitVar;

    uiSplitVar.hl = uirTqFeedFwd;

    *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_TQ) = uiSplitVar.split.l;
    *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_TQ + 1) = uiSplitVar.split.h;
}

/*void modifyDigitalOutputValue(uint16 uirDigitalOp, uint8 uirOffset)
{
    union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiSplitVar;

    uiSplitVar.hl = uirDigitalOp;

    *(ec_slave[0].outputs + uirOffset) = uiSplitVar.split.h;
    *(ec_slave[0].outputs + uirOffset + 1) = uiSplitVar.split.l;
}*/

void modifyMaxTorqueValue(uint16 uirMaxTorque/*, uint8 uirOffset*/)
{
    union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiSplitVar;
    uiSplitVar.hl = uirMaxTorque;

    *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ) = uiSplitVar.split.h;
    *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ + 1) = uiSplitVar.split.l;
}

void modifyMaxTorqueValueRegister(uint16 uirSlave, uint16 uirMaxTorque)
{
    uint16 uilclRetval = 0;
    uilclRetval += ec_SDOwrite(uirSlave, 0x6073, 0x00, FALSE, sizeof(uirMaxTorque), &uirMaxTorque, EC_TIMEOUTRXM);
}

void DriveEnable()
{
    uint16 uiDesiredStat = 0;
    uint32 uiLclDoNothingFlag = 0;
    //int size;
    //int retval;
    //int32 i32val;
    /*union {
        uint16 hl;
        struct {
            uint8 l;
            uint8 h;
        }split;
    }uiMaxTq;*/
    //retval = 0;
    switch (uiDriveStatus)
    {

    case STATE_MACHINE_STAT_SWITCH_ON_DISABLED:	//Nothing can be done over Ethercat to change the state!
        //printf("A");
        if (getReasonForLossOfOperationalMode == 1) {
            getReasonForLossOfOperationalMode = 0;
            printf("StatusJustBeforeLoss:%x\n", uiDelMeStatus);
           

        }
        uiDesiredStat = CTL_WD_SHUT_DOWN;
        //uiDesiredStat = CTL_WD_SWITCH_ON;
        break;
        /*case STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_INTERNAL_LIMIT_ACTIVE:
            uiMaxTq.hl = 1000; //Resolution is 0.1% so 1000 = 100%
            *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ) = uiMaxTq.split.h;
            *(ec_slave[0].outputs + (OUTPUT_OFFSET_MAX_TQ + 1)) = uiMaxTq.split.l;
            break;
            */
    case STATE_MACHINE_STAT_RDY_TO_SWITCH_ON:
        //printf("B");
        uiDesiredStat = CTL_WD_SWITCH_ON;
        break;
    case STATE_MACHINE_STAT_SWITCHED_ON:
        //printf("C");
        uiDesiredStat = CTL_WD_ENABLE_OPN;
        break;
    case STATE_MACHINE_STAT_OPERATION_ENABLED:
        printf("X");
        //uiDesiredStat = CTL_WD_PP_MODE;
        //uiLclDoNothingFlag = 1;
        //getReasonForLossOfOperationalMode = 1;
      
        /*
        uiMaxTorque.hl = 100;
        
        *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_TQ) = uiMaxTorque.split.l; //3
        *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_TQ + 1)) = uiMaxTorque.split.h; //4

        *(ec_slave[0].outputs + OUTPUT_OFFSET_MAX_TQ) = uiMaxTorque.split.l; //5
        *(ec_slave[0].outputs + (OUTPUT_OFFSET_MAX_TQ + 1)) = uiMaxTorque.split.h; //6

        *(ec_slave[0].outputs + OUTPUT_OFFSET_TARGET_POSN) = iPosActualValue.split.ll; //7
        *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 1)) = iPosActualValue.split.lh; //8
        *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 2)) = iPosActualValue.split.hl; //9
        *(ec_slave[0].outputs + (OUTPUT_OFFSET_TARGET_POSN + 3)) = iPosActualValue.split.hh; //10

        
        //Max motor speed
        *(ec_slave[0].outputs + 11) = 0;
        *(ec_slave[0].outputs + 12) = 0;
        *(ec_slave[0].outputs + 13) = 0;
        *(ec_slave[0].outputs + 14) = 0;

        //Target velocity
        *(ec_slave[0].outputs + 15) = 0;
        *(ec_slave[0].outputs + 16) = 0;
        *(ec_slave[0].outputs + 17) = 0;
        *(ec_slave[0].outputs + 18) = 0;
        
        //uiDesiredStat = CTL_WD_NEW_SET_POINT;


        */


        //Step 1: Set the target position (607A)
        //Step 2: 
        /*if (uiInterpolationEnableFlag == 1)
        {
            uiDesiredStat = CTL_WD_ENABLE_IP;
        }
        else
        {
            uiDesiredStat = CTL_WD_ENABLE_OPN;
        }*/
        break;

    case STATE_MACHINE_STAT_QUICK_STOP_ACTIVE:
    case STATE_MACHINE_STAT_NOT_RDY_TO_SWITCH_ON:	//The drive is booting up Wait!
       printf("E1");
        break;

    case STATE_MACHINE_STAT_FAULT:
        printf("F");
        uiDesiredStat = CTL_WD_FAULT_RESET;
        break;

    case STATE_MACHINE_STAT_SWITCH_ON_DISABLED_WITH_VE:
        printf("G");
        uiDesiredStat = CTL_WD_DISABLE_VTG;//CTL_WD_SWITCH_ON;//CTL_WD_RESET_VE;

        break;

    case STATE_MACHINE_STAT_UNKNOWN:
    case STATE_MACHINE_STAT_FAULT_REACTION_ACTIVE:
    default:
        printf("H");
        uiDesiredStat = CTL_WD_SWITCH_ON;
        break;
    }
    if (uiLclDoNothingFlag == 0) {
        modifyControlWord(/*slave,*/ uiDesiredStat/*, 0*/);
    }

    //*(ec_slave[0].outputs + 1) = 0;
}

void StopDrive()
{
    uint16 uiDesiredStat = CTL_WD_STOP;
    modifyControlWord(/*slave,*/ uiDesiredStat/*, 0*/);
}

void ShutDownDrive()
{
    uint16 uiDesiredStat = CTL_WD_SHUT_DOWN;
    modifyControlWord(/*slave,*/ uiDesiredStat/*, 0*/);
}

void setOutputPDO(uint16 slave, uint16 uirOutputPDO)
{
    uint8 u8val;
    uint16 u16val;
    int retval = 0;
    //Refer Pg. 36 of AKD Ethercat Communication Edition K, December 2014
    //Output Bytes in the frame are set by this
    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);	//Mapping Section is cleared
    u16val = uirOutputPDO;	//Desired Value presently is thought to be 1722
    retval += ec_SDOwrite(slave, 0x1c12, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
}

void setInputPDO(uint16 slave, uint16 uirInputPDO)
{
    //Refer Pg. 36 of AKD Ethercat Communication Edition K, December 2014
    //Input Bytes in the frame are set by this
    uint8 u8val;
    uint16 u16val;
    int retval = 0;
    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);	//Mapping Section is cleared
    u16val = uirInputPDO;
    retval += ec_SDOwrite(slave, 0x1c13, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
}


//Panasonic Specific
void setDCModeSetup(uint16 uirSlave) {
    int retval = 0;
    uint16 u16val;
    uint8 u8val;
    //uint32 u32val;
    //int32 i32val;
    //int16 i16val;
    //Refer Pg. 7 of the Ethercat document of Panasonic    
    u16val = 2;
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u32val = 2000000; //Same value has to be fed in 1C33:3
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x02, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u16val = 2;
    retval += ec_SDOwrite(uirSlave, 0x1C33, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u32val = 0;//10000000;
    retval += ec_SDOwrite(uirSlave, 0x1C33, 0x03, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u8val = 1;
    retval += ec_SDOwrite(uirSlave, 0x6060, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);


    //For putting the system in free run mode!
    /*
    u16val = 0;
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u32val = 0;
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x02, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u16val = 0;
    retval += ec_SDOwrite(uirSlave, 0x1C33, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u8val = 1;
    retval += ec_SDOwrite(uirSlave, 0x6060, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    */

    


    /*
    int size;
    u16val = 2;
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u32val = 2000000;
    retval += ec_SDOwrite(uirSlave, 0x1C32, 0x02, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u16val = 2;
    retval += ec_SDOwrite(uirSlave, 0x1C33, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

    u32val = 0;
    retval += ec_SDOwrite(uirSlave, 0x1C33, 0x03, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    */
    /* u16val = 1000;
     retval += ec_SDOwrite(uirSlave, 0x6072, 0x00, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);*/

     /*u16val = 100;
     retval += ec_SDOwrite(uirSlave, 0x3013, 0x00, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

     u16val = 100;
     retval += ec_SDOwrite(uirSlave, 0x3522, 0x00, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

     u16val = 1000;
     retval += ec_SDOwrite(uirSlave, 0x3504, 0x00, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);*/


     //default value was 2
     //i16val = 2;
     //retval += ec_SDOwrite(uirSlave, 0x605A, 0, FALSE, sizeof(i16val), &i16val, EC_TIMEOUTRXM);

     //printf("\n\n\n*************QS Option Code: %d", i16val);


     /*
     size = sizeof(i32val);

     retval += ec_SDOread(uirSlave, 0x4F33, 0, FALSE, &size, &i32val, EC_TIMEOUTRXM);

     printf("\n\n\n*************REASON: %d", i32val);
     
    i16val = 1;
    retval += ec_SDOwrite(uirSlave, 0x3799, 0, FALSE, sizeof(i16val), &i16val, EC_TIMEOUTRXM);
    */


}

void setFlexibleOutputPDO(uint16 uirSlave)
{

    //Remember: Set Values PDO cannot exceed 22 bytes
    uint8 u8val;
    uint16 u16val;
    uint32 u32val;
    int retval = 0;
    u8val = 0;

    //Mapping Section is cleared
    retval += ec_SDOwrite(uirSlave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    //Clear Output PDO's
    ec_SDOwrite(uirSlave, 0x1600, 0, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1601, 0, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1602, 0, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1603, 0, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    //Map the fields here using the following logic:
    //The meaning of the data (for example 0x60410010 in the mapping of 0x1A00 sub 1) is as follows:
    //0x6041 is the index of the DS402 status word
    //0x00 is the subindex of the DS402 status word
    //0x10 is the number of bits for this entry, i. e. 16 bits or 2 bytes.
    u32val = 0x60400010;    //Controlword
    ec_SDOwrite(uirSlave, 0x1600, 1, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x60600008;    //Modes of operation
    ec_SDOwrite(uirSlave, 0x1600, 2, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x60710010;    //Target Torque
    ec_SDOwrite(uirSlave, 0x1600, 3, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x60720010;    //Max Torque
    ec_SDOwrite(uirSlave, 0x1600, 4, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x607A0020;    //Target Position
    ec_SDOwrite(uirSlave, 0x1600, 5, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x60800020;//Max motor speed
    ec_SDOwrite(uirSlave, 0x1600, 6, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u32val = 0x60FF0020;//Target Velocity
    ec_SDOwrite(uirSlave, 0x1600, 7, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);



    //1600.0 ==> Inform the number of variables there.
    u8val = 7;//Since 1600 maps only 7 Variable
    ec_SDOwrite(uirSlave, 0x1600, 0, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);	//Since 1600 maps 7 Variables

    //Map PDO 1600 to RxPDO Assign
    //This makes the 1600 PDO mapping acitve
    //Set the number of PDO's to 1
    //1C12.0 ==> Number of assigned PDOs
    u8val = 1;
    ec_SDOwrite(uirSlave, 0x1C12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u16val = 0x1600;
    ec_SDOwrite(uirSlave, 0x1C12, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

}

void setFlexibleInputPDO(uint16 uirSlave)
{

    //Remember: Actual Values PDO cannot exceed 32 bytes
    uint8 u8val;
    uint16 u16val;
    uint32 u32val;
    int retval = 0;
    u8val = 0;

    //Mapping Section is cleared
    retval += ec_SDOwrite(uirSlave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    //Clear Input PDO's
    ec_SDOwrite(uirSlave, 0x1A00, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1A01, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1A02, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    ec_SDOwrite(uirSlave, 0x1A03, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    //Map the fields here using the following logic:
    //The meaning of the data (for example 0x60410010 in the mapping of 0x1A00 sub 1) is as follows:
    //0x6041 is the index of the DS402 status word
    //0x00 is the subindex of the DS402 status word
    //0x10 is the number of bits for this entry, i. e. 16 bits or 2 bytes.

    //Error code 603F0010h
    u32val = 0x603F0010;
    ec_SDOwrite(uirSlave, 0x1A00, 1, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    //Statusword 60410010h
    u32val = 0x60410010;
    ec_SDOwrite(uirSlave, 0x1A00, 2, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    //Modes of operation display 60610008h
    u32val = 0x60610008;
    ec_SDOwrite(uirSlave, 0x1A00, 3, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    //Position actual value 60640020h
    u32val = 0x60640020;
    ec_SDOwrite(uirSlave, 0x1A00, 4, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    //Velocity actual value 606C0020h
    u32val = 0x606C0020;
    ec_SDOwrite(uirSlave, 0x1A00, 5, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    //Torque actual value 60770010h
    u32val = 0x606C0020;
    ec_SDOwrite(uirSlave, 0x1A00, 6, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);



    //Enter the number of PDO's in each variable

    //printf("\n Enable PDO 0x1A00");
    u8val = 6;	//Since it maps 2 variables
    ec_SDOwrite(uirSlave, 0x1A00, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    //Set the number of PDO's to 
    u8val = 0x01;
    ec_SDOwrite(uirSlave, 0x1C13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u16val = 0x1A00;
    ec_SDOwrite(uirSlave, 0x1C13, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);

}

unsigned char checkInterPolationDone()
{
    unsigned char ucRetVar;
    ucRetVar = 0;

    return ucRetVar;
}

void drv_SetOperationMode(uint16 slave, int32 irSelectedMode)
{
    int32 iOperatingMode = 0;
    //uint8 u16ErrCntr = 0;
    int8 iModeOfOpnParam = 0;	//This variable value is being determined on the pg. 45 of AKD Ethercat Communication Edition K, December 2014										
    //int retval = 0;
    uint8 uiRetArr[10];
    //int l = sizeof(uiRetArr) - 1;
    memset(&uiRetArr, 0, 10);

    switch (irSelectedMode)
    {
    case OPN_MODE_PROFILE_VELOCITY:
        iModeOfOpnParam = 3;
        iOperatingMode = 1;
        break;
    case OPN_MODE_INTERPOLATED_POSN:
        iModeOfOpnParam = 7;
        iOperatingMode = 2;
        uiInterpolationEnableFlag = 1;
        break;
    case OPN_MODE_HOMING_MODE:
        iModeOfOpnParam = 6;
        iOperatingMode = 2;
        break;
    case OPN_MODE_PROFILE_POSN:
        iModeOfOpnParam = 1;
        iOperatingMode = 2;
        break;
    case OPN_MODE_PROFILE_TORQUE:
        iModeOfOpnParam = 4;
        iOperatingMode = 1;
        break;
    case OPN_MODE_CYCLIC_SYNCH_POSN:
        iModeOfOpnParam = 8;
        iOperatingMode = 1;
        break;
    case OPN_MODE_VELOCITY:
        iModeOfOpnParam = 2;
        break;
    case OPN_MODE_CYCLIC_SYNCH_VELOCITY:
        iModeOfOpnParam = 9;
        break;
    case OPN_MODE_CYCLIC_SYNCH_TORQUE:
        iModeOfOpnParam = 10;
        break;
    default:
        break;
    }
    //printf("\n\n\n\n\n\n\n\n\n\n Value of iModeOfOpnParam: %d", iModeOfOpnParam);
    printf("Operational mode: %d", iOperatingMode);

    if (iModeOfOpnParam != 0)
    {
        //REG_MODE_OF_OPN = 0x6060
        ec_slave[0].outputs[OUTPUT_OFFSET_MODE_OF_OPN] = iModeOfOpnParam;
        //retval += ec_SDOwrite(slave, REG_MODE_OF_OPN, 0x00, FALSE, sizeof(iModeOfOpnParam), &iModeOfOpnParam, EC_TIMEOUTRXM);
    }
    else
    {
        printf("FAILED TO SET THE MODE.... NOT SUPPORTED!!!");
    }
    printf("\n Slave param: %d", slave);
}

unsigned int ReadInteger(FILE* fp, unsigned int uiOffsetFromBegin)
{
    unsigned int readInt;
    fseek(fp, uiOffsetFromBegin, SEEK_SET);
    fread(&readInt, sizeof(readInt), 1, fp);
    return readInt;
}

void StoreInteger(FILE* fp, unsigned int IntegerToStore)
{
    fwrite(&IntegerToStore, sizeof(IntegerToStore), 1, fp);
}

float ReadFloat(FILE* fp, unsigned int uiOffsetFromBegin)
{
    float readFloat;
    fseek(fp, uiOffsetFromBegin, SEEK_SET);
    fread(&readFloat, sizeof(readFloat), 1, fp);
    return readFloat;
}

void StoreFloat(FILE* fp, float floatToStore)
{
    fwrite(&floatToStore, sizeof(floatToStore), 1, fp);
}


//Comment
void resetDesiredTqAndDegOfRtn() {
    uiDesiredDegreeOfRtn = 0;
    fDesiredTq = 0;
    uiDesiredStatus = 0;
    uiModifyCmdStatusFlag = 1;
    uiDesiredDirectionOfRtn = 0;
    uiDesiredRPM = 0;
    printf("Initial setting completed...\n");
}


void GetDesiredTqAndDegOfRtn()
{
    /*
    //To convert string to integer use atoi function
    //To convert string to float use atof function
    //strok to extract the string
    char cmdStr[] = "CTD";
    char cmdNoActionStr[] = "CTR";
    char** tokens;

    printf("Original String from client: %s", recvbuf);

    if (strncmp(recvbuf, cmdStr, 3) == 0) {
        printf("CTD Rcvd\n");
        tokens = str_split(recvbuf, ',');
        if (tokens) {
            int lclCntr;
            //printf("This is the no. of splits: %d", *(tokens + lclCntr));
            for (lclCntr = 0; *(tokens + lclCntr); lclCntr++) {
                switch (lclCntr) {
                case 0:
                    break;
                case 1:
                    uiDesiredDegreeOfRtn = atoi(*(tokens + lclCntr));
                    printf("uiDesiredDegreeOfRtn: %d\n", uiDesiredDegreeOfRtn);
                    break;
                case 2:
                    fDesiredTq = (float)(atof(*(tokens + lclCntr)));
                    printf("fDesiredTq : %f\n", fDesiredTq);
                    break;
                case 3:
                    uiDesiredRPM = atoi(*(tokens + lclCntr));
                    printf("uiDesiredRPM: %d\n", uiDesiredRPM);
                    break;
                case 4:
                    uiDesiredDirectionOfRtn = atoi(*(tokens + lclCntr));
                    printf("uiDesiredDirectionOfRtn: %d\n", uiDesiredDirectionOfRtn);
                    break;
                case 5:
                    uiDesiredStatus = atoi(*(tokens + lclCntr));
                    printf("uiDesiredStatus : %d\n", uiDesiredStatus);
                    break;
                default:
                    break;
                }
                free(*(tokens + lclCntr));
            }
            free(tokens);
        }
        ValidateInteger(&uiDesiredStatus, 0, 6, 0);
        if (uiDesiredStatus != 0)
            uiModifyCmdStatusFlag = 1;
    }
    else {
        if (strncmp(recvbuf, cmdNoActionStr, 3) == 0) {
            printf("...No action desired");
        }
        else {
            printf("Invalid Cmd received");
        }
    }
    */
}

void SetActualTqAndPosn()
{

    if (uiDesiredStatus == CMD_STAT_COMPLETED_POSN) {
        uiStatusResetCntr++;
        if (uiStatusResetCntr > 3) {
            uiStatusResetCntr = 0;
            uiDesiredStatus = CMD_STAT_UNKNOWN;
        }
    }

    char stringToTx[100];
    printf("\nActPosn: %d, ACT_TQ:%d, DesStat: %d", uiActualPosn, uiActualTq, uiDesiredStatus);
    sprintf(stringToTx, "STP,%d,%d,%d", uiActualPosn, uiActualTq, uiDesiredStatus);
    printf("\n Calling fn SocketSendResponse...");
    //SocketSendResponse(stringToTx);
}

/*
void SetCommandStatus(UINT32 uirCmdStatus)
{
	unsigned int uilclErrNo = 0;
	unsigned int uiPrintCntr = 0;
	char txStr[100];
	sprintf(txStr, "SCS,%d", uirCmdStatus);
	printf("\n StatUpdated: %d", uirCmdStatus);
}
*/


void ConvertDegreeOfRotationToCount(uint32 uirRPM, uint32 uirScanIntervalInMsec, uint32 uirDegreesToRotate)
{
    float fLclVar;
    float fDegreesPerScanInterval = 0;
    //Refer Sheet 2 of SOEM Calculations Sheet 2 in SOEM-Calculations folder
    //Step 1: Convert RPM to RPS
    fLclVar = (float)uirRPM / 60;
    printf("\n Step 1: %f", fLclVar);
    //Step 2: Convert RPS to Degrees in One Second
    fLclVar = fLclVar * 360;
    printf("\n Step 2: %f", fLclVar);

    //Step 3: Convert Degrees in One Second to Degrees in scan Interval
    fLclVar = fLclVar * ((float)uirScanIntervalInMsec / 1000);
    fDegreesPerScanInterval = fLclVar;
    printf("\n Step 3: %f", fLclVar);

    //Step 4: Count to be traversed in 1 Scan interval
    fLclVar = (float)(((fLclVar * pow(2, ENCODER_RESOLUTION_IN_BITS) * GEAR_RATIO)) / 360);
    uiRotationOffset = (uint32)fLclVar;
    printf("\n Step 4: %f", fLclVar);

    //Step 5: Steps required for completing the motion
    ui32StepsExecuted = 0;
    ui32StepsProgramed = (int32)((float)uirDegreesToRotate / fDegreesPerScanInterval);
    ui32StepsProgramed = ui32StepsProgramed /* + (int)(0 * (double)ui32StepsProgramed)*/;
    //Debug: Reducing for debug purposes
    //ui32StepsProgramed *= 2;
    //uiRotationOffset = uiRotationOffset / 1000;



    printf("\n********************** Steps Programed: %d", ui32StepsProgramed);
    printf("\n**************** Rotation Offset: %d", uiRotationOffset);
}



UINT32 ValidateInteger(UINT32* uirIntegerToValidate, UINT32 uirIntegerMinVal, UINT32 uirIntegerMaxVal, UINT32 uirFillValIfInvalid)
{
    UINT32 uiRetVar = 1;
    if (*uirIntegerToValidate < uirIntegerMinVal)
    {
        //*uirIntegerToValidate = uirFillValIfInvalid;
        uiRetVar = 0;
    }
    if (*uirIntegerToValidate > uirIntegerMaxVal)
    {
        //*uirIntegerToValidate = uirFillValIfInvalid;
        uiRetVar = 0;
    }
    if (uiRetVar == 0) {
        *uirIntegerToValidate = uirFillValIfInvalid;
    }
    return uiRetVar;
}


UINT32 ValidateFloat(float* frFloatToValidate, float frFloatMinVal, float frFloatMaxVal, float frFillValIfInvalid)
{
    UINT32 uiRetVar = 1;
    if (*frFloatToValidate < frFloatMinVal)
    {
        //*frFloatToValidate = frFillValIfInvalid;
        uiRetVar = 0;
    }
    if (*frFloatToValidate > frFloatMaxVal)
    {
        //*frFloatToValidate = frFillValIfInvalid;
        uiRetVar = 0;
    }

    if (uiRetVar == 0) {
        *frFloatToValidate = frFillValIfInvalid;
    }
    return uiRetVar;
}

int32 calculateFinalDesiredPosn(uint32 ui32rDesRtn) {
    int32 retVal;
    printf("Total Steps:%d", ui32StepsProgramed);
    printf("Rotation Offset:%d", uiRotationOffset);

    uint32 uLclTotalSteps = ui32StepsProgramed * uiRotationOffset;
    if (ui32rDesRtn == CW) {
        retVal = iPosActualValue.hl - uLclTotalSteps;
    }
    else {
        retVal = iPosActualValue.hl + uLclTotalSteps;
    }

    return retVal;
}


int PanasonicSetup(uint16 slave) {
    int retval;
    //uint8 u8val;

    //uint32 uiRet32Arr[10];
    printf("\n This is the slave parameter: %d", slave);
    //int l32 = sizeof(uiRet32Arr) - 1;
    //memset(&uiRet32Arr, 0, 10);
    retval = 0;
    //u8val = 0;
    setDCModeSetup(slave);
    setSoftwarePositionLimit(slave, 0, 0);
    setFlexibleOutputPDO(slave);
    setFlexibleInputPDO(slave);


    while (EcatError) printf("%s", ec_elist2string());

    printf("Panasonic Servo slave %d set, retval = %d\n", slave, retval);
    return 1;
}


/* most basic RT thread for process data, just does IO transfer */
OSAL_THREAD_FUNC_RT RTthread(void* ptr)
{//1
    //int32 i32PosnDiff = 0;
    /*	union{
            int32 hl;
            struct{
                uint8 ll;
                uint8 lh;
                uint8 hl;
                uint8 hh;
            }split;
        }iSplitVar;
    */
    struct timespec   ts, tleft;
    int ht;
    int64 cycletime;
    //For debug only
    tleft.tv_nsec = 1000;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    //clock_gettime_monotonic(&ts);
    ht = (ts.tv_nsec / 1000000) + 1; /* round to nearest ms */
    ts.tv_nsec = ht * 1000000;
    if (ts.tv_nsec >= NSEC_PER_SEC) {
        ts.tv_sec++;
        ts.tv_nsec -= NSEC_PER_SEC;
    }
    cycletime = *(int*)ptr * 1000; /* cycletime in ns */
    printf("\ncycletime:%lld", cycletime);
    printf("\n ht: %d", ht);
    printf("\n ts.tv_nsec: %ld", ts.tv_nsec);

    toff = 0;
    dorun = 0;
    ec_send_processdata();
    while (1) {
        /* calculate next cycle start */
        //add_timespec(&ts, cycletime + toff);
        /* wait to cycle start */
        //printf("\ndr:%d", dorun);
        //clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
        if (dorun > 0) {
            IOmap[0]++;
            dorun++;
            //printf("ctlwd: %d", uiCtlWd.hl);
            ec_slave[0].outputs = opPDO;

            //printf("\nAXSASSA\n");
            //Set the mode of operation initially itself
            opPDO[OUTPUT_OFFSET_MODE_OF_OPN] = 1;//OPN_MODE_PROFILE_POSN;


            wkc = ec_receive_processdata(EC_TIMEOUTRET);
            if (ec_slave[0].hasdc)
            {
                /* calulate toff to get linux time and DC synced */
                ec_sync(ec_DCtime, cycletime, &toff);
            }
            else {
                printf("n");
            }
            ec_send_processdata();
            rtcnt++;

            /* do RT control stuff here */

            iErrCode.split.l = *(ec_slave[0].inputs + INPUT_OFFSET_ERRCODE);
            iErrCode.split.h = *(ec_slave[0].inputs + (INPUT_OFFSET_ERRCODE + 1));

            uiStatusWd.split.l = *(ec_slave[0].inputs + INPUT_OFFSET_STATUSWORD);
            uiStatusWd.split.h = *(ec_slave[0].inputs + (INPUT_OFFSET_STATUSWORD + 1));
            updateStatus(uiStatusWd.hl);

            iPosActualValue.split.ll = *(ec_slave[0].inputs + INPUT_OFFSET_POSN_ACTUAL_VALUE);
            iPosActualValue.split.lh = *(ec_slave[0].inputs + INPUT_OFFSET_POSN_ACTUAL_VALUE + 1);
            iPosActualValue.split.hl = *(ec_slave[0].inputs + INPUT_OFFSET_POSN_ACTUAL_VALUE + 2);
            iPosActualValue.split.hh = *(ec_slave[0].inputs + INPUT_OFFSET_POSN_ACTUAL_VALUE + 3);

            ui8ModesOfOpnDisplay = *(ec_slave[0].inputs + INPUT_OFFSET_MODE_OF_OPN_DISP);

            uiTqActual.split.l = *(ec_slave[0].inputs + INPUT_OFFSET_TQ_ACTUAL_VALUE);
            uiTqActual.split.h = *(ec_slave[0].inputs + INPUT_OFFSET_TQ_ACTUAL_VALUE + 1);

            ui32RtThreadSpeedCntr++;
        }
   





        if (inOP == TRUE)	//indicates that servo is in operational state
        {//2
            DriveEnable();
        }//\2

        
        //This line should be at the bottom of the while loop
        uiLoopCntr++;

        osal_usleep(SCAN_INTERVAL_IN_MSEC * 1000);

    }

   /* else//If the drive is not enabled then set the desired value of position as actual value so as not to start the motor on enable!
    {
        iDesiredPositionVal.hl = iPosActualValue.hl ;
        modifyInterpolatedPositionCommandValue(iDesiredPositionVal.hl, 10, 5);
    }*/

    //Commenting //3 section for a trial.

    //if (uiDriveStatus == STATE_MACHINE_STAT_OPERATION_ENABLED)
    //{//3
    //    if (uiSetMotionFlag == 0)
    //    {//4						
    //        if (uiInterpolationActive == 1)
    //        {//5
    //            switch (uiDesiredStatus)
    //            {//6
    //            case CMD_STAT_LOADED:
    //                //printf("\n CMD_STAT_LOADED \n CMD_STAT_LOADED \n CMD_STAT_LOADED");
    //                printf("\nRtn:%d,Des.Tq:%f,dirn:%d,rpm:%d", uiDesiredDegreeOfRtn, fDesiredTq, uiDesiredDirectionOfRtn, uiDesiredRPM);
    //                if (ValidateInteger(&uiDesiredDegreeOfRtn, 1, 14400, 1) == 1)	//Valid Degree of Rtn Entered !!!3600 should be 2520
    //                {//7
    //                    if (ValidateFloat(&fDesiredTq, 1, 50, 1) == 1)	//Valid Torque Value is entered
    //                    {//8								
    //                        if (ValidateInteger(&uiDesiredDirectionOfRtn, 0, 1, 0) == 1)	//Valid Direction of Rotation Has Been Entered
    //                        {//9	
    //                            if (ValidateInteger(&uiDesiredRPM, 5, 30, 10) == 1)
    //                            {//10
    //                                //If the function reaches this point then it indicates that all the parameters have been entered correctly!!											
    //                                uiDesiredStatus = CMD_STAT_ACCEPTED;
    //                                uiModifyCmdStatusFlag = 1;
    //                                printf("\n Command Accepted!");
    //                                //Fill up the desired Position Value
    //                                ConvertDegreeOfRotationToCount(uiDesiredRPM, SCAN_INTERVAL_IN_MSEC / 10, uiDesiredDegreeOfRtn);
    //                                iFinalPosnDesired = calculateFinalDesiredPosn(uiDesiredDirectionOfRtn);
    //                                //!printf("\n PAV Used: %d, Rotation Offset: %d",iPosActualValue.hl, uiRotationOffset);
    //                                //!printf("\n Setting the Desired Status!!!");
    //                                //modifyInterpolatedPositionCommandValue(iDesiredPositionVal.hl,fDesiredTq,5);	
    //                                uiSetMotionFlag = 1;
    //                                //Set the desired value once here.
    //                                /*if (uiDesiredDirectionOfRtn == CW) {
    //                                    iDesiredPositionVal.hl = iPosActualValue.hl - uiRotationOffset;
    //                                }
    //                                else {
    //                                    iDesiredPositionVal.hl = iPosActualValue.hl + uiRotationOffset;
    //                                }*/
    //                                iDesiredPositionVal.hl = iPosActualValue.hl;

    //                                iMotionCompletedFlag = 0;

    //                                u16DirnCntr = 0;
    //                                uiBlockedRotorCntr = 0;
    //                                u16MotionTimeOutCntr = 0;
    //                                ui2MsecCntr = 0;
    //                                ui64Cntr = 0;
    //                                //printf("\n And I wanted the motion!!");
    //                            }//\10
    //                            else {
    //                                uiDesiredStatus = CMD_STAT_REJECTED;
    //                                printf("*****REJECTED*****");
    //                                uiModifyCmdStatusFlag = 1;
    //                            }
    //                        }//\9
    //                        else {
    //                            uiDesiredStatus = CMD_STAT_REJECTED;
    //                            uiModifyCmdStatusFlag = 1;
    //                        }
    //                    }//\8
    //                    else {
    //                        uiDesiredStatus = CMD_STAT_REJECTED;
    //                        uiModifyCmdStatusFlag = 1;
    //                    }
    //                }//\7
    //                else {
    //                    uiDesiredStatus = CMD_STAT_REJECTED;
    //                    uiModifyCmdStatusFlag = 1;
    //                }
    //                break;

    //            case CMD_STAT_ACCEPTED:	//If Cmd Stat is accepted then set motion flag has to be 1. If not reset it... this is an error condition!!!
    //                uiDesiredStatus = CMD_STAT_UNKNOWN;
    //                uiModifyCmdStatusFlag = 1;
    //                //SetCommandStatus(CMD_STAT_UNKNOWN);
    //                break;

    //            default:
    //                break;
    //            }//\6
    //        }//\5				
    //    }//\4


    //    //This section of code was implemented to check that the RtThread really runs every 2 msec.
    //    /*
    //    if(ui32RtThreadSpeedCntr >= 1000)
    //    {
    //        printf("\n\n\n\n TICK TOCK TICK TOCK: %d - %d\n\n\n",ui32RtThreadSpeedCntr,ui32RtThreadOvFlowCntr);
    //        ui32RtThreadSpeedCntr = 0;
    //        ui32RtThreadOvFlowCntr++;
    //        if(ui32RtThreadOvFlowCntr >= 100)
    //        {
    //            ui32RtThreadOvFlowCntr = 0;
    //            printf("\n\n\n\n OVERFLOW DETECTED!!!!\nOVERFLOW DETECTED!!!!\nOVERFLOW DETECTED!!!!\n\n\n");
    //        }
    //    }
    //    */

    //    if (uiSetMotionFlag == 1)
    //    {//4				
    //        u16MotionTimeOutCntr++;
    //        iBuffPosValue = iDesiredPositionVal.hl;

    //        if (uiDesiredDirectionOfRtn == CW) {
    //            iDesiredPositionVal.hl = iDesiredPositionVal.hl - uiRotationOffset;
    //        }
    //        else {
    //            iDesiredPositionVal.hl = iDesiredPositionVal.hl + uiRotationOffset;
    //        }
    //        if (ui32StepsExecuted > 2500) {
    //            printf("\n DP: %d", iDesiredPositionVal.hl);
    //        }

    //        //Blocked Rotor Detection Logic Start
    //        /*
    //        i32PosnDiff = iBuffPosValue - iPosActualValue.hl;
    //        if (i32PosnDiff < 0) i32PosnDiff *= -1;
    //        printf("\nPos Diff: %d", i32PosnDiff);
    //        if (i32PosnDiff > 100000) { //Indicates very small movement was achieved
    //            uiBlockedRotorCntr++;
    //            printf("\n blocked rotor: %d", uiBlockedRotorCntr);
    //        }
    //        else
    //            uiBlockedRotorCntr = 0;

    //        if (uiBlockedRotorCntr > BLOCK_ROTOR_DECLARE_CNT) {
    //            ui32StepsExecuted = ui32StepsProgramed + 100; //This effectively says that the job is done if rotor is locked!
    //        }
    //        */
    //        //Blocked Rotor Detection Logic End
    //        ui32StepsExecuted++;
    //        modifyInterpolatedPositionCommandValue(iDesiredPositionVal.hl, fDesiredTq, 5);
    //        if (ui32StepsExecuted > 2500) {
    //            printf("\tFP: %d", iFinalPosnDesired);
    //            printf("\tSteps Ex: %d", ui32StepsExecuted);
    //            printf("\tAV:%ld", iPosActualValue.hl);
    //        }

    //        if (uiDesiredDirectionOfRtn == CW) {
    //            if (iPosActualValue.hl <= iFinalPosnDesired) {
    //                iMotionCompletedFlag = 1;
    //                printf("\nACSCSCSCSC AV: %d FP: %d", iPosActualValue.hl, iFinalPosnDesired);
    //            }
    //        }
    //        else {
    //            if (iPosActualValue.hl >= iFinalPosnDesired) {
    //                iMotionCompletedFlag = 1;
    //                printf("\nBBBBCSCSCSCSC AV: %d FP: %d", iPosActualValue.hl, iFinalPosnDesired);
    //            }
    //        }

    //        //inidicates that the motion is complete
    //        //if(ui32StepsExecuted >= 2000)
    //        //if ((ui32StepsExecuted >= (ui32StepsProgramed)))
    //        if ((ui32StepsExecuted >= (ui32StepsProgramed)))
    //        {
    //            printf("\nSE: %d", ui32StepsExecuted);
    //            iMotionCompletedFlag = 1;
    //            printf("\nXXXZZZCSCSCSCSC");
    //        }

    //        if (iMotionCompletedFlag == 1) {
    //            uiDesiredStatus = CMD_STAT_COMPLETED_POSN;
    //            uiStatusResetCntr = 0;
    //            uiModifyCmdStatusFlag = 1;
    //            uiSetMotionFlag = 0;
    //            ui2MsecCntr = 0;
    //            ui32StepsProgramed = 0;
    //            ui32StepsExecuted = 0;
    //            iMotionCompletedFlag = 0;
    //            modifyInterpolatedPositionCommandValue(iDesiredPositionVal.hl, 0, 5);
    //        }
    //        ui64Cntr++;

    //        if (u16MotionTimeOutCntr > MOTION_TIMEOUT_VAL)	//Indicates that Motion Has Timed Out
    //        {//5
    //            iDesiredPositionVal.hl = iPosActualValue.hl;
    //            modifyInterpolatedPositionCommandValue(iDesiredPositionVal.hl, 0, 5);
    //            //while(1){
    //            printf("\n\n\n\n\nTIME OUTTTTTTTTTTTT\n\n\n\n");
    //            //}
    //            uiSetMotionFlag = 0;
    //            u16MotionTimeOutCntr = 0;
    //            StopDrive();
    //            ui2MsecCntr = 0;
    //        }//\5


    //        //i32PosnDiff = iPosActualValue.hl - iDesiredPositionVal.hl;
    //        //if(i32PosnDiff < 0) i32PosnDiff *= -1;	//Convert negative value to +ve

    //    }//\4


    //}//\3
    
}//\1


void simpletest(char* ifname)
{
    //int j;
    int32 i, oloop, iloop, chk, slc;
    //int32 chkCntr, wkc_count;
    //uint32 mmResult;
    //chkCntr = 0;
    needlf = FALSE;
    inOP = FALSE;
  //  int size;
 //   int32 i32val;
//    int16 i16val;

    uint32 t;
    uint64 t1;

    printf("Starting simple test\n");
    resetDesiredTqAndDegOfRtn();

    /* initialise SOEM, bind socket to ifname */
    if (ec_init(ifname))
    {
        printf("ec_init on %s succeeded.\n", ifname);
        /* find and auto-config slaves */

        
        if (ec_config_init(FALSE) > 0)
        {
            printf("%d slaves found and configured.\n", ec_slavecount);

            if ((ec_slavecount >= 1))	//Earlier it was only >1, thus this loop was not getting executed!
            {
                for (slc = 1; slc <= ec_slavecount; slc++)
                {

                    //Panasonic Ethercat Servo Motor
                    if ((ec_slave[slc].eep_man == MANUFACTURER_ID) && (ec_slave[slc].eep_id == PRODUCT_ID))
                    {
                        printf("It gives us great pleasure to inform you that Panasonic Ethercat Servo has been found!!!\n");
                        printf("Found %s at position %d\n", ec_slave[slc].name, slc);
                        // link slave specific setup to preop->safeop hook
                        ec_slave[slc].PO2SOconfig = &PanasonicSetup;
                    }
                }
            }
            else
            {
                printf("Could not enter the if loop\n");
            }
            ec_config_map(&IOmap);
            ec_configdc();
            //ec_dcsync0(1, TRUE, 2000000U, 2000U);

            printf("Slaves mapped, state to SAFE_OP.\n");
           
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

            t1 = 0;
            /*
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSYSTIME, sizeof(t1), &t1, EC_TIMEOUTRET);
            printf("\nSystem time of Slave: %lld", t1);
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSYSDELAY, sizeof(t), &t, EC_TIMEOUTRET);
            printf("\nDelay time of Slave: %d", t);
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSYSOFFSET, sizeof(t1), &t1, EC_TIMEOUTRET);
            printf("\nOffset time of Slave: %lld", t1);
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSOF, sizeof(t1), &t1, EC_TIMEOUTRET);
            printf("\nLocal time of Slave: %lld", t1);
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSYSDIFF, sizeof(t), &t, EC_TIMEOUTRET);
            printf("\nSystem time difference of slave: %d", t);
            wc = ec_FPRD(ec_slave[1].configadr, ECT_REG_DCSTART0, sizeof(t1), &t1, EC_TIMEOUTRET);
            printf("\nNext sync0 start time of slave: %lld\n", t1);
            */






            oloop = ec_slave[0].Obytes;
            if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
            if (oloop > 8) oloop = 8;//Temporarily commented by ASA to find the actual number of bytes in response
            iloop = ec_slave[0].Ibytes;

            if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
            if (iloop > 8) iloop = 8;

            printf("segments : %d : %d %d %d %d\n", ec_group[0].nsegments, ec_group[0].IOsegment[0], ec_group[0].IOsegment[1], ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);

            printf("Request operational state for all slaves\n");
            printf("oloop: %d, iloop: %d \n", oloop, iloop);
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);

            /* start RT thread as periodic MM timer */
            //timeBeginPeriod(1);
               /* create RT thread */
            osal_thread_create_rt(&thread1, stack64k * 2, &RTthread, (void*)&ctime);
            printf("\n*********Thread Created************");
            //mmResult = timeSetEvent(1, 0, RTthread, 0, TIME_PERIODIC);
          

            /* request OP state for all slaves */
            ec_writestate(0);
            chk = 40;
            /* activate cyclic process data */
            dorun = 1;
            /* wait for all slaves to reach OP state */
            do
            {
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
            if (ec_slave[0].state == EC_STATE_OPERATIONAL)
            {
                printf("Operational state reached for all slaves.\n");
                //wkc_count = 0;
                inOP = TRUE;


                /* cyclic loop, reads data from RT thread */
                for (i = 1; i <= 15000; i++)
                {
                    i--;    //To run the loop infinitely.

                    printf("Processdata cycle %5d , Wck %3d, DCtime %12"PRId64", dt %12"PRId64", O:",
                        dorun, wkc, ec_DCtime, gl_delta);

                    iSocketElapsedCntr++;
                    if ((iSocketElapsedCntr > SOCKET_SCAN_CYCLES) && (uiSetMotionFlag == 0))
                    {
                        iSocketElapsedCntr = 0;
                        //printf("SS:%d", );
                        //printf("\nA");
                        //socketServerAction();
                    }

                    
                    if (wkc >= expectedWKC)
                    {
                        needlf = TRUE;
                        uiActualPosn = iPosActualValue.hl;
                        uiActualTq = uiTqActual.hl;
                        printStatus(uiDriveStatus);
                        
                        /*if (uiModifyCmdStatusFlag != 0)
                        {
                            SetCommandStatus(uiDesiredStatus);
                            uiModifyCmdStatusFlag = 0;
                        }*/
                    }
                    printStatus(uiDriveStatus);

                    osal_usleep(500000);    //Sleep for 0.5 Seconds
                }
                dorun = 0;
                inOP = FALSE;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for (i = 1; i <= ec_slavecount; i++)
                {
                    if (ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }

           /*
            j = 0;

            while (j == 0)
            {
                StopDrive();
                uiStatusWd.split.l = *(ec_slave[0].inputs + INPUT_OFFSET_STATUSWORD);
                uiStatusWd.split.h = *(ec_slave[0].inputs + INPUT_OFFSET_STATUSWORD + 1);
                updateStatus(uiStatusWd.hl);
                if (uiDriveStatus != STATE_MACHINE_STAT_OPERATION_ENABLED)
                {
                    j = 1000;
                }
            }
            */
            /* stop RT thread */
            //Not in the linux Code! To be checked if it is required.
            //timeKillEvent(mmResult);

            printf("\nRequest init state for all slaves\n");
            ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            ec_writestate(0);
        }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n", ifname);
    }
}

OSAL_THREAD_FUNC ecatcheck(void* ptr)
{
    int slave;
    (void)ptr;                  /* Not used */


    while (1)
    {
        if (inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
                needlf = FALSE;
                //printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
                if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
                {
                    ec_group[currentgroup].docheckstate = TRUE;
                    if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                    {
                        printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                        ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                        ec_writestate(slave);
                    }
                    else if (ec_slave[slave].state == EC_STATE_SAFE_OP)
                    {
                        printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                        ec_slave[slave].state = EC_STATE_OPERATIONAL;
                        ec_writestate(slave);
                    }
                    else if (ec_slave[slave].state > EC_STATE_NONE)
                    {
                        if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d reconfigured\n", slave);
                        }
                    }
                    else if (!ec_slave[slave].islost)
                    {
                        /* re-check state */
                        ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                        if (ec_slave[slave].state == EC_STATE_NONE)
                        {
                            ec_slave[slave].islost = TRUE;
                            printf("ERROR : slave %d lost\n", slave);
                        }
                    }
                }
                if (ec_slave[slave].islost)
                {
                    if (ec_slave[slave].state == EC_STATE_NONE)
                    {
                        if (ec_recover_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d recovered\n", slave);
                        }
                    }
                    else
                    {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d found\n", slave);
                    }
                }
            }
            if (!ec_group[currentgroup].docheckstate)
                printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(SCAN_INTERVAL_IN_MSEC * 1000);
    }
}

/*
void socketServerAction() {
    INT8 bLclTryAgain = FALSE;

    switch (uiServerSocketStatus) {
    case SOCKET_SERVER_APP_START_INIT:
        sktConnectedToClient = false;
        printf("\nInitialising Winsock...");
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
            printf("WSAStartup failed with error : %d\n", iResult);
            WSACleanup();
            bLclTryAgain = TRUE;
        }
        printf("Initialised.\n");

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            bLclTryAgain = TRUE;
        }
        if (bLclTryAgain == FALSE) {
            uiServerSocketStatus = SOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT;
            printf("\nSOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT");
        }
        break;
    case SOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT:
        printf("\nInitialising Winsock...");
        // Create a SOCKET for the server to listen for client connections.
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            bLclTryAgain = TRUE;
        }

        if (bLclTryAgain == TRUE) {
            uiServerSocketStatus = SOCKET_SERVER_CREATE_SOCKET_FAILURE_ACTION;
            printf("\nSOCKET_SERVER_CREATE_SOCKET_FAILURE_ACTION");

        }
        else {
            uiServerSocketStatus = SOCKET_SERVER_SETUP_LISTENING_SOCKET;
            printf("\nSOCKET_SERVER_CREATE_SOCKET_FAILURE_ACTION");
        }

        break;
    case SOCKET_SERVER_CREATE_SOCKET_FAILURE_ACTION:
        bLclTryAgain = WSAGetLastError();
        switch (bLclTryAgain) {
        case WSANOTINITIALISED:
            uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
            printf("\nSOCKET_SERVER_APP_START_INIT");
            break;
        default:
            uiServerSocketStatus = SOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT;
            printf("\nSOCKET_SERVER_CREATE_SOCKET_TO_LISTEN_FOR_CLIENT");
            break;
        }
        break;
    case SOCKET_SERVER_SETUP_LISTENING_SOCKET:
        // Setup the TCP listening socket
        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
            printf("\nSOCKET_SERVER_APP_START_INIT");

        }
        else {
            printf("\nbind result: %d\n", iResult);
            freeaddrinfo(result);
            uiServerSocketStatus = SOCKET_SERVER_LISTEN_FOR_CLIENT_SOCKET;
            printf("\nSOCKET_SERVER_LISTEN_FOR_CLIENT_SOCKET");

        }
        break;

    case SOCKET_SERVER_LISTEN_FOR_CLIENT_SOCKET:
        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
            printf("\nSOCKET_SERVER_APP_START_INIT");

        }
        else {
            uiServerSocketStatus = SOCKET_SERVER_ACCEPT_CLIENT_SOCKET;
            printf("\nSOCKET_SERVER_ACCEPT_CLIENT_SOCKET");
        }
        break;

    case SOCKET_SERVER_ACCEPT_CLIENT_SOCKET:
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
            printf("\nSOCKET_SERVER_APP_START_INIT");

        }
        else {
            //uiDelMeCntr[2]++;
            sktConnectedToClient = true;
            uiServerSocketStatus = SOCKET_SERVER_SEND_STATUS;
            printf("\nSOCKET_SERVER_GET_DATA_FROM_CLIENT");
        }
        // No longer need server socket
        closesocket(ListenSocket);
        break;

    case SOCKET_SERVER_SEND_STATUS:
        printf("Sending Status...\n");
        SetActualTqAndPosn();
        uiServerSocketStatus = SOCKET_SERVER_GET_DATA_FROM_CLIENT;
        break;

    case SOCKET_SERVER_GET_DATA_FROM_CLIENT:
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        //uiDelMeCntr[1]++;
        if (iResult > 0) {
            printf("buf: %s ... Done", recvbuf);
            uiServerSocketStatus = SOCKET_SERVER_ANALYZE_QUERY;
            printf("\nSOCKET_SERVER_ANALYZE_QUERY");
        }
        else {
            if (iResult == 0) {
                uiServerSocketStatus = SOCKET_SERVER_CLOSE_SOCKET;
            }
            else { //Indicates receive failure with error!
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
                printf("\nSOCKET_SERVER_APP_START_INIT");

            }
        }
        break;
    case SOCKET_SERVER_ANALYZE_QUERY:
        printf("Query analyze...\n");
        GetDesiredTqAndDegOfRtn();
        //uiDelMeCntr[0]++;
        uiServerSocketStatus = SOCKET_SERVER_SEND_STATUS;
        break;

    case SOCKET_SERVER_CLOSE_SOCKET:
        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
            //printf("\nSOCKET_SERVER_APP_START_INIT:%d,%d,%d", uiDelMeCntr[0], uiDelMeCntr[1], uiDelMeCntr[2]);

        }
        break;
    default:
        WSACleanup();
        uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
        printf("\nSOCKET_SERVER_APP_START_INIT");
        break;
    }

}
*/

/*
void SocketSendResponse(char* strDataToSend) {
    printf("Str to send: %s", strDataToSend);
    int len = (int)(strlen(strDataToSend));
    iSendResult = send(ClientSocket, strDataToSend, len, 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        uiServerSocketStatus = SOCKET_SERVER_APP_START_INIT;
        printf("\nSOCKET_SERVER_APP_START_INIT");

    }
    else {
        printf("Bytes sent: %d\n", iSendResult);
        uiServerSocketStatus = SOCKET_SERVER_GET_DATA_FROM_CLIENT;
        printf("\nSOCKET_SERVER_GET_DATA_FROM_CLIENT");

    }
}
*/

int main(int argc, char* argv[])
{
    printf("Endurance 2\n");
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");
    printf("argc:%d \n", argc);

    if (argc > 1)
    {
        // create thread to handle slave error handling in OP 
  //      pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
        printf("Creating thread...\n");
        osal_thread_create(&thread1, 128000, &ecatcheck, (void*)&ctime);
        printf("Starting cyclic part...\n");
        // start cyclic part 
        simpletest(argv[1]);
    }
    else
    {
        ec_adaptert* adapter = NULL;
        printf("Usage: simple_test ifname1\nifname = eth0 for example\n");

        printf("\nAvailable adapters:\n");
        adapter = ec_find_adapters();
        while (adapter != NULL)
        {
            printf("    - %s  (%s)\n", adapter->name, adapter->desc);
            adapter = adapter->next;
        }
        ec_free_adapters(adapter);
    }

    printf("End program\n");
    return (0);
}

void add_timespec(struct timespec* ts, int64 addtime)
{
    int64 sec, nsec;

    nsec = addtime % NSEC_PER_SEC;
    sec = (addtime - nsec) / NSEC_PER_SEC;
    ts->tv_sec += sec;
    ts->tv_nsec += (long)nsec;
    if (ts->tv_nsec >= NSEC_PER_SEC)
    {
        nsec = ts->tv_nsec % NSEC_PER_SEC;
        ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
        ts->tv_nsec = (long)nsec;
    }
}


/* PI calculation to get linux time synced to DC time */
void ec_sync(int64 reftime, int64 cycletime, int64* offsettime)
{
    static int64 integral = 0;
    int64 delta;
    /* set linux sync point 50us later than DC sync, just as example */
    delta = (reftime - 50000) % cycletime;
    if (delta > (cycletime / 2)) { delta = delta - cycletime; }
    if (delta > 0) { integral++; }
    if (delta < 0) { integral--; }
    *offsettime = -(delta / 100) - (integral / 20);
    gl_delta = delta;
}


