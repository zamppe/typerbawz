#include "logging.h"
#include <time.h>

/*
#define LOG_ERROR	1
#define LOG_INFO	2
*/


int loglevel = NULL;


FILE *logfile = NULL;


/* logopen() attempts to open the logfile for writing. If logfile cannot be opened with write permissions, we send a message to stoudt 
explaining this. logopen() returns 0 on success and 1 on failures. */
int logopen(char *filename, int loglevel) {
	
	logfile = fopen(filename, "w");
	
	if (logfile == NULL) {
		/* We log to standard output in this function only, so that we at least get a message if the logfile could not be opened. */
		fprintf(stdout, "Error opening logfile for writing.\n");
		return 1;
	}
	
	loglevel = LOG_INFO;	// Change loglevel here by using the defined variables LOG_INFO or LOG_ERROR

	return 0;

	loginfo("Logfile successfully opened with write permissions");
}


int logclose(char *filename) {
	if (fclose(filename) != 0) {
		fprintf(stdout, "Error closing the logfile. This shouldn't happen. Let's hope OS cleans up when main() exits.\n");
		return 1;
	}

	return 0;
}


int loginfo(char *logmsg) {

}



int logerror(char *logmsg) {

}