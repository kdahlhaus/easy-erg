#ifndef Erg_h
#define Erg_h

#include <ctime>
#define NUM_TIMES 50

#include "RunningAverage.h"
#include "CIIHeaders.h"
#include "ErgState.h"
#include "SlideRatio.h"
#define PM3_BUFFER_SIZE 50

#include "ErgNet.h"


// to enforce time out, must define 'LIMITED'

#define LICENSE_HAS_TIMEDOUT longTermDrag
#define HAS_TIMEDOUT_THIS_EXEC longTermSlideRatio
#define NUM_CSAFE_CHECKS shortTermPower 

#define METERS_IN_LIMIT 15 //1500
#define SECONDS_IN_LIMIT 25
#define NUM_CSAFE_CALLS_PER_CHECK 100


//define TIME_UPDATESTATE to enable
// timing of updateState(&ergState) calls
// #define TIME_UPDATESTATE
//

class Erg;




class ErgValueCache
{
    private:
        Erg *erg;


        unsigned short lastHeartRate;
        RunningAverage<unsigned short, unsigned long> averageHeartRateEntireWorkout;
        clock_t nextHR;
        static const clock_t HR_PERIOD = CLOCKS_PER_SEC*1;

        unsigned short lastPower;
        RunningAverage<unsigned short, unsigned long> averageWattsEntireWorkout;
        clock_t nextPower;
        static const clock_t POWER_PERIOD = CLOCKS_PER_SEC*0.52;

        unsigned short lastCadence;
        clock_t nextCadence;
        static const clock_t CADENCE_PERIOD = CLOCKS_PER_SEC*0.7; 

        unsigned short lastPaceInSecondsPer500;
        clock_t nextPaceInSecondsPer500;
        static const clock_t PACE_PERIOD = CLOCKS_PER_SEC*0.45; 

        unsigned short lastDragFactor;
        clock_t nextDragFactor;
        static const clock_t DRAG_FACTOR_PERIOD = CLOCKS_PER_SEC*2;

        int lastAccumulatedCalories;
        clock_t nextAccumulatedCalories;
        static const clock_t ACCUMULATED_CALORIES_PERIOD = CLOCKS_PER_SEC*1;

        int lastCaloriesPerHour; //coupled to pace


    public:
        ErgValueCache(Erg *theErg) : lastHeartRate(0)
        {
            erg=theErg;
            reset();
        }

        void reset();

        unsigned short getHeartRate( clock_t now );
        unsigned short getAverageHeartRateEntireWorkout() { return averageHeartRateEntireWorkout.aAverage(); }
        unsigned short getPower( clock_t now );
        unsigned short getAverageWattsEntireWorkout() { return averageWattsEntireWorkout.aAverage(); }
        unsigned short getCadence( clock_t now );
        unsigned short getPaceInSecondsPer500( clock_t now );
        unsigned short getDragFactor( clock_t now );
        int getAccumulatedCalories( clock_t now );
        int getCaloriesPerHour(clock_t now) { return lastCaloriesPerHour; }




};


class Erg
{
    ErgValueCache cache;
    static const char *workoutStateText[];
    static const char *intervalTypeText[];

    UINT16_T unitAddress;
    char verPtr[PM3_BUFFER_SIZE];
    char serialNum[PM3_BUFFER_SIZE];

    int metersGoal; // 
    int timeGoal; // in seconds

    int workoutType;
    int lastStrokeState;
    int numStrokesIn;

    //for metersPerStroke;
    int metersPerStroke;
    int metersAtStartOfStroke;


    // dat for hasStartedRowing
    bool _hasStartedRowing;
    bool _hasSeenStrokeState0;


    SlideRatio slideRatioCalculator;

    ErgNet ergNet;
    ErgType ergType;

    #ifdef TIME_UPDATESTATE
        clock_t times[NUM_TIMES]; 
        int currentTimeIndex;
    #endif

    bool debug;

public:

    

    Erg( int aUnitAddress);

    inline ErgType getErgType() { return ergType; }
   
    float getRequiredFirmwareVersion();

    // 1 - show errors on screen, 0 do not show them
    void enableScreenErrorMode( int showErrors );

    // setup erg for that many meters and show screen
    // 0 = default split for given distance
    void doFixedDistance(int meters, int split=0);

    // setup erg for that many seconds and show screen
    // 0 = default split for given time
    void doFixedTime(int hours, int min, int seconds, int split=0);

    
    //TODO: renable when info back from c2
    //void doJustRow(int split=500);


    int hasStartedRowing();

    // reset the data used to calculate when the row has started
    void resetHasStartedRowing();

    unsigned short getCadence();
    unsigned short getPaceInSecondsPer500();
    void getPace( int &paceMinutes, int &paceSecs);

    unsigned short getPower();

    unsigned short getHeartRate();
    int getAccumulatedCalories();

    // meters must be >= 100
    inline void setSplitDurationInMeters(unsigned int meters)
    {
        setSplitDuration(128, meters);
    }


    // unit is seconds (while .01s are actually sent to pm3)
    inline void setSplitDurationInSeconds(unsigned int seconds)
    {
        setSplitDuration(0, seconds * 100);
    }


    // set split duration units are meters if timeDistance=128, .01seconds if timeDistance=128
    // meters split must be >= 100
    void setSplitDuration(unsigned short timeDistance, unsigned int duration);

    void setHorizontal(unsigned int meters);
    void setWork(unsigned short hours, unsigned short minutes, unsigned short seconds);
    void setPower(unsigned int power);


    unsigned short getDragFactor();



    //time stuff
    unsigned long int getTargetTime(); // returns the number of seconds in a fixed-time piece or ErgState::NO_TIME if the workout does not have this, only valid after call to start()
    unsigned long int getDisplayedTime(); // in seconds as displayed on PM3, pm3 command, may count up or down, continues to count up/down if rower stops on a set distance/time piece, stops counting on just row if rower stops, only valid after call to start()
    unsigned long int getSecondsIntoThePiece(); // time into piece, csafe, counts up always, will stop counting if rower stops on a set time/dist piece, goes to zero if just row and rower stops
    void getTimeIntoThePiece( int *hours, int *min, int *sec); // same as getSecondsIntoThePiece but broken out

    // distance
    unsigned long int getTargetDistance(); // returns the number of meters in a fixed-distance piece or ErgState::NO_METERS if the workout does not have this
    unsigned long getDistanceRowedSoFar(); // meters rowed, always counts up
    unsigned long int getDisplayedMeters(); // echos displayed meters on PM3, may count down





    void goReady();
    void goIdle();
    void goInUse();
    void goFinished();

    unsigned short getStrokeState();
    const char *getStrokeStateText();

    unsigned short getWorkoutType();
    const char *getWorkoutTypeText();
    const char *getWorkoutTypeText(unsigned short workoutType);


/* State of workout logic: 
    0.  Waiting to begin  
    1.  Workout row 
    2.  Countdown pause 
    3.  Interval rest 
    4.  Work time interval 
    5.  Work distance interval 
    6.  Rest interval end to work time interval begin 
    7.  Rest interval end to work distance interval begin 
    8.  Work time interval end to rest interval begin 
    9.  Work distance interval end to rest interval begin  
    10.  Workout end 
    11.  Workout terminate 
    12.  Workout logged 
    13.  Workout rearm 
*/
    unsigned short getWorkoutState();
    const char *getWorkoutStateText();
    const char *getWorkoutStateText(int workoutState);


    unsigned short getWorkoutIntervalType();
    const char *getWorkoutIntervalTypeText();
    const char *getWorkoutIntervalTypeText(unsigned short intervalType);

    unsigned short getWorkoutIntervalCount();
        

    //return interval rest time in seconds
    int getRestTime();








    void setMetersGoal(unsigned long int meters);

    // set workout time
    void setTimeGoal(int hours, int min, int seconds);

    // tell pmx to display given program, 0 = whatever was just programmed
    void setProgram(unsigned short int id=0);


    const char *getFirmwareVersion();
    const char *getSerialNumber();


    // begin a new piece, required to initialize workout parameters that are only available before the user starts rowing
    void start(); 

    // update ergState object at given address by pulling data from the 
    // real ergometer.  Note: this is intentionally a pointer and not a
    // reference due to the needs of the main client (wrapper for Python)
    void updateState( ErgState *ergState);

    // reset the erg wrapper in preparation for a new workout
    // this clears things like running averages, cached values, etc, 
    // but does not not reset the Performance Monitor on the real erg
    void reset();


    // return true if row if compiled with trial limits in place, false if not
    inline int isLimited() 
    { 
        #ifdef LIMITED
            return 1;
        #else
            return 0;
        #endif
    }


    void log(const char *format, ...);




protected:

    int LICENSE_HAS_TIMEDOUT;  // ACTUALLY the HAS_TIMEDOUT value, if true will check for max meters/time in
    int HAS_TIMEDOUT_THIS_EXEC;  // nozero if has already passed limits this exec - this avoids 'reset' problem
    int NUM_CSAFE_CHECKS; // check CSAFe every this many calls

    bool setJustRow;

    #ifdef LIMITED
    inline bool rowShouldBeLimited(int metersIn=0, int secondsIn=0)
    {
        return ((LICENSE_HAS_TIMEDOUT || HAS_TIMEDOUT_THIS_EXEC) && (metersIn >= METERS_IN_LIMIT || secondsIn >= SECONDS_IN_LIMIT));
    }
    #else
        #define rowShouldBeLimited(x,y) false
    #endif



            
private:
    
    void  doCsafeCommand( const char *memo, UINT16_T cmdSize, UINT32_T *cmdData, UINT16_T& rspDataSize, UINT32_T *rspData);

};



#endif
