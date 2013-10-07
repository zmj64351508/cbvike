/////////////////////////////////////////////////////////////////////////////
// Name:        debugging.h
// Purpose:     enable/disable debugging
// Author:      Jia Wei
// Created:     2011/12/17
// Copyright:   (c) Jia Wei
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

//enable debug when define LOGGING
#define LOGGING 1
#define LOGIT wxLogDebug


#if LOGGING
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
