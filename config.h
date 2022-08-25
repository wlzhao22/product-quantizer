#ifndef CONFIG_H
#define CONFIG_H

/*****************************************************************
Please edit this file when compile the code in different platforms

******************************************************************/

/**/
#ifndef LINUX
#define LINUX
#endif
/**/

#ifdef LINUX
#define RM_CMMD "rm -f "
#else
#define RM_CMMD "del "
#endif

#endif
