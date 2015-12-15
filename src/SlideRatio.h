#ifndef SlideRatio_h
#define SlideRatio_h

#include "CIIHeaders.h"
#include "ErgState.h"
#include "time.h"

// calculates slide ratio in ergstate

class SlideRatio
{


public:

    SlideRatio() { reset(); }
    void reset(void);
    void update(ErgState *ergState);

protected:

    int state;
    clock_t startedDriveAt;
    clock_t startedRecoveryAt;
    float ratio;
    
};



#endif
