#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOG_ERROR	1
#define LOG_INFO	2

/* logopen() attempts to open the logfile for writing. If logfile cannot be opened with write permissions, we send a message to stoudt 
explaining this. logopen() returns 0 on success and 1 on failures. */
int logopen(char *filename, int loglevel);

/**/
int logclose(char *filename);

/**/
int loginfo(char *logmsg);

/**/
int logerror(char *logmsg);

#endif

