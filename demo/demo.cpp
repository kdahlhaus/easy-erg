//#include "CIIHeaders.h"

#include <iostream>

/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0; 


#include "ErgNet.h"
#include "Erg.h"
#include "CIIHeaders.h"

using namespace std;


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
        cout << "net.discoverErgs() = " << net.discoverErgs() << "\n";



        cout << "tkcmdsetDDI_get_dll_version() " << tkcmdsetDDI_get_dll_version() << "\n";
        cout << "tkcmdsetUSB_get_dll_version() " << tkcmdsetUSB_get_dll_version() << "\n";
        cout << "tkcmdsetCSAFE_get_dll_version() " << csafe.tkcmdsetCSAFE_get_dll_version() << "\n";

        Erg erg = Erg(0);
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

        cout << "erg.reset():\n";
        erg.reset();

        cout << "erg.goFinished():\n";
        erg.goFinished();

        cout << "erg.goIdle():\n";
        erg.goIdle();

        cout << "erg.goReady():\n";
        erg.goReady();

        cout << "erg.goInUse():\n";
        erg.goInUse();

        cout << "workout type: " << erg.getWorkoutType() 
            << "  " << erg.getWorkoutTypeText() 
            << "\n";
        cout.flush();

        while (1)
        {
            cout << erg.hasStartedRowing() << "  " << erg.getStrokeState() << " " << erg.getStrokeStateText() << "\n";
        }


        
        return 0;
    }
    catch (int error)
    {
        cerr << "caught error: " << error;
        return 1;
    }

}
