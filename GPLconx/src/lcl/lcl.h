/* ****************************************************************************
   |      File: lcl.h
   |      Contains: basic defs and main include
   |      Created: 28.02.1998
   |      Last Changed: 14.07.1999
   |      Version: 0.0
   |         
   |      ToDo:
   |
   **************************************************************************** */


#ifndef _LCL_H_
#define _LCL_H_

#ifndef _REENTRANT
#define _REENTRANT
#define _THREAD_SAFE
#endif

#include <iostream.h>

#ifndef FALSE
   #define FALSE 0
   #define TRUE 1
#endif

#ifndef ISORNOT
   #define ISORNOT(s) (s ? "" : "not")
#endif

#ifndef YESNO
   #define YESNO(s) (s ? "yes" : "no")
#endif

#ifndef CLASSNAME
   #define CLASSNAME(x) \
public: \
  virtual const char * className() {return x;};
#endif

#ifndef MIN
   #define MIN(v1,v2) ((v1>v2) ? v2 : v1)
#endif

#ifndef MAX
   #define MAX(v1,v2) ((v1>v2) ? v1 : v2)
#endif

#ifndef MSTHROW
   #ifdef _WIN32
      #define MSTHROW(v) return *(( v *)NULL)
   #else
      #define MSTHROW(v)
   #endif // _WIN32
#endif // MSTHROW

#if defined ( _WIN32 ) && ! defined ( __NUTC__ )

  #include <winsock2.h>
typedef HANDLE LThreadID;
typedef HANDLE LFileDescriptor;

#else

  #include <pthread.h>
typedef pthread_t LThreadID;
typedef int LFileDescriptor;

#endif

#endif // _LCL_H_

