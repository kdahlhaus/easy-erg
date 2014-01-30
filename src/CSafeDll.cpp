#include "CSafeDll.h"

/*
LIBRARY RPPM3Csafe.dll
EXPORTS
??4CPM3Csafe@@QAEAAV0@ABV0@@Z	@1
?tkcmdsetCSAFE_command@@YAFGGQAKPAG0@Z	@2
?tkcmdsetCSAFE_get_cmd_data_types@@YAFEPAE0QAE01@Z	@3
?tkcmdsetCSAFE_get_cmd_name@@YAFEPADG@Z	@4
?tkcmdsetCSAFE_get_cmd_text@@YAFEPADG@Z	@5
?tkcmdsetCSAFE_get_dll_version@@YAGXZ	@6
?tkcmdsetCSAFE_get_error_name@@YAFFPADG@Z	@7
?tkcmdsetCSAFE_get_error_text@@YAFFPADG@Z	@8
?tkcmdsetCSAFE_get_status@@YAEXZ	@9
?tkcmdsetCSAFE_init_protocol@@YAFG@Z	@10
?tkcmdsetCSAFE_read_logblk8@@YAFGKKPAEPAK@Z	@11
?tkcmdsetCSAFE_write_logblk16@@YAFGKPAGK@Z	@12
*/


CSafeDll::CSafeDll()
{
   hdll = LoadLibrary("RPPM3CSAFE.dll"); 
   if (!hdll)
   {
       throw "Unable to initialize RPPM3Csafe.dll";
   }

   getDllVer=(pUint16Void)GetProcAddress(hdll, (const char *)5);
   csafeCommand=(pCommand)GetProcAddress(hdll, (const char *)1); 
   initProto=(pFunc16)GetProcAddress( hdll, (const char *)9); 
   errorName=(pErrorData)GetProcAddress( hdll, (const char *)6); 
   errorText=(pErrorData)GetProcAddress( hdll, (const char *)7); 
}


CSafeDll::~CSafeDll()
{
   FreeLibrary(hdll); 
}

