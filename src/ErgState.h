#ifndef ErgState_h
#define ErgState_h

class ErgState
{
    public:

        static const long int NO_METERS; // value indicating 'no meters', such as meters left on a 'just row'
        static const long int NO_TIME; // value indicating 'no time', such as time remaining on a 'just row'
        static const long int NO_PERCENT; // value indicating 'no percent', such as %in on a 'just row'

    void reset() 
    {
        metersIn=0;
        metersToGo=0;
        timeIn=0;
        timeToGo=0;
        percentComplete=0;
        cadence=0;
        split=0;
        watts=0;
        numStrokesIn=0;
        strokeState=0;
        slideRatio=0.0;
        heartRate=0;
        workoutType=0;
        dragFactor=0;
        accumulatedCalories=0;
        caloriesPerHour=0;
        metersPerStroke=0;

        averageHeartRateEntireWorkout=0;
        averageWattsEntireWorkout=0;

        workoutState=0;
        workoutIntervalCount=0;
        workoutIntervalType=0;
        restTime=0; 
    }

    
    long int metersIn;
    long int metersToGo;
    long int timeIn;  // in seconds
    long int timeToGo;
    int percentComplete;
    int cadence;
    int split; // #sec/500m
    int watts;
    long int numStrokesIn;
    int strokeState;
    float slideRatio;
    int heartRate;
    int workoutType;
    int dragFactor;
    int accumulatedCalories;
    int caloriesPerHour;
    int metersPerStroke;
    int averageHeartRateEntireWorkout;
    int averageWattsEntireWorkout;

    int workoutState;
    int workoutIntervalCount;
    int workoutIntervalType;
    int restTime;




};

//for python wrappers since it's hard to access static members of a class from pyrex
inline long int get_NO_METERS() { return ErgState::NO_METERS; }
inline long int get_NO_TIME() { return ErgState::NO_METERS; }
inline long int get_NO_PERCENT() { return ErgState::NO_PERCENT; }

#endif

