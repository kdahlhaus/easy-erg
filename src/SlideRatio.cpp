#include "SlideRatio.h"
#include <stdio.h>


//erg state values
#define STROKESTATE_DRIVING 2
#define STROKESTATE_RECOVERY 4
        
//states
#define INIT 0
#define W4NOTDRIVE 1
#define W4DRIVE 2
#define DRIVE 3
#define RECOVERY 4


#include "Filter.h"
Filter<float> filter(3);


void SlideRatio::reset(void)
{
        startedDriveAt=1;
        startedRecoveryAt=1;
        state=INIT;
        ratio=1.0;
        filter.setAllTo(ratio);
}


void SlideRatio::update(ErgState *ergState)
{
    //printf("%d %d\n", clock(), int(ergState->strokeState));
    switch (state)
    {
       case(INIT):
            if (ergState->strokeState==STROKESTATE_DRIVING) { state=W4NOTDRIVE; } else { state = W4DRIVE; }
            break;

       case(W4NOTDRIVE):
            if (ergState->strokeState!=STROKESTATE_DRIVING) { state=W4DRIVE; } 
            break;

       case(W4DRIVE):
            if (ergState->strokeState==STROKESTATE_DRIVING) 
            {
                startedDriveAt = clock();
                state=DRIVE;
            }
            break;

        case(DRIVE):
            //if (ergState->strokeState != STROKESTATE_DRIVING) 
            if (ergState->strokeState == STROKESTATE_RECOVERY) 
            {
                startedRecoveryAt = clock();
                state=RECOVERY;
            }
            break;

        case(RECOVERY):
            if (ergState->strokeState != STROKESTATE_RECOVERY)
            {
                clock_t now = clock();
                float recoveryTime = now-startedRecoveryAt;
                float driveTime = startedRecoveryAt - startedDriveAt;
                //ratio = recoveryTime / driveTime;
                filter.add(recoveryTime/driveTime);
                ratio = filter.aMean();
                state=DRIVE;
                //printf("startedDriveAt:%d startedRecoveryAt:%d driveTime:%f recoveryTime:%f ratio: %f meanRatio: %f aveRatio: %f\n",  (int)startedDriveAt, (int)startedRecoveryAt, driveTime, recoveryTime, ratio, filter.aMean(), filter.aAverage()) ;
                //printf("ratio: %.2f 3m: %.2f 4m: %.2f 3a: %.2f 4a: %.2f\n", ratio, filter.aMean(), filter4.aMean(), filter.aAverage(), filter4.aAverage()) ;
                startedDriveAt=now;
            }
            break;
    }

    ergState->slideRatio = ratio;            
}


