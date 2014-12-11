/*

note: make sure that ergnet is set for ergtype of this erg before every call into the dll.
the check is very fast.  see top line of doCsafeCommand

*/



#include "Pm3Erg.h"
#include "CIIHeaders.h"

#include <stdio.h>

#include <iostream>

#include "windows.h"


#include "ErgNet.h"


#if LIMITED
#include "Asprotect.h"
#endif


extern void initializeDlls();

using namespace std;


void ErgValueCache::reset()
{
    nextHR=-1;
    nextPower=-1;
    nextCadence=-1;
    nextPaceInSecondsPer500=-1;
    nextDragFactor=-1;
    lastHeartRate=0;
    lastPower=0;
    lastCadence=0;
    lastPaceInSecondsPer500=0;
    lastDragFactor=0;
    lastAccumulatedCalories=0;
    lastCaloriesPerHour=0;
    averageHeartRateEntireWorkout.reset();
    averageWattsEntireWorkout.reset();
} 


unsigned short ErgValueCache::getHeartRate( clock_t now )
{
    if (now >= nextHR)
    {
        nextHR = now + HR_PERIOD;
        lastHeartRate=erg->getHeartRate();
        if (lastHeartRate > 3 && lastHeartRate < 300)
        {
            averageHeartRateEntireWorkout.add(lastHeartRate);
        }
    }
    return lastHeartRate;
} 

unsigned short ErgValueCache::getPower( clock_t now )
{
    if (now >= nextPower)
    {
        nextPower = now + POWER_PERIOD;
        lastPower=erg->getPower();
        averageWattsEntireWorkout.add(lastPower);
    }
    return lastPower;
}

unsigned short ErgValueCache::getCadence( clock_t now )
{
    if (now >= nextCadence)
    {
        nextCadence = now + CADENCE_PERIOD;
        lastCadence=erg->getCadence();
    }
    return lastCadence;
}


unsigned short ErgValueCache::getPaceInSecondsPer500( clock_t now )
{
    if (now >= nextPaceInSecondsPer500)
    {
        nextPaceInSecondsPer500 = now + PACE_PERIOD;
        lastPaceInSecondsPer500=erg->getPaceInSecondsPer500();
        double paceMetPerSec = lastPaceInSecondsPer500/500.0;
        if (paceMetPerSec != 0.0 )
        {
            lastCaloriesPerHour = (int)((2.8/(paceMetPerSec*paceMetPerSec*paceMetPerSec))*(3.4416000000000002))+300.0;
        }
    }
    return lastPaceInSecondsPer500;
}

unsigned short ErgValueCache::getDragFactor( clock_t now )
{
    if (now >= nextDragFactor)
    {
        nextDragFactor = now + DRAG_FACTOR_PERIOD;
        lastDragFactor=erg->getDragFactor();
    }
    return lastDragFactor;
}

int ErgValueCache::getAccumulatedCalories( clock_t now )
{
    if (now >= nextAccumulatedCalories)
    {
        nextAccumulatedCalories = now + ACCUMULATED_CALORIES_PERIOD;
        lastAccumulatedCalories=erg->getAccumulatedCalories();
    }
    return lastAccumulatedCalories;
}



const char *Pm3Erg::workoutStateText[] = 
{
    "Waiting to begin",
    "Workout row", 
    "Countdown pause",
    "Interval rest", 
    "Work time interval", 
    "Work distance interval", 
    "Rest interval end to work time interval begin", 
    "Rest interval end to work distance interval begin", 
    "Work time interval end to rest interval begin", 
    "Work distance interval end to rest interval begin",  
    "Workout end", 
    "Workout terminate", 
    "Workout logged", 
    "Workout rearm"
}; 

const char *Pm3Erg::intervalTypeText[] = { "Time", "Distance", "Rest" }; // p12



Pm3Erg::Pm3Erg( int aUnitAddress ) : cache(this)
{
    //cout << "*** Pm3Erg(" << aUnitAddress << ")\n";
    
    initializeDlls();

#if LIMITED
    LICENSE_HAS_TIMEDOUT=!canRunImp();
#endif

    HAS_TIMEDOUT_THIS_EXEC=0;
    NUM_CSAFE_CHECKS=0;

    ergType = ergNet.getErgType(aUnitAddress);

    setJustRow = false;
    metersGoal = ErgState::NO_METERS;
    timeGoal = ErgState::NO_TIME;
    workoutType = 0;
    lastStrokeState = 0;
    numStrokesIn=0;
    unitAddress = aUnitAddress;
    strncpy( verPtr, "<this is not the real version number string>", PM3_BUFFER_SIZE);
    strncpy( serialNum, "<this is not the real serial number string>", PM3_BUFFER_SIZE);
    resetHasStartedRowing();
}

void  Pm3Erg::doCsafeCommand( const char *memo, UINT16_T cmdSize, UINT32_T *cmdData, UINT16_T& rspDataSize, UINT32_T *rspData)
{
    log("doCsafeCommand '%s', %d, ...", memo, cmdSize);
#if LIMITED
    if (LICENSE_HAS_TIMEDOUT)
    {
        return;    // quick and easy
    }
#endif

#if 0
#if LIMITED
    NUM_CSAFE_CHECKS+=1;
    //cout << "LICENSE_HAS_TIMEDOUT: " << LICENSE_HAS_TIMEDOUT << " NUM_CSAFE_CHECKS=" << NUM_CSAFE_CHECKS << "  NUM_CSAFE_CALLS_PER_CHECK=" << NUM_CSAFE_CALLS_PER_CHECK << "\n";
    if (LICENSE_HAS_TIMEDOUT && (NUM_CSAFE_CHECKS==NUM_CSAFE_CALLS_PER_CHECK))
    {
        cout << ".\n";
        if (rowShouldBeLimited(getDistanceRowedSoFar(),0) || rowShouldBeLimited(0,getSecondsIntoThePiece()))
        {
            HAS_TIMEDOUT_THIS_EXEC=1;
            cout << "Pm3 has Timed out!\n";
            return;
        }
        NUM_CSAFE_CHECKS=0;
    }
#endif
#endif
    ergNet.setForErgType(ergType);
    ERRCODE_T errorCode;
    errorCode = csafe.tkcmdsetCSAFE_command( unitAddress, cmdSize, cmdData, &rspDataSize, rspData);
    if (errorCode)
    {
        log("error: %d", errorCode);
        logCSafeError(errorCode, memo);
    }
}

unsigned long int Pm3Erg::getTargetTime()
{
    return timeGoal;
}
unsigned long int Pm3Erg::getTargetDistance()
{
    return metersGoal;
}




void Pm3Erg::getTimeIntoThePiece(int *hours, int *mins, int *secs)
{
    UINT32_T cmdData[]={ 0xA0 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getWork", 1,  cmdData, rspDataSize, rspData ) ;

    *hours = rspData[2];
    *mins = rspData[3];
    *secs = rspData[4];
}

unsigned long int Pm3Erg::getSecondsIntoThePiece()
{
    int hours, mins, secs;
    getTimeIntoThePiece( &hours, &mins, &secs);
    return (hours*3600+mins*60+secs);
}


unsigned long int Pm3Erg::getDisplayedTime()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0xa0};
    doCsafeCommand( "getWorkTime", 3,  cmd, rspDataSize, rspData ) ;
    return  ((((rspData[7] << 8 ) + (rspData[6] ) << 8 ) + (rspData[5] ) << 8) + rspData[4])/100;
}




unsigned long Pm3Erg::getDistanceRowedSoFar()
{
    // csafe version
    UINT16_T cmdSize=1;
    UINT32_T cmdData[]={ 0xa1 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getDistanceRowedSoFar", 1,  cmdData, rspDataSize, rspData ) ;
    unsigned long meters =  (rspData[3] << 8) + rspData[2];
    log("getDistanceRowedSoFar()=%d", meters);
    return meters;
}

int Pm3Erg::getAccumulatedCalories()
{
    UINT16_T cmdSize=1;
    UINT32_T cmdData[]={ 0xa3 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getAccumulatedCalories", 1,  cmdData, rspDataSize, rspData ) ;
    int cals =  (rspData[3] << 8) + rspData[2];
    log("getAccumulatedCalories()=%d", cals);
    return cals;
}


unsigned long int Pm3Erg::getDisplayedMeters()
{
    // pm3 specific
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0xa3};
    doCsafeCommand( "getDisplayedMeters", 3,  cmd, rspDataSize, rspData ) ;
    //return  (((rspData[7] << 8 ) + (rspData[6] ) << 8 ) + (rspData[5] ) << 8) + rspData[4]; // in tenths of meter
    long int meters =   ((((rspData[7] << 8 ) + (rspData[6] ) << 8 ) + (rspData[5] ) << 8) + rspData[4])/10;
    log("getDisplayedMeters()=%d", meters);
    return meters;
}

void Pm3Erg::enableScreenErrorMode( int showErrors )
{
    log("enableScreenErrorMode(%d)", showErrors);
    Sleep(5000);
    int mode = showErrors ? 1 : 0;
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x02, 0x27, 0x01};
    cmd[4]=showErrors;
    doCsafeCommand( "enableScreenErrorMode", 5,  cmd, rspDataSize, rspData ) ; 
}



void Pm3Erg::setSplitDuration(unsigned short timeDistance, unsigned int duration)
{
    log("setSplitDuration(%d,%d)", timeDistance, duration);
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x07, 0x05, 0x05, 0, 0,  0, 0, 0};
    cmd[4]=timeDistance;
    cmd[5]=      duration & 0xff;
    cmd[6]= (duration>>8) & 0xff;
    cmd[7]=(duration>>16) & 0xff;
    cmd[8]=(duration>>24) & 0xff;
    #if 0
    cout << "setSplitDuration " << duration << " cmd: ";
    cout << int(cmd[4]) << " ";    
    cout << int(cmd[5]) << " ";    
    cout << int(cmd[6]) << " ";    
    cout << int(cmd[7]) << " ";    
    cout << int(cmd[8]) << " \n";    
    #endif
    doCsafeCommand( "setSplitDuration", 9,  cmd, rspDataSize, rspData ) ;
}

unsigned short Pm3Erg::getStrokeState()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0xBF};
    doCsafeCommand( "getStrokeState", 3,  cmd, rspDataSize, rspData ) ;
    log("getStrokeState()=%d", rspData[rspDataSize-1]);
    return rspData[rspDataSize-1];
}

unsigned short Pm3Erg::getWorkoutState()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0x8D};
    doCsafeCommand( "getWorkoutState", 3,  cmd, rspDataSize, rspData ) ;
    log("getWorkoutState()=%d", rspData[rspDataSize-1]);
    return rspData[rspDataSize-1];
}

unsigned short Pm3Erg::getWorkoutIntervalCount()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0x9F};
    doCsafeCommand( "getWorkoutIntervalCount", 3,  cmd, rspDataSize, rspData ) ;
    log("getWorkoutIntervalCount()=%d", rspData[rspDataSize-1]);
    return rspData[rspDataSize-1];
}

unsigned short Pm3Erg::getWorkoutIntervalType()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0x8E};
    doCsafeCommand( "getWorkoutIntervalType", 3,  cmd, rspDataSize, rspData ) ;
    log("getWorkoutIntervalType()=%d", rspData[rspDataSize-1]);
    return rspData[rspDataSize-1];
}

int Pm3Erg::getRestTime()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0xCF};
    doCsafeCommand( "getRestTime", 3,  cmd, rspDataSize, rspData ) ;
    int restTime =  (rspData[5] << 8) + rspData[4];
    log("getRestTime()=%d", restTime);
    return restTime;
}




unsigned short Pm3Erg::getDragFactor()
{
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    UINT32_T cmd[]={0x1a, 0x01, 0xC1};
    doCsafeCommand( "getDragFactor", 3,  cmd, rspDataSize, rspData ) ;
    log("getDragFactor()=%d", rspData[4]);
    return rspData[4];
}

const char *Pm3Erg::getFirmwareVersion()
{
    ergNet.setForErgType(ergType);
    ERRCODE_T errorCode;
    errorCode = tkcmdsetDDI_fw_version( unitAddress, (INT8_T *)verPtr, PM3_BUFFER_SIZE);
    if (errorCode)
    {
        logDdiError(errorCode, "error in getFwVersion");
    }
    return (const char *)verPtr;
}

const char *Pm3Erg::getSerialNumber()
{
    ergNet.setForErgType(ergType);
    ERRCODE_T errorCode;
    errorCode = tkcmdsetDDI_serial_number( unitAddress, (INT8_T *)serialNum, PM3_BUFFER_SIZE);
    if (errorCode)
    {
        logDdiError(errorCode, "error in get serial number");
    }
    return (const char *)serialNum;
}



void Pm3Erg::goReady()
{
    UINT32_T cmdData[]={ 0x87 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "goReady", 1,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::goIdle()
{
    UINT32_T cmdData[]={ 0x82 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "goIdle", 1,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::goInUse()
{
    UINT32_T cmdData[]={ 0x85 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "goInUse", 1,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::goFinished()
{
    UINT32_T cmdData[]={ 0x86 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "goFinished", 1,  cmdData, rspDataSize, rspData ) ;
}





unsigned short Pm3Erg::getWorkoutType()
{
    // TODO - fix this when learn how to set a real JUST ROW workout
    if (setJustRow)
    { 
        log("getWorkoutType()=1");
        return 1; 
    } 
    else
    {
        UINT16_T rspDataSize=64;
        UINT32_T rspData[64];
        UINT32_T cmd[]={0x1a, 0x01, 0x89};
        doCsafeCommand( "getWorkoutType", 3,  cmd, rspDataSize, rspData ) ;
        log("getWorkoutType()=%d", rspData[4]);
        return rspData[4];
    }
}




const char * Pm3Erg::getWorkoutTypeText()
{ return getWorkoutTypeText(getWorkoutType()); }

const char * Pm3Erg::getWorkoutTypeText(unsigned short workoutType)
{
    if (workoutType < 9)
    {
        const char *workoutTypes[] =
        {
            "Just Row/no splits",
            "Just Row/splits",
            "Fixed Distance/no splits",
            "Fixed Distance/splits",
            "Fixed Time/no splits",
            "Fixed Time/splits",
            "Fixed Time Interval",
            "Fixed Distance Interval",
            "Variable Interval"
        };
        return workoutTypes[workoutType];
    }
    return "Invalid workout type";
}

const char * Pm3Erg::getStrokeStateText()
{
    const char *strokeState[] =
    {
        "Waiting for w to reach min speed",
        "Waiting for w to accelerate",
        "Driving",
        "Dwell after drive",
        "Recovery"
    };
    return strokeState[getStrokeState()];
}


unsigned short Pm3Erg::getCadence()
{
    UINT32_T cmdData[]={ 0xA7 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getCadence", 1,  cmdData, rspDataSize, rspData ) ;
    unsigned short cadence = (rspData[3] << 8) + rspData[2];
    log("getCadence()=%d", cadence);
    return cadence;
}

unsigned short Pm3Erg::getPaceInSecondsPer500()
{
    UINT32_T cmdData[]={ 0xA6 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getPace", 1,  cmdData, rspDataSize, rspData ) ;
    unsigned short pace = ((rspData[3] << 8) + rspData[2])/2;
    log("getPaceInSecondsPer500()=%d", pace);
    return pace;
}

unsigned short Pm3Erg::getPower()
{
    UINT32_T cmdData[]={ 0xb4 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getPower", 1,  cmdData, rspDataSize, rspData ) ;
    unsigned short power = (rspData[3] << 8) + rspData[2];
    log("getPower()", power);
    return power;
}

unsigned short Pm3Erg::getHeartRate()
{
    UINT32_T cmdData[]={ 0xb0 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "getHeartRate", 1,  cmdData, rspDataSize, rspData ) ;
    log("getHeartRate()=%d", rspData[2]);
    return rspData[2];
}



void Pm3Erg::getPace( int &paceMinutes, int &paceSecs)
{
    unsigned int pace = getPaceInSecondsPer500();
    paceMinutes = pace/60;
    paceSecs = pace - ( paceMinutes * 60 );
}




void Pm3Erg::setMetersGoal(unsigned long int meters)
{
    UINT32_T cmdData[]={ 0x21, 3, 0, 0, 0  };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    cmdData[2]=meters & 0x00ff;
    cmdData[3]=(meters >> 8) & 0x00ff;
    cmdData[4]=36; //meters
    doCsafeCommand( "setMetersGoal", 5,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::setProgram(unsigned short int id)
{
    UINT32_T cmdData[]={ 0x24, 2, 0, 0 };
    cmdData[2]=id;
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "setProgram", 4,  cmdData, rspDataSize, rspData ) ;
}


void Pm3Erg::setTimeGoal(int hours, int min, int seconds)
{
    UINT32_T cmdData[]={ 0x20, 3, 0, 0, 0 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    cmdData[2]=hours;
    cmdData[3]=min;
    cmdData[4]=seconds;
    doCsafeCommand( "setTimeGoal", 5,  cmdData, rspDataSize, rspData ) ;
}


void Pm3Erg::setHorizontal(unsigned int meters)
{
    log("setHorizontal(%d)",meters);
    UINT32_T cmdData[]={ 0x21, 3, 0, 0, 36};
    cmdData[2]=meters & 0xff;
    cmdData[3]=(meters>>8) & 0xff;
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "setHorizontal", 5,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::setWork(unsigned short hours, unsigned short minutes, unsigned short seconds)
{
    UINT32_T cmdData[]={ 0x20, 3, 0, 0, 0};
    cmdData[2]=hours;
    cmdData[3]=minutes;
    cmdData[4]=seconds;
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "setWork", 5,  cmdData, rspDataSize, rspData ) ;
}

void Pm3Erg::setPower(unsigned int power)
{
    UINT32_T cmdData[]={ 0x34, 3, 0, 0, 58};
    cmdData[2]=power & 0xff;
    cmdData[3]=(power>>8) & 0xff;
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "setPower", 5,  cmdData, rspDataSize, rspData ) ;
}


/*

case 0:  // Just Row/no splits
case 1:  // Just Row/splits
case 2:  // Fixed Distance/no splits
case 3:  // Fixed Distance/splits
case 4:  // Fixed Time/no splits
case 5:  // Fixed Time/splits
case 6:  // Fixed Time Interval
case 7:  // Fixed Distance Interval
case 8:  // Variable Interval

*/

void Pm3Erg::start()
{
    //cout << "**** Pm3Erg::start\n";
    log("Pm3Erg::start");
    numStrokesIn=0;
    lastStrokeState=0;
    resetHasStartedRowing(); 

    //determine initial workout parameters
    workoutType = getWorkoutType();

    switch ( workoutType )
    {
    case 0:  // Just Row/no splits
    case 1:  // Just Row/splits
    case 6:  // Fixed Time Interval
    case 7:  // Fixed Distance Interval
    case 8:  // Variable Interval
        metersGoal = ErgState::NO_METERS;
        timeGoal = ErgState::NO_TIME;
        break;

    case 2:  // Fixed Distance/no splits
    case 3:  // Fixed Distance/splits
        metersGoal = getDisplayedMeters();
        timeGoal = ErgState::NO_TIME;
        break;

    case 4:  // Fixed Time/no splits
    case 5:  // Fixed Time/splits
        timeGoal = getDisplayedTime();
        metersGoal = ErgState::NO_METERS;
        break;
    default:
        char msg[200];
        sprintf( msg, "Erg is set for an unknown workout type: %d (Pm3Erg::start)", workoutType);
        logGenericError(-1, msg, "Unknown workout type", msg);
    }
}

void Pm3Erg::updateState( ErgState *state)
{
    clock_t now = clock();

    #ifdef TIME_UPDATESTATE        
    times[currentTimeIndex++]=clock();
    if (currentTimeIndex == NUM_TIMES)
    {
        float total=0;
        int num=0;
        for (int i=1; i<NUM_TIMES; i++)
        {
            float t1=times[i];
            float t2=times[i-1];
            float diff = (t1-t2)/CLOCKS_PER_SEC;
            total += diff;
            num++;
        }
        cout << "average: " << total/num << " freq:" <<  1.0/(total/num) << "\n";
        currentTimeIndex=0;
    }
    #endif

    state->metersIn=getDistanceRowedSoFar();
    state->timeIn=getSecondsIntoThePiece();

#ifdef LIMITED
    if (rowShouldBeLimited( state->metersIn, state->timeIn ))
    {
        HAS_TIMEDOUT_THIS_EXEC=1;
        state->metersIn=METERS_IN_LIMIT;
        state->timeIn=SECONDS_IN_LIMIT;
        return;
    }
#endif

    state->cadence=cache.getCadence(now);
    state->split=cache.getPaceInSecondsPer500(now);
    state->watts=cache.getPower(now); //getPower();
    state->strokeState=getStrokeState();
    state->heartRate = cache.getHeartRate(now); //getHeartRate();
    state->dragFactor = cache.getDragFactor(now);
    state->accumulatedCalories = cache.getAccumulatedCalories(now);
    state->caloriesPerHour = cache.getCaloriesPerHour(now);
    state->averageHeartRateEntireWorkout = cache.getAverageHeartRateEntireWorkout();
    state->averageWattsEntireWorkout = cache.getAverageWattsEntireWorkout();



    // need to fix this
    if (state->strokeState != lastStrokeState)
    {
        lastStrokeState = state->strokeState;
        if (lastStrokeState==2) //changed to drive
        {
            state->numStrokesIn=numStrokesIn++;
            metersPerStroke=state->metersIn-metersAtStartOfStroke;
            metersAtStartOfStroke=state->metersIn;
        }
    }
    state->metersPerStroke=metersPerStroke;

    state->workoutType  = workoutType;

    if (workoutType==6 || workoutType==7) // Fixed Time/Distance interval
    {
        state->workoutState = getWorkoutState();
        state->workoutIntervalCount = getWorkoutIntervalCount();
        state->workoutIntervalType = getWorkoutIntervalType();
        state->restTime = getRestTime();
    }

    switch ( workoutType )
    {
    case 0:  // Just Row/no splits
    case 1:  // Just Row/splits
    case 6:  // Fixed Time Interval
    case 7:  // Fixed Distance Interval
    case 8:  // Variable Interval
        state->metersToGo = ErgState::NO_METERS;
        state->timeToGo = ErgState::NO_TIME;
        state->percentComplete = ErgState::NO_PERCENT;
        break;

    case 2:  // Fixed Distance/no splits
    case 3:  // Fixed Distance/splits
        state->metersToGo = metersGoal-(state->metersIn);
        state->timeToGo = (( state->split ) * state->metersToGo )/500;
        state->percentComplete = metersGoal != 0 ? (((state->metersIn)*100)/metersGoal) : ErgState::NO_PERCENT;
        break;

    case 4:  // Fixed Time/no splits
    case 5:  // Fixed Time/splits
        state->timeToGo = timeGoal - (state->timeIn);
        state->percentComplete = timeGoal != 0 ? (((state->timeIn)*100)/timeGoal) : ErgState::NO_PERCENT;
        if (state->split)
        {
            state->metersToGo = ((state->timeToGo )*500)/(state->split);
        }
        else
        {
            state->metersToGo=ErgState::NO_METERS;
        }
        break;
    default:
        char msg[200];
        sprintf( msg, "Erg is set for an unkown workout type: %d (Pm3Erg::updateState)", workoutType);
        logGenericError(-1, msg, "Unkown workout type", msg);


    }
    slideRatioCalculator.update(state);
}


void Pm3Erg::reset()
{

    //cout << "**** Pm3Erg::reset\n";

    UINT32_T cmdData[]={ 0x81 };
    UINT16_T rspDataSize=64;
    UINT32_T rspData[64];
    doCsafeCommand( "reset", 1,  cmdData, rspDataSize, rspData ) ;

    resetHasStartedRowing(); 
    cache.reset();

    metersPerStroke=0;
    metersAtStartOfStroke=0;

    #ifdef TIME_UPDATESTATE
    currentTimeIndex=0;
    #endif
}


void Pm3Erg::resetHasStartedRowing()
{
    _hasStartedRowing = false;
    _hasSeenStrokeState0 = false;
}

int Pm3Erg::hasStartedRowing()
{
    // 'hasStartedRowing' if we pass through stroke state 1 (waiting for wheel to accelerate)
    if (!_hasStartedRowing)
    {
        int currentStrokeState = getStrokeState();
        if (currentStrokeState==2 || _hasSeenStrokeState0 && currentStrokeState==1) 
        {
            _hasStartedRowing=true;
        }
        else if (currentStrokeState==0)
        {
            _hasSeenStrokeState0=true;
        }
        

        
        /*
        if (!_hasSeenStrokeState1 && currentStrokeState==1) 
        { 
            _hasSeenStrokeState1=true;
        }
        else if (!_hasSeenStrokeState2 && currentStrokeState==2)
        {
            _hasSeenStrokeState2 = true;
            _hasStartedRowing = true;
        }  
        */
    }
    log("hasStartedRowing()=%d  _hasSeenStrokeState0=%d", _hasStartedRowing, _hasSeenStrokeState0);
    return _hasStartedRowing;
}

float Pm3Erg::getRequiredFirmwareVersion()
{
    switch ( getErgType() )
    {
    case PM3_ERGTYPE:
        return 97.0;
    case PM4_ERGTYPE:
        return 21;
    default:
        return -1;
    }
}

void Pm3Erg::doFixedDistance(int meters, int split)
{

    log("doFixedDistance(%d,%d)",meters,split);
    reset();
    goFinished();
    goIdle();
    setHorizontal(meters);
    if (split==0)
    {
       if (meters<500) { split = 100; }
       if (meters>=500 && meters<=15000) { split=500; }
       if (meters>15000 && meters<=30000) { split=1000; }
       if (meters>30000) { split=10000; }
    }
    setSplitDurationInMeters(split);
    setProgram(0);
    goInUse();
    setJustRow=false;
}


void Pm3Erg::doFixedTime(int hours, int min, int seconds, int split)
{

    reset();
    goFinished();
    goIdle();
    goReady();
    setTimeGoal( hours, min, seconds);
    if (split==0)
    {
        long int tot = (hours*3600+min*60+seconds);
        if (tot<=120) { split = tot; }
        if (tot>120  && tot<=3600) { split = 120;  }  // 2 min
        if (tot>3600 && tot<=7200) { split = 240; }  // 4 min
        if (tot>7200 && tot<=18000) { split = 600; } //10 min
        if (tot>18000) { split = 1800; }  // 30min
    }
    setSplitDurationInSeconds(split);
    setProgram(0);
    goInUse();
    setJustRow=false;
}


void Pm3Erg::log(const char *format, ...)
{
    return;
    char buffer[128];
    va_list ap;
    va_start(ap, format); /*make ap point to the first arg after fmt*/
    vsprintf( buffer, format, ap);
    cout << buffer << "\n";
    //Sleep(5000);
}




/*
void Pm3Erg::doJustRow(int split)
{

    reset();
    goFinished();
    goIdle();
    if (split>0)
    {
        setSplitDurationInMeters(split);
    }
    setProgram(0);
    goReady();
    goInUse();
    setJustRow=true;
}
*/



const char *Pm3Erg::getWorkoutStateText() { return getWorkoutStateText(getWorkoutState()); }

const char *Pm3Erg::getWorkoutStateText(int workoutState)
    {
        if (workoutState>-1 && workoutState<14)
        {
        return workoutStateText[workoutState];
        }
        return "Invalid workout state number";
}




const char *Pm3Erg::getWorkoutIntervalTypeText() 
{ return getWorkoutIntervalTypeText(getWorkoutIntervalType()); }

const char *Pm3Erg::getWorkoutIntervalTypeText(unsigned short intervalType)
    {
        if (intervalType<3)
        {
        return intervalTypeText[intervalType];
        }
        return "Invalid interval type";
}





