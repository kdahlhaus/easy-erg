#include "CIIHeaders.h"
#include <iostream>

CSafeDll csafe;

using namespace std;

#define BL 128

ErgErrorCallbackFunction errorCallback=0;

void setCIIErrorCallback( ErgErrorCallbackFunction newFunc )
{
    errorCallback = newFunc;
}

void logGenericError( int code, char *note, char *name, char *text)
{
    if (errorCallback)
    {
        errorCallback( (int)code, (char *)note, name, text );
    }
    else
    {
        cerr << "\nError: " << code << " " << name << " " << text << " " << note;
    }
}

void logCSafeError(ERRCODE_T error, const char *note)
{
    cout << "logCSafeError\n";
    char name[BL];
    char text[BL];

    csafe.tkcmdsetCSAFE_get_error_name( error, name, BL);
    csafe.tkcmdsetCSAFE_get_error_text( error, text, BL);
    
    if (errorCallback)
    {
        errorCallback( (int)error, (char *)note, name, text );
    }
    else
    {
        cout << "Error at " << note << " : " << name << ": " << text << "\n";
    }
}

void logDdiError(ERRCODE_T error, const char *note)
{
    char name[BL];
    char text[BL];

    tkcmdsetDDI_get_error_name( error, name, BL);
    tkcmdsetDDI_get_error_text( error, text, BL);
    
    if (errorCallback)
    {
        errorCallback( (int)error, (char *)note, name, text );
    }
    else
    {
        cout << "Error at " << note << " : " << name << ": " << text << "\n";
    }
}



