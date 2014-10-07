#ifndef CIIHeaders_h
#define CIIHeaders_h

#ifdef __cplusplus
extern "C"
{
#endif 

#include "PM3DDICP.h"
/* #include "PM3Csafe.h" */
#include "PM3USBCP.h"

#ifdef __cplusplus
}
#endif 

#include "CSafeDll.h"


/* 1/21/07 added temp support for PM4 */
#ifndef TKCMDSET_PM4_PRODUCT_NAME
#define TKCMDSET_PM4_PRODUCT_NAME "Concept2 Performance Monitor 4 (PM4)" 
#endif



extern CSafeDll csafe;
extern void logCSafeError(ERRCODE_T error, const char *note="");
extern void logDdiError(ERRCODE_T error, const char *note="");

typedef void (*ErgErrorCallbackFunction)(int errorCode,  char *note, char *errorName, char *errorText);
extern void setCIIErrorCallback( ErgErrorCallbackFunction newFunc );

extern void logGenericError( int code, char *note, char *name, char *text);
// log generic error - must pass in all info


#endif
