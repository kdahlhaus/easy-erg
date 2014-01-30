#ifndef ErgNet_h
#define ErgNet_h

#include "CIIHeaders.h"


// erg types
enum ErgType
{
    UNKNOWN_ERGTYPE = 0,
    PM3_ERGTYPE = 3,
    PM4_ERGTYPE = 4
};
#define ERGTYPE_MAX 4


#define MAX_NUM_ERGS 64

class ErgNet
{

protected:

    static ErgNet *theErgNet;
    static ErgType currentErgType;
    static ErgType ergTypeAtAddress[MAX_NUM_ERGS];


    static void resetErgInfo();
    static int discoverErgsOfType( ErgType );

    static ERRCODE_T errorCode;
    static char errorBuffer[256];
    static UINT16_T baseAddress[ERGTYPE_MAX+1];



public:

    ErgNet();


    static void setForErg(int address);
    //sets comm link for erg at given addres ( makes the type ok)

            
    static ErgType getErgType(int address);
    //returns erg type for the given net address

    static int discoverErgs();
    //checks for ergs on the net returns number of ergs found, or -1 if there was an error


    static void setForErgType(ErgType ergType);
    //ensures the comm. link is initialized for this erg type


    static void setErrorCallback( ErgErrorCallbackFunction newCallback )
    { setCIIErrorCallback( newCallback ); } 

    static ErgNet *getInstance()
    {
        if (!theErgNet)
        {
            theErgNet = new ErgNet();
        }
        return theErgNet;
    }

    

    ERRCODE_T aErrorCode();

    const char *getErrorName();
    const char *getErrorText();

};


#endif
