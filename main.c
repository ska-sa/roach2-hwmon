#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include "fork-parent.h"
#include "sensorlib.h"
#include "sensord.h"
#include "log.h"
#include "sense.h"

#define SCAN_INTERVAL_S    (5)
#define READ_INTERVAL_S   (15)

static volatile sig_atomic_t doScan = 1;

void signalHandler(int signalNumber)
{
	switch (signalNumber) {
		case SIGTERM:
		case SIGINT:
			doScan = 0;
			break;

		default:
			return; 
	}
}

/* c-function prototypes */
static int daemonize(void);

int main(int argc, char *argv[])
{
	struct sigaction sa;
	unsigned int counter = 0;
	int scanValue = 0, readValue = 0, sleepTime = 0;

	printf("The name of this program is '%s'.\n", argv[0]);
	printf("The process ID is %d.\n", (int)getpid());

	/* initialize the signal handler */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &signalHandler;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	chips_parse();

	/* run process by default in the foreground */
	/* \todo add -d daemon option */
	if (argc != 1) {
		if (daemonize() < 0) {
			printf("Could not daemonize...\n");
			return EXIT_FAILURE;
		}
	}

	if (sensorlib_load() < 0) {
		fprintf(stderr, "Could not initialize sensor library.\n");
		return EXIT_FAILURE;
	}

	if (log_init() < 0) {
		fprintf(stderr, "Could not initialize katcl message logic.\n");
	}

	initKnownChips();

	/* main process loop ... */
	while (doScan) {
		printf("start scan...");
		printf("done.\n");
		fflush(stdout);

		if (READ_INTERVAL_S && (readValue <= 0)) {
			sense_readChips();
			readValue += READ_INTERVAL_S;
		}

		if (SCAN_INTERVAL_S && (scanValue <= 0)) {
			sense_scanChips();
			scanValue += SCAN_INTERVAL_S;
		}

		log_message(KATCP_LEVEL_INFO, "Scan done %d.\n", counter);
		counter++;

		/* calculate the sleeptime, since we have a read and scan interval */
		int a = SCAN_INTERVAL_S ? scanValue : INT_MAX;
		int b = READ_INTERVAL_S ? readValue : INT_MAX;
		sleepTime = (a < b) ? a : b;

		sleep(sleepTime);

		scanValue -= sleepTime;
		readValue -= sleepTime;

	}

	/* clean-up */
	printf("Performing clean-up...\n");
	log_message(KATCP_LEVEL_INFO, "Shutting down...\n");
	freeKnownChips();
	sensorlib_unload();
	log_cleanup();

	return EXIT_SUCCESS;
}

/* daemonize the process by detaching itself from the parent */
int daemonize(void)
{
	if (fork_parent() < 0) {
        fprintf(stderr, "Unable to launch child process.\n");
		return EXIT_FAILURE;
	}

	/* print initialization strings via parent stderr pipe */
	fprintf(stderr, "Initializing daemon...\n");
	fprintf(stderr, "The daemon process id is %d.\n", (int)getpid());

	/* change daemon working directory to root */
	if (chdir("/") < 0) {
		fprintf(stderr, "Could not change working directory to root.\n");
		return EXIT_FAILURE;
	}
	
	/* close the pipe which will close the parent... */
	fflush(stderr);
	close(STDERR_FILENO);

	return 0;
}
