#include "ErgNet.h"
#include "CIIHeaders.h"
#include <iostream>





using namespace std;

int isInitialized = 0;



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


ERRCODE_T ErgNet::errorCode;
char ErgNet::errorBuffer[256];





ErgNet::ErgNet()
{
    if (!isInitialized)
    {
        initializeDlls();
    }
    resetErgInfo();
}

void ErgNet::setErrorCallback( ErgErrorCallbackFunction newCallback )
{
    setCIIErrorCallback( newCallback );
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
    if (address<foundErgs.size())
    {
        return foundErgs[address];
    }
    return UNKNOWN_ERGTYPE;
}



int ErgNet::discoverErgs()
{
    resetErgInfo();
    discoverErgsOfType(PM3_ERGTYPE);
    discoverErgsOfType(PM4_ERGTYPE);
    discoverErgsOfType(PM5_ERGTYPE);
    return foundErgs.size();
}

void ErgNet::resetErgInfo(void)
{
    currentBaseAddress=0;
    foundErgs.clear();
}

int ErgNet::discoverErgsOfType(ErgType erg_type_to_look_for)
{
    UINT16_T num_found_in_total=0;
    errorCode = callDDIDiscoverPm3s( (INT8_T *)ergTypeToTkCmdSetProductName(erg_type_to_look_for),  currentBaseAddress, &num_found_in_total);
    if (errorCode==-10201) // no ergs found
    {
        return 0;
    }
    if (errorCode)
    {
        return -1; // some error 
    }

    //found some ergs

    int num_found_before = foundErgs.size();
    int num_newly_found_ergs = num_found_in_total-num_found_before;

    //add the newly found ergs
    for (int i=currentBaseAddress; i<currentBaseAddress+num_newly_found_ergs; i++)
    {
        foundErgs[i]=erg_type_to_look_for;
    }

    //update the currentBaseAddress
    currentBaseAddress += num_newly_found_ergs;

    return num_newly_found_ergs;
}

ERRCODE_T ErgNet::callDDIDiscoverPm3s( INT8_T *product_name, UINT16_T starting_address, UINT16_T *num_units)
{
    return tkcmdsetDDI_discover_pm3s( product_name, starting_address, num_units );
}

 

const char *ErgNet::ergTypeToTkCmdSetProductName(ErgType type)
{
    switch (type)
    {
        case PM3_ERGTYPE: return TKCMDSET_PM3_PRODUCT_NAME2;
        case PM4_ERGTYPE: return TKCMDSET_PM4_PRODUCT_NAME;
        case PM5_ERGTYPE: return TKCMDSET_PM5_PRODUCT_NAME;
    }
    return "<error: unknown erg type has n o TKCMDSET name>";
}



