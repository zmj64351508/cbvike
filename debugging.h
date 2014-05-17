#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

//enable debug when define LOGGING
#ifdef LOGGING
    #define LOGIT wxLogDebug
#else
    #define LOGIT
#endif

#ifdef LOGGING
    #undef LOGIT
   #ifdef __WXMSW__
    #define TRAP asm("int3")
    #define LOGIT wxLogMessage
   #endif
   #ifdef __WXGTK__
    #define TRAP asm("int3")
    #define LOGIT wxLogMessage
   #endif
   #ifdef __WXMAC__
    #define TRAP asm("trap")
    #define LOGIT wxLogMessage
   #endif
#endif

#endif  //__DEBUGGING_H__
