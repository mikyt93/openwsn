#include "opendefs.h"
#include "schedule.h"
#include "openserial.h"
#include "opentimers.h"
#include "openrandom.h"
#include "sixtop.h"
#include "IEEE802154.h"
#include "myschedule.h"

//Global Variable
myschedule_vars_t myschedule_vars;
//Private function prototypes
void mySchedule_scheduleNewNeighbor(OpenQueueEntry_t* msg);
void timers_mySchedule_fired(void);
void mySchedule_init(void){
	memset(&myschedule_vars,0,sizeof(myschedule_vars_t));
		myschedule_vars.state	=MYSCHEDULE_REQUEST;
		myschedule_vars.busySending	=FALSE;
		myschedule_vars.timerld	=opentimers_start(MYSCHEDULETIMERPERIOD,TIMER_PERIODIC,TIME_MS,timers_mySchedule_fired);
}
