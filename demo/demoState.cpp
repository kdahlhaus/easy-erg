#include <iostream>
#include <time.h>


/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0;


#include "ErgNet.h"
#include "Erg.h"
#include "CIIHeaders.h"

using namespace std;

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void onErgError( int errorCode, char *note, char *errorName, char *errorText)
{
    cerr << "onErgError called:\nerrorcode:" << errorCode << "\nnote: " << note << "\nerror name: " << errorName << "\nerror text: " << errorText << "\n";
    exit(1);
}





int main()
{
    try
    {
        ErgNet::setErrorCallback( onErgError );

        ErgNet net = ErgNet();
        net.discoverErgs();

        Erg erg = Erg(0);

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
        cerr << "caught error: " << error;
        return 1;
    }
    return 0;

}
