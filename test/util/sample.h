
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SAMPLE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SAMPLE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINDOWS
#ifdef SAMPLE_EXPORTS
#define SAMPLE_API __declspec(dllexport)
#else
#define SAMPLE_API __declspec(dllimport)
#endif
#else
#define SAMPLE_API
#endif

#include <string>

extern "C" {
  SAMPLE_API void reverse_w(wchar_t* pString);
  SAMPLE_API void reverse_c(char* pString);
  SAMPLE_API int getModuleVersion(void);
}
