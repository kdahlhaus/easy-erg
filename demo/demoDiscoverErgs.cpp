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
        cout << "net.discoverErgs() = ";
        int numErgs = net.discoverErgs();
        cout << numErgs << "\n";

        for (int address=0; address<numErgs; address++ )
        {
            cout << "\nFor erg on address " << address << ":\n";
    
            Erg erg = Erg(address);
            cout << "   easy-erg detected a PM" << net.getErgType(address) << "\n";
            cout << "   serial #: " << erg.getSerialNumber() << "\n";
            cout << "   firmware version: " << erg.getFirmwareVersion() << "\n";
        }
        
        return 0;
    }
    catch (int error)
    {
        cerr << "caught error: " << error;
        return 1;
    }
}
