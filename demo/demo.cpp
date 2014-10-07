//#include "CIIHeaders.h"

#include <iostream>

/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0; 


#include "ErgNet.h"
#include "Pm3Erg.h"
#include "CIIHeaders.h"

using namespace std;

void onErgError( int errorCode, char *note, char *errorName, char *errorText)
{
    printf("onErgError called:\nerrorcode: %d\nnote: %s\nerror name: %s\nerror text: %s\n", errorCode, note, errorName, errorText);
    exit(1);
}


int main()
{
    try
    {
        ErgNet::setErrorCallback( onErgError );
    
        ErgNet *net = ErgNet::getInstance();
        cout << "net.discoverErgs() = " << net->discoverErgs() << "\n";



        cout << "tkcmdsetDDI_get_dll_version() " << tkcmdsetDDI_get_dll_version() << "\n";
        cout << "tkcmdsetUSB_get_dll_version() " << tkcmdsetUSB_get_dll_version() << "\n";
        cout << "tkcmdsetCSAFE_get_dll_version() " << csafe.tkcmdsetCSAFE_get_dll_version() << "\n";

        Pm3Erg erg = Pm3Erg(0);
        cout << "serial #: " << erg.getSerialNumber() << "\n";
        cout << "firmware version: " << erg.getFirmwareVersion() << "\n";


        // following is the sequence of commands to set and show a fixed distance workout
        if (0)
        {
            erg.reset();
            erg.goFinished();
            erg.goIdle();
            //erg.setHorizontal(5000);
            //erg.setSplitDurationInMeters(0);
            erg.setProgram(0);
            erg.goInUse();
        }

        //erg.doFixedDistance(2400);
        //erg.doFixedTime(0, 15, 0);

        //cout << "workout type: " << erg.getWorkoutType() << "  " << erg.getWorkoutTypeText() << "\n";

        if (0)
        {
            erg.reset();
            erg.goFinished();
            erg.goIdle();
            //erg.setHorizontal(0);
            //erg.setSplitDurationInMeters(500);
            erg.setProgram(0);
            erg.goInUse();
        } 

        erg.reset();
        erg.goFinished();
        erg.goIdle();
        erg.goReady();
        erg.goInUse();
        cout << "workout type: " << erg.getWorkoutType() << "  " << erg.getWorkoutTypeText() << "\n";
        cout.flush();

        while (1)
        {
            cout << erg.hasStartedRowing() << "  " << erg.getStrokeState() << " " << erg.getStrokeStateText() << "\n";
        }


        
        return 0;
    }
    catch (int error)
    {
        printf("caught error:%d", error);
        return 1;
    }

}
