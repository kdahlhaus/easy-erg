#ifndef CSAFEDLL_H
#define CSAFEDLL_H

class CSafeDll;

#include "CIIHeaders.h"
#include <windows.h>

class CSafeDll
{

protected:
    HMODULE hdll;

    typedef UINT16_T (*pUint16Void)();
    typedef ERRCODE_T (*pCommand)( UINT16_T, UINT16_T, UINT32_T *, UINT16_T *, UINT32_T *);
    typedef ERRCODE_T (*pFunc16)( UINT16_T );
    typedef ERRCODE_T (*pErrorData)( ERRCODE_T, char *, UINT16_T );


    pUint16Void getDllVer;
    pCommand csafeCommand;
    pFunc16 initProto;
    pErrorData errorName;
    pErrorData errorText;


public:

    CSafeDll();
    ~CSafeDll();

    inline UINT16_T tkcmdsetCSAFE_get_dll_version()
    { return getDllVer(); }

    inline ERRCODE_T tkcmdsetCSAFE_command(UINT16_T unit_address, UINT16_T cmd_data_size, UINT32_T cmd_data[], UINT16_T *rsp_data_size, UINT32_T rsp_data[])
    { return csafeCommand( unit_address, cmd_data_size, cmd_data, rsp_data_size, rsp_data); }

    inline ERRCODE_T tkcmdsetCSAFE_init_protocol( UINT16_T timeout )
    { return initProto( timeout ); }

    inline ERRCODE_T tkcmdsetCSAFE_get_error_name( ERRCODE_T errorCode, char *namePtr, UINT16_T namelen)
    { return errorName( errorCode, namePtr, namelen); }

    inline ERRCODE_T tkcmdsetCSAFE_get_error_text( ERRCODE_T errorCode, char *textPtr, UINT16_T textlen)
    { return errorText( errorCode, textPtr, textlen); }

};

#endif
