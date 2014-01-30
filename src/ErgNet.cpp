#include "ErgNet.h"
#include "CIIHeaders.h"
#include <iostream>



ErgNet *ErgNet::theErgNet = 0;

using namespace std;

int isInitialized = 0;
int discoveredErgs = 0;

void initializeDlls()
{
    if (!isInitialized)
    {
        // initialize the dlls
        ERRCODE_T errorCode;

        errorCode = tkcmdsetDDI_init();
        if (errorCode)
        {
            logDdiError(errorCode, "tkcmdsetDDI_init");
            throw errorCode; 
        }

        errorCode=csafe.tkcmdsetCSAFE_init_protocol(1000);
        if (errorCode)
        {
            logCSafeError(errorCode, "tkcmdsetCSAFE_init_protocol");
            throw errorCode;
        }

        isInitialized = 1;
    }
}


ErgType ErgNet::ergTypeAtAddress[MAX_NUM_ERGS];
ErgType ErgNet::currentErgType = UNKNOWN_ERGTYPE;
ERRCODE_T ErgNet::errorCode;
char ErgNet::errorBuffer[256];
UINT16_T ErgNet::baseAddress[ERGTYPE_MAX+1];





ErgNet::ErgNet()
{
    if (!isInitialized)
    {
        initializeDlls();
    }

    if (!discoveredErgs)
    {
        discoverErgs();
    }
}


ERRCODE_T ErgNet::aErrorCode()
{ return errorCode; }


const char *NoErrorName = "NO_ERROR";
const char *NoErrorText = "No error occurred";

const char *ErgNet::getErrorName()
{
    if (!errorCode) { return NoErrorName; }
    tkcmdsetDDI_get_error_name( errorCode, errorBuffer, 256 );
    return errorBuffer;
}

const char *ErgNet::getErrorText()
{
    if (!errorCode) { return NoErrorText; }
    tkcmdsetDDI_get_error_text( errorCode, errorBuffer, 256 );
    return errorBuffer;
}

ErgType ErgNet::getErgType(int address)
{
    if (address<MAX_NUM_ERGS && address>-1)
    {
        return ergTypeAtAddress[address];
    }
    return UNKNOWN_ERGTYPE;
}

void ErgNet::resetErgInfo()
{
    int i;

    for (int i=0; i<=ERGTYPE_MAX; i++)
    { baseAddress[i]=0; }

    for (int i=0; i<MAX_NUM_ERGS; i++)
    {  ergTypeAtAddress[i]=UNKNOWN_ERGTYPE; }
}


int ErgNet::discoverErgsOfType( ErgType ergType )
//requires base address set but does not update it
{
    UINT16_T numPms=0;
    switch (ergType)
    {
        case PM3_ERGTYPE:
            errorCode = tkcmdsetDDI_discover_pm3s( TKCMDSET_PM3_PRODUCT_NAME2, baseAddress[ergType], &numPms);
            break;
        case PM4_ERGTYPE:
            errorCode = tkcmdsetDDI_discover_pm3s( TKCMDSET_PM4_PRODUCT_NAME, baseAddress[ergType], &numPms);
            break;
        default:
            return 0;
    }
    currentErgType = ergType;
    if (errorCode == -10201) // just means no ergs found
    {
        errorCode = 0;
    }
    if (errorCode)
    {
        return -1;
    }
    return numPms;
}
    


int ErgNet::discoverErgs()
{
    int i;

    resetErgInfo();

    int numPm3s=0;

    //check for pm3s
    baseAddress[PM3_ERGTYPE]=0;
    numPm3s = discoverErgsOfType( PM3_ERGTYPE );
    if (numPm3s==-1)
    {
        return -1;
    }
    for (i=0; i<numPm3s & i<MAX_NUM_ERGS; i++)
    {
        ergTypeAtAddress[i] = PM3_ERGTYPE;
    }

    //check for pm4s
    int numPm4s=0;
    baseAddress[PM4_ERGTYPE]=numPm3s;
    numPm4s = discoverErgsOfType( PM4_ERGTYPE );
    if (numPm4s==-1)
    {
        return -1;
    }
    for (i=0; i<numPm4s & i<MAX_NUM_ERGS; i++)
    {
        ergTypeAtAddress[i] = PM4_ERGTYPE;
    }

    discoveredErgs = 1;

    return (int)(numPm3s+numPm4s);
}

void ErgNet::setForErgType(ErgType ergType)
{
    if ( currentErgType != ergType )
    {
        discoverErgsOfType(ergType);
    }
}

void ErgNet::setForErg(int address)
{
    if (currentErgType != getErgType(address))
    {
        setForErgType(getErgType(address));
    }
}


