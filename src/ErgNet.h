#ifndef ErgNet_h
#define ErgNet_h

#include "CIIHeaders.h"
#include <map>

using std::map;


// erg types
enum ErgType
{
    UNKNOWN_ERGTYPE = 0,
    PM3_ERGTYPE = 3,
    PM4_ERGTYPE = 4,
    PM5_ERGTYPE = 5
};
#define ERGTYPE_MAX 5


class ErgNet
{

protected:

    map<int, ErgType> foundErgs;
    UINT16_T currentBaseAddress;

    // reset what we know about found ergs
    void resetErgInfo();

    virtual int discoverErgsOfType( ErgType );

    static ERRCODE_T errorCode; // error is at the DLL level, so it must be shared across all ErgNet instances
    static char errorBuffer[256];


    // virtual to allow mocking with googlemock in tests
    virtual ERRCODE_T callDDIDiscoverPm3s( INT8_T *product_name, UINT16_T starting_address, UINT16_T *num_units);
    

public:

    ErgNet();
    virtual ~ErgNet(){};

    const char *ergTypeToTkCmdSetProductName(ErgType type);


    //returns erg type for the given net address
    ErgType getErgType(int address);

    //checks for ergs on the net returns number of ergs found, or -1 if there was an error
    int discoverErgs();


    // SDK error state is global, so these are all static
    static void setErrorCallback( ErgErrorCallbackFunction newCallback );
    static ERRCODE_T aErrorCode();
    static const char *getErrorName();
    static const char *getErrorText();

};

#endif
