#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

//enable debug when define LOGGING
#ifdef LOGGING
    #define LOGIT wxLogMessage
#else
    #define LOGIT(fmt, arg...)
#endif

#endif  //__DEBUGGING_H__
