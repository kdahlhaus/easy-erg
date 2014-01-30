#include <iostream>
#include <time.h>


/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0;


#include "ErgNet.h"
#include "Pm3Erg.h"
#include "CIIHeaders.h"

using namespace std;

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void onErgError( int errorCode, char *note, char *errorName, char *errorText)
{
    printf("onErgError called:\n%d\n%s\n%s\n%s\n", errorCode, note, errorName, errorText);
    exit(1);
}





int main()
{
    try
    {
        ErgNet::setErrorCallback( onErgError );

        ErgNet *net = ErgNet::getInstance();

        Pm3Erg erg = Pm3Erg(0);

        erg.reset();
        erg.goFinished();
        erg.goIdle();
        erg.setHorizontal(5000);
        erg.setSplitDurationInMeters(0);
        erg.setProgram(0);
        erg.goInUse();

        ErgState state;

        while (1)
        {
            erg.updateState(&state);
            cout << "numStrokes: " << state.numStrokesIn << "  meters/stroke: " << state.metersPerStroke << "\n"; 
        }
    }
    catch (int error)
    {
        printf("caught error:%d", error);
        return 1;
    }
    return 0;

}
