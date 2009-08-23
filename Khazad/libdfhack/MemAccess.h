#ifndef PROCESSUTIL_H_INCLUDED
#define PROCESSUTIL_H_INCLUDED

#ifdef LINUX_BUILD
    #include "LinuxMemAccess.h"
#else
    #include "WindowsMemAccess.h"
#endif

#endif // PROCESSUTIL_H_INCLUDED
