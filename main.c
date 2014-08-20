#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <getopt.h>
#include "r2hwmond.h"
#include "fork-parent.h"
#include "sensorlib.h"
#include "sensord.h"
#include "log.h"
#include "sense.h"
#include "r2hwmon_ver.h"
#include "version.h"

static volatile sig_atomic_t doScan = 1;
static volatile sig_atomic_t pipeBroken = 0;

struct r2hwmond_configuration r2hwmond_cfg = {
		.daemon 	 = 0,
		.cfgFile 	 = 0,
		.scanTime 	 = R2HWMOND_SCAN_INTERVAL_S,
		.updateTime  = R2HWMOND_UPDATE_INTERVAL_S,
};

/* c-function prototypes */
static int daemonize(void);
static void signalHandler(int signalNumber);
static void print_version(void);
static void print_usage(void);

static void signalHandler(int signalNumber)
{
    switch (signalNumber) {
        case SIGTERM:
        case SIGINT:
            doScan = 0;
            break;

        case SIGPIPE:
        	/* tcpborphserver most probably stopped ... disable logging */
        	pipeBroken = 1;
        	log_disable();
        	break;

        default:
            return; 
    }
}

int main(int argc, char *argv[])
{
    int opt = 0;
	struct sigaction sa;
    unsigned int counter = 0;
    int scanValue = 0, readValue = 0, sleepTime = 0;
    int ret = 0;

#if 0
    printf("The name of this program is '%s'.\n", argv[0]);
    printf("The process ID is %d.\n", (int)getpid());
#endif

    /* initialize the signal handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signalHandler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    /* process command line arguments */
    while ((opt = getopt(argc, argv, "dc:s:u:v")) != -1) {
    	switch (opt) {
    	case 'd':
    		r2hwmond_cfg.daemon = 1;
    		break;
    	case 'c':
    		r2hwmond_cfg.cfgFile = 1;
    		r2hwmond_cfg.sensorCfgFile = optarg;
    		break;
    	case 's':
    		r2hwmond_cfg.scanTime = atoi(optarg);
    		break;
    	case 'u':
    		r2hwmond_cfg.updateTime = atoi(optarg);
    		break;
    	case 'v':
    		print_version();
    		exit(EXIT_SUCCESS);
    		break;
    	case '?':
    		print_usage();
    		exit(EXIT_FAILURE);
    	default:
    		print_usage();
    		exit(EXIT_FAILURE);
    	}
    }

    /* run process by default in the foreground */
    if (r2hwmond_cfg.daemon) {
        if (daemonize() < 0) {
            printf("Could not daemonize...\n");
            return EXIT_FAILURE;
        }
    } else {
        /* initialize katcp logs */
        if (log_init() < 0) {
            fprintf(stderr, "Could not initialize katcl message logic.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* load sensor lib with configuration file if it was specified */
    if (r2hwmond_cfg.cfgFile) {
    	ret = sensorlib_load(r2hwmond_cfg.sensorCfgFile);
    } else {
    	ret = sensorlib_load(NULL);
    }
    if (ret < 0) {
    	fprintf(stderr, "Could not initialize sensor library.\n");
    	exit(EXIT_FAILURE);
    }

    /* prepare sensor library parameters */
    chips_parse();
    initKnownChips();

    /* katcp sensor list get initialized on first sense_readChips() */
    sense_readChips();

    /* main process loop ... */
    while (doScan) {

#if 0
        printf("start scan...");
#endif

        /* read/update sensor values */
        if (r2hwmond_cfg.updateTime && (readValue <= 0)) {
            sense_readChips();
            readValue += r2hwmond_cfg.updateTime;
        }

        /* scan sensors for alarms */
        if (r2hwmond_cfg.scanTime && (scanValue <= 0)) {
            sense_scanChips();
            scanValue += r2hwmond_cfg.scanTime;
        }

        log_message(KATCP_LEVEL_TRACE, "scan %d done.\n", counter);
        counter++;

#if 0
        printf("done.\n");
        fflush(stdout);
#endif

        /* calculate the sleeptime, since we have a read and scan interval */
        int a = r2hwmond_cfg.scanTime ? scanValue : INT_MAX;
        int b = r2hwmond_cfg.updateTime ? readValue : INT_MAX;
        sleepTime = (a < b) ? a : b;

        sleep(sleepTime);
        scanValue -= sleepTime;
        readValue -= sleepTime;
    }

    /* clean-up */
#if 0
    printf("Performing clean-up...\n");
#endif
    log_message(KATCP_LEVEL_INFO, "Shutting down...\n");
    freeKnownChips();
    sensorlib_unload();
    log_cleanup();

    return EXIT_SUCCESS;
}

/* daemonize the process by detaching itself from the parent */
static int daemonize(void)
{
    if (fork_parent() < 0) {
        fprintf(stderr, "Unable to launch child process.\n");
        return EXIT_FAILURE;
    }

    /* print initialization strings via parent stderr pipe */
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

static void print_version(void)
{
	printf("%s version %d.%d.%d with libsensors version %s\n", R2HWMOND_PROG,
			R2HWMOND_VER_MAJOR, R2HWMOND_VER_MINOR, R2HWMOND_VER_BUGFIX, LM_VERSION);
	printf("Build %s %s\n", __DATE__, __TIME__);
}

static void print_usage(void)
{
	fprintf(stderr, "Usage: %s [-d daemon] [-c sensorConfigFile] [-s scanTime]"
			" [-u updateTime] [-v version]\n", R2HWMOND_PROG);
}
