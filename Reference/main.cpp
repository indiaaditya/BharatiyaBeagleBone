#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <inttypes.h>
#include <iostream>

#include "ethercat.h"

#define NSEC_PER_SEC 1000000000
#define EC_TIMEOUTMON 500

struct sched_param schedp;
char IOmap[4096];
pthread_t thread1, thread2;
struct timeval tv, t1, t2;
int dorun = 0;
int deltat, tmax = 0;
int64 toff, gl_delta;
int DCdiff;
int os;
uint8 ob;
uint16 ob2;
uint8 *digout = 0;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;
boolean dcsync_enable = TRUE;
//
typedef struct PACKED
{
    uint16_t	   ControlWord;
    int32_t 	   TargetPosition;
    int32_t        TargetVelocity;
    int16_t        TargetTorque;
    int8_t         ModesOfOperation;
    uint8_t        DummyByte;
    uint16_t       TouchProbeFunction;
} out;

typedef struct PACKED
{
    uint16_t StatusWord;
    int32_t PositionActualValue;
    int32_t VelocityActualValue;
    int16_t TorqueActualValue;
    int8_t ModeOfOperationDisplay;
    uint8_t DummyByte;
    int32_t FollowingErrorActualValue;
    uint32_t DigitalInputs;
    uint16_t TouchProbeStatus;
    int32_t TouchProbe1PositiveEdgePositionValue;
} in;



using namespace std;
static int slave_dc_config(uint16 slave)
{
//  ec_dcsync0(slave,   active,           cycletime,  calc and copy time)
    ec_dcsync0(slave,   dcsync_enable,    4000000U,   2000U);
    printf("ec_dcsync0 called on slave %u\n",slave);
    return 0;
}

#define _BV(bit) 				(1 << (bit))
#define bit_is_set(val, bit) 	(val & _BV(bit))
#define bit_is_clear(val, bit) 	(!(val & _BV(bit)))

#define STATUSWORD_READY_TO_SWITCH_ON_BIT 		0
#define STATUSWORD_SWITCHED_ON_BIT 				1
#define STATUSWORD_OPERATION_ENABLE_BIT 		2
#define STATUSWORD_FAULT_BIT 					3
#define STATUSWORD_VOLTAGE_ENABLE_BIT 			4
#define STATUSWORD_QUICK_STOP_BIT 				5
#define STATUSWORD_SWITCH_ON_DISABLE_BIT 		6
#define STATUSWORD_NO_USED_WARNING_BIT 			7
#define STATUSWORD_ELMO_NOT_USED_BIT 			8
#define STATUSWORD_REMOTE_BIT 					9
#define STATUSWORD_TARGET_REACHED_BIT 			10
#define STATUSWORD_INTERNAL_LIMIT_ACTIVE_BIT	11

void redtest(char *ifname)
{
    int cnt, i, j, oloop, iloop;

    printf("Starting Redundant test\n");

    uint16 wc, slaveh;

//   if (ec_init_redundant(ifname, ifname2))
    if (ec_init(ifname))
    {
        printf("ec_init on %s succeeded.\n",ifname);
        /* find and auto-config slaves */
        if ( ec_config_init(FALSE) > 0 )
        {
            printf("%d slaves found and configured.\n",ec_slavecount);
            /* wait for all slaves to reach SAFE_OP state */
            // ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);
            // PO2SOconfig is for registering a hook function that will be called when the slave does the transition
            // between Pre-OP and Safe-OP.

            /* configure DC options for every DC capable slave found in the list */
            ///This is the original position of ec_configdc()
            ec_configdc();

            uint32 t;
            uint64 t1;

            //ecx_context.manualstatechange = 1;
//            ec_config_map(&IOmap);
//            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);

            for(int i = 1; i <= 6; i++)
            {
                ec_dcsync0(i,   dcsync_enable,    2000000U,   2000U);
            }

            ///This is the original position of mapping
            ec_config_map(&IOmap);
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);

            //read the copied system time of local clock
            t1 = 0;
            for(int i = 1; i <= 6; i++)
            {
                t1 = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSYSTIME, sizeof(t1), &t1, EC_TIMEOUTRET);
                cerr<<"System time of slave "<<i<<" : "<<dec<<t1<<endl;
            }

            //read the propagation delay of slaves
            t = 0;
            for(int i = 1; i <= 6; i++)
            {
                t = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSYSDELAY, sizeof(t), &t, EC_TIMEOUTRET);
                cerr<<"Delay time of slave "<<i<<" : "<<dec<<t<<endl;
            }

            //read the offset time from reference time to slaves
            t1 = 0;
            for(int i = 1; i <= 6; i++)
            {
                t1 = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSYSOFFSET, sizeof(t1), &t1, EC_TIMEOUTRET);
                cerr<<"Offset time of slave "<<i<<" : "<<dec<<t1<<endl;
            }

            //read the local clock of slaves
            t1 = 0;
            for(int i = 1; i <= 6; i++)
            {
                t1 = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSOF, sizeof(t1), &t1, EC_TIMEOUTRET);
                cerr<<"Local time of slave "<<i<<" : "<<dec<<t1<<endl;
            }

            //read the system time difference of slaves
            t = 0;
            for(int i = 1; i <= 6; i++)
            {
                t = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSYSDIFF, sizeof(t), &t, EC_TIMEOUTRET);
                cerr<<"System time difference of slave "<<i<<" : "<<dec<<t<<endl;
            }

            //read the sync0 cycle start time
            t1 = 0;
            for(int i = 1; i <= 6; i++)
            {
                t = 0;
                slaveh = ec_slave[i].configadr;
                wc = ec_FPRD(slaveh, ECT_REG_DCSTART0, sizeof(t1), &t1, EC_TIMEOUTRET);
                cerr<<"next sync0 start time of slave "<<i<<" : "<<dec<<t1<<endl;
            }

            for(int i = 1; i <= 6; i++)
            {
                in* in_ptr = (in*) ec_slave[i].inputs;
                out* out_ptr = (out*) ec_slave[i].outputs;

                out_ptr->ModesOfOperation = 0x08;
//                ec_send_processdata();
//                wkc = ec_receive_processdata(EC_TIMEOUTRET);
                //osal_usleep(5000);  // 0.5ms
            }
            in* in_ptr = (in*) ec_slave[6].inputs;
            out* out_ptr = (out*) ec_slave[6].outputs;

            /* read indevidual slave state and store in ec_slave[] */
            ec_readstate();
            for(cnt = 1; cnt <= ec_slavecount ; cnt++)
            {
                printf("Slave:%d Name:%s Output size:%3dbits Input size:%3dbits State:%2d delay:%d.%d\n",
                       cnt, ec_slave[cnt].name, ec_slave[cnt].Obits, ec_slave[cnt].Ibits,
                       ec_slave[cnt].state, (int)ec_slave[cnt].pdelay, ec_slave[cnt].hasdc);
                printf("         Out:%p,%4d In:%p,%4d\n",
                       ec_slave[cnt].outputs, ec_slave[cnt].Obytes, ec_slave[cnt].inputs, ec_slave[cnt].Ibytes);

            }
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);

            printf("Request operational state for all slaves\n");
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            /* request OP state for all slaves */
            ec_writestate(0);
            /* activate cyclic process data */
            dorun = 1;
            /* wait for all slaves to reach OP state */
            ec_statecheck(0, EC_STATE_OPERATIONAL,  5 * EC_TIMEOUTSTATE);
            oloop = ec_slave[0].Obytes;
            if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
            if (oloop > 8) oloop = 8;
            iloop = ec_slave[0].Ibytes;
            if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
            if (iloop > 8) iloop = 8;
            if (ec_slave[0].state == EC_STATE_OPERATIONAL )
            {
                printf("Operational state reached for all slaves.\n");
                inOP = TRUE;
                ///**********************************************////
                ///Second version of sending to the control word
//                if (bit_is_set(in_ptr->StatusWord, STATUSWORD_FAULT_BIT))  //FAULT exist
//                {
//                    out_ptr->ControlWord = 0x08;	//FAULT RESET command
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    cerr<<"print out status word inside fault loop: "<<hex<<in_ptr->StatusWord<<endl;
//                }
//                else //NO FAULT
//                {
//                    out_ptr->ControlWord = 0x06;	//SHUTDOWN command (transition#2)
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    cerr<<"print out status word after written 0x06: "<<hex<<in_ptr->StatusWord<<endl;
//                }
//                osal_usleep(5000);
//
//                if(bit_is_clear(in_ptr->StatusWord, STATUSWORD_READY_TO_SWITCH_ON_BIT))
//                {
//                    out_ptr->ControlWord = 0x07;
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    cerr<<"print out status word after written 0x07: "<<hex<<in_ptr->StatusWord<<endl;
//                }
//                else
//                {
//                    cerr<<"slave is not ready to switch on yet!!!"<<endl;
//                }
//                osal_usleep(5000);
//
//                if(bit_is_clear(in_ptr->StatusWord, STATUSWORD_SWITCHED_ON_BIT))
//                {
//                    out_ptr->ControlWord = 0x0f;	//SHUTDOWN command (transition#2)
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    cerr<<"print out status word after written 0x0f: "<<hex<<in_ptr->StatusWord<<endl;
//                }
//                else
//                {
//                    cerr<<"slave is not enabled!!!"<<endl;
//                }
//                osal_usleep(20000);

                ///Second version ends here
                ///*********************************************///


                ///**********************************************/////
                ///First version of sending to the control word
                out_ptr->ControlWord = 0x6;
                ec_send_processdata();
                wkc = ec_receive_processdata(EC_TIMEOUTRET);
                osal_usleep(5000);  // 0.5ms

                out_ptr->ControlWord = 0x7;
                ec_send_processdata();
                wkc = ec_receive_processdata(EC_TIMEOUTRET);
                osal_usleep(5000);  // 0.5ms

                out_ptr->ControlWord = 0xf;
                ec_send_processdata();
                wkc = ec_receive_processdata(EC_TIMEOUTRET);
                osal_usleep(100000);


                ///First version ends here
                ///***************************************////////


                ///Sending position command to the slaves Start
                ///**************************************///

                cerr<<"print something here "<<endl;
                int32_t tp = 0x00000000;
                cerr<<"tp:"<<hex<<tp<<endl;
                //out_ptr->TargetPosition = tp;
                cerr<<"print something here "<<endl;

                double sine_amp = 11500, f = 0.2, period;
                double gt = 0;
                int x = bit_is_set(in_ptr->StatusWord, STATUSWORD_OPERATION_ENABLE_BIT);
                //if(bit_is_set(in_ptr->StatusWord, STATUSWORD_OPERATION_ENABLE_BIT))


//                cerr<<"from:"<<hex<<in_ptr->PositionActualValue<<endl;
//
//                uint32_t max_movment = 50000;
//                int32_t gap = tp - in_ptr->PositionActualValue;
//                cerr<<"print out gap : "<<dec<<gap<<endl;
//
//                while(gap>0)
//                {
//                    if(gap >= max_movment)
//                    {
//                        out_ptr->TargetPosition = in_ptr->PositionActualValue + max_movment;
//                    }
//                    else
//                    {
//                        out_ptr->TargetPosition = in_ptr->PositionActualValue + gap;
//                    }
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    gap = tp - in_ptr->PositionActualValue;
//                }
//                while(gap < 0)
//                {
//                    if(gap >= max_movment)
//                    {
//                        out_ptr->TargetPosition = in_ptr->PositionActualValue - max_movment;
//                    }
//                    else
//                    {
//                        out_ptr->TargetPosition = in_ptr->PositionActualValue - gap;
//                    }
//                    ec_send_processdata();
//                    wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                    gap = tp - in_ptr->PositionActualValue;
//                }
//                cout<<"Reach target position"<<endl;

                ///Sending position command to slaves end here
                ///**********************************************///

                // acyclic loop 5000 x 20ms = 10s
                for(i = 1; i <= 5000; i++)
                {
//                       printf("Processdata cycle %5d ,Wck %3d, DCtime %12"PRId64", dt %12"PRId64", O:",
//                              dorun, wkc, ec_DCtime, gl_delta);
                    for(j = 0 ; j < oloop; j++)
                    {
                        printf(" %2.2x", *(ec_slave[0].outputs + j));
                    }
                    printf(" I:");
                    for(j = 0 ; j < iloop; j++)
                    {
                        printf(" %2.2x", *(ec_slave[0].inputs + j));
                    }
                    printf("\r");
                    fflush(stdout);
                    osal_usleep(20000);
                    //read the time difference
                }
                dorun = 0;
                inOP = FALSE;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                               i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }

            /// ec_dcsync0(6, FALSE, 8000,  0);
            printf("Request safe operational state for all slaves\n");
            ec_slave[0].state = EC_STATE_SAFE_OP;
            /* request SAFE_OP state for all slaves */
            ec_writestate(0);


        }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End redundant test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

/* add ns to timespec */
void add_timespec(struct timespec *ts, int64 addtime)
{
    int64 sec, nsec;

    nsec = addtime % NSEC_PER_SEC;
    sec = (addtime - nsec) / NSEC_PER_SEC;
    ts->tv_sec += sec;
    ts->tv_nsec += nsec;
    if ( ts->tv_nsec >= NSEC_PER_SEC )
    {
        nsec = ts->tv_nsec % NSEC_PER_SEC;
        ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
        ts->tv_nsec = nsec;
    }
}

/* PI calculation to get linux time synced to DC time */
void ec_sync(int64 reftime, int64 cycletime, int64 *offsettime)
{
    static int64 integral = 0;
    int64 delta;
    /* set linux sync point 50us later than DC sync, just as example */
    delta = (reftime - 50000) % cycletime;
    if(delta> (cycletime / 2))
    {
        delta= delta - cycletime;
    }
    if(delta>0)
    {
        integral++;
    }
    if(delta<0)
    {
        integral--;
    }
    *offsettime = -(delta / 100) - (integral / 20);
    gl_delta = delta;
}

/* RT EtherCAT thread */
OSAL_THREAD_FUNC_RT ecatthread(void *ptr)
{
    struct timespec   ts, tleft;
    int ht;
    int64 cycletime;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    ht = (ts.tv_nsec / 1000000) + 1; /* round to nearest ms */
    ts.tv_nsec = ht * 1000000;
    if (ts.tv_nsec >= NSEC_PER_SEC)
    {
        ts.tv_sec++;
        ts.tv_nsec -= NSEC_PER_SEC;
    }
    cycletime = *(int*)ptr * 1000; /* cycletime in ns */
    toff = 0;
    dorun = 0;
    ec_send_processdata();
    while(1)
    {
        /* calculate next cycle start */
        add_timespec(&ts, cycletime + toff);
        /* wait to cycle start */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
        if (dorun>0)
        {
            wkc = ec_receive_processdata(EC_TIMEOUTRET);

            dorun++;
            /* if we have some digital output, cycle */
            if( digout ) *digout = (uint8) ((dorun / 16) & 0xff);

            if (ec_slave[0].hasdc)
            {
                /* calulate toff to get linux time and DC synced */
                ec_sync(ec_DCtime, cycletime, &toff);
            }
            ec_send_processdata();
        }
    }
}

OSAL_THREAD_FUNC ecatcheck( void *ptr )
{
    int slave;

    (void) ptr;

    uint32 t, t1;
    uint16 wc, slaveh;
    uint64 t2;

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
                needlf = FALSE;
                printf("\n");
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
//                        for(int i = 1; i <= 6; i++)
//                        {
//                            slaveh = ec_slave[i].configadr;
//                            wc = ec_FPRD(slaveh, ECT_REG_DCSYSDIFF, sizeof(t1), &t1, EC_TIMEOUTRET);
//                            cerr<<"Time difference of slave "<<i<<" : "<<dec<<t1<<endl;
//                        }
                    }
                    else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                    {
                        printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                        ec_slave[slave].state = EC_STATE_OPERATIONAL;
                        ec_writestate(slave);
//                        for(int i = 1; i <= 6; i++)
//                        {
//                            slaveh = ec_slave[i].configadr;
//                            wc = ec_FPRD(slaveh, ECT_REG_DCSYSDIFF, sizeof(t1), &t1, EC_TIMEOUTRET);
//                            cerr<<"Time difference of slave "<<i<<" : "<<dec<<t1<<endl;
//                        }
                    }
                    else if(ec_slave[slave].state > EC_STATE_NONE)
                    {
                        if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d reconfigured\n",slave);
                        }
                    }
                    else if(!ec_slave[slave].islost)
                    {
                        /* re-check state */
                        ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                        if (ec_slave[slave].state == EC_STATE_NONE)
                        {
                            ec_slave[slave].islost = TRUE;
                            printf("ERROR : slave %d lost\n",slave);
//                            for(int i = 1; i <= 6; i++)
//                            {
//                                slaveh = ec_slave[i].configadr;
//                                wc = ec_FPRD(slaveh, ECT_REG_DCSYSDIFF, sizeof(t1), &t1, EC_TIMEOUTRET);
//                                cerr<<"Time difference of slave "<<i<<" : "<<dec<<t1<<endl;
//                            }
                        }
                    }
                }
                if (ec_slave[slave].islost)
                {
                    if(ec_slave[slave].state == EC_STATE_NONE)
                    {
                        if (ec_recover_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d recovered\n",slave);
                        }
                    }
                    else
                    {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d found\n",slave);
                    }
                }
            }
            if(!ec_group[currentgroup].docheckstate)
            {
                printf("OK : all slaves resumed OPERATIONAL.\n");
//                for(int i = 1; i <= 6; i++)
//                {
//                    slaveh = ec_slave[i].configadr;
//                    wc = ec_FPRD(slaveh, ECT_REG_DCSYSDIFF, sizeof(t1), &t1, EC_TIMEOUTRET);
//                    cerr<<"Time difference of slave "<<i<<" : "<<dec<<t1<<endl;
//                }
            }
        }
        osal_usleep(10000);
    }
}

#define stack64k (64 * 1024)

int main(int argc, char *argv[])
{
    int ctime;

    printf("SOEM (Simple Open EtherCAT Master)\nRedundancy test\n");

    if (argc > 2)
    {
        dorun = 0;
        ctime = atoi(argv[2]);

        /* create RT thread */
        osal_thread_create_rt((void*)&thread1, stack64k * 2, (void*)&ecatthread, (void*) &ctime);

        /* create thread to handle slave error handling in OP */
        osal_thread_create((void*)&thread2, stack64k * 4, (void*)&ecatcheck, NULL);

        /* start acyclic part */
        redtest(argv[1]);
    }
    else
    {
        printf("Usage: ifname cycletime\nifname = eth0 for example\ncycletime in us\n");
    }

    printf("End program\n");

    return (0);
}
