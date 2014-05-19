#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "fork-parent.h"

int main(int argc, char *argv[])
{
	printf("The name of this program is '%s'.\n", argv[0]);
	printf("The parent process ID is %d.\n", (int)getpid());

	/* daemonize the process... */
	if (fork_parent() < 0) {
        fprintf(stderr, "unable to launch child process\n");
		return -1;
	}

	/* print init strings via parent stderr pipe */
    fprintf(stderr, "Initializing daemon...\n");
	fprintf(stderr, "The daemon process id is %d.\n", (int)getpid());

	/* change daemon working directory to root */
	if (chdir("/") < 0) {
		fprintf(stderr, "Could not change working directory to root.\n");
		return -1;
	}
	
	/* close the pipe which wil close the parent... */
	fflush(stderr);
	close(STDERR_FILENO);

    /* main process loop ... */
    while(1) {
		/* print status message to katcp log here */
		sleep(10);
	}	

	/* should not end up here.... */
	return 0;
}

