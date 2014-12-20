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
    
        ErgNet net = ErgNet();
        cout << "net.discoverErgs() = ";
        int numErgs = net.discoverErgs();
        cout << numErgs << "\n";

        for (int address=0; address<numErgs; address++ )
        {
            cout << "\nFor erg on address " << address << ":\n";
    
            Pm3Erg erg = Pm3Erg(address);
            cout << "   easy-erg detected a PM" << net.getErgType(address) << "\n";
            cout << "   serial #: " << erg.getSerialNumber() << "\n";
            cout << "   firmware version: " << erg.getFirmwareVersion() << "\n";
        }
        
        return 0;
    }
    catch (int error)
    {
        printf("caught error:%d", error);
        return 1;
    }
}
