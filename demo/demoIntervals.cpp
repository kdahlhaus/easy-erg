//#include "CIIHeaders.h"

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

        ErgNet net = ErgNet();
        /*
          cout << "Test Intervals\nnet.discoverErgs() = " << net.discoverErgs() << "\n";

        cout << "tkcmdsetDDI_get_dll_version() " << tkcmdsetDDI_get_dll_version() << "\n";
        cout << "tkcmdsetUSB_get_dll_version() " << tkcmdsetUSB_get_dll_version() << "\n";
        cout << "tkcmdsetCSAFE_get_dll_version() " << csafe.tkcmdsetCSAFE_get_dll_version() << "\n";
        */

        Pm3Erg erg = Pm3Erg(0);
        //cout << "serial #: " << erg.getSerialNumber() << "\n";
        //cout << "firmware version: " << erg.getFirmwareVersion() << "\n";



        //erg.reset();
        //erg.goFinished();
        //erg.goIdle();
        //erg.setHorizontal(5000);
        //erg.setSplitDurationInMeters(0);
        //erg.setProgram(0);
        //erg.goInUse();

        cout << "getWorkoutType,";
        cout << "getWorkoutState,";
        cout << "getWorkoutStateText,";
        cout << "workoutIntervalCount,";
        cout << "workoutIntervalType,";
        cout << "workoutIntervalTypeText,";
        cout << "restTime,";
        cout << "accum calories,";
        cout << "getTargetTime,";
        cout << "getDisplayedTime,";
        cout << "getSecondsIntoThePiece,";
        cout << "getTargetDistance,";
        cout << "getDistanceRowedSoFar,";
        cout << "getDisplayedMeters";
        cout << "\n";

        while (1)
        {
            cout << erg.getWorkoutTypeText() << ",";
            cout << erg.getWorkoutState() << ",";
            cout << erg.getWorkoutStateText() << ",";
            cout << erg.getWorkoutIntervalCount() << ",";
            cout << erg.getWorkoutIntervalType() << ",";
            cout << erg.getWorkoutIntervalTypeText() << ",";
            cout << erg.getRestTime() << ",";
            cout << erg.getAccumulatedCalories() << ",";
            cout << erg.getTargetTime() << ",";
            cout << erg.getDisplayedTime() << ",";
            cout << erg.getSecondsIntoThePiece() << ",";
            cout << erg.getTargetDistance() << ",";
            cout << erg.getDistanceRowedSoFar() << ",";
            cout << erg.getDisplayedMeters() << "";
            cout << "\n";
            sleep(1000);
        }
    }
    catch (int error)
    {
        printf("caught error:%d", error);
        return 1;
    }
    return 0;

}
