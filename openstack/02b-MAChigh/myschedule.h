#ifndef __MYSCHEDULE_H
#define __MYSCHEDULE_H

#include "opendefs.h"
//=====================define====================
#define MYSCHEDULETIMERPERIOD 4000
#define MAXNEIGHBORS 10 //maximum number of neighbors that can be scheduled
#define STARTOFSCHEDULE 20
//=====================typedef===================
typedef struct {
	open_addr_t address; //[LENGHT_ADDR64b]; //is updated with the address of a neighbor scheduled
	uint8_t relative_slot;		//the relative slot scheduled
	uint8_t relative_channel;		//the relative channel scheduled
	bool isUsed;	//Set to TRUE if the relative slot is scheduled
}myScheduleInfo_element_t;
typedef struct{
	myScheduleInfo_element_t scheduleBuf[MAXNEIGHBORS];
	uint8_t relativePosition;//
	uint8_t state;//
	uint8_t timerld;//hold the value of the timer id
	bool busySending;//
}mySchedule_vars_t;
enum{
	MYSCHEDULE_REQUEST				   =0, //request schedule to parent
	MYSCHEDULE_RESPONSE				   =1, //send the response to last child
	MYSCHEDULE_STABLE				   =2, //send the response to last child
};
//=======================prototypes===============
void mySchedule_init(void); //initialize the variables
void mySchedule_receive(OpenQueueEntry_t* msg); //process an income message
void mySchedule_sendReq(void);
void mySchedule_processReq(OpenQueueEntry_t* msg);
void mySchedule_sendRes(void);
void mySchedule_processRes(OpenQueueEntry_t* msg);
void mySchedule_notifySendDone(OpenQueueEntry_t* msg);
#endif
