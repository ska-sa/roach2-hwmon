/*
 * alarmhandler.c
 *
 *  Created on: 23 May 2014
 *      Author: rijandn
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/reboot.h>
#include <unistd.h>
#include "alarm.h"
#include "r2hwmond.h"
#include "log.h"
#include "katcl.h"
#include "katcp.h"

#define RPC_TIMEOUT_MS	(5000)

static int send_katcp_command(const char *command)
{
    struct katcl_line *l = NULL;
    int  retVal;

    /* connect to a remote machine, arg is "server:port" where ":port is optional" */
    l = create_name_rpc_katcl(R2HWMOND_SERVER);
    if (l == NULL) {
        fprintf(stderr, "Unable to create client connection to server: %s\n", strerror(errno));
        return 1;
    }

    /* send the request, with RPC_TIMEOUT_MS timeout. Here we don't pass any parameters */
    retVal = send_rpc_katcl(l, RPC_TIMEOUT_MS, KATCP_FLAG_FIRST | KATCP_FLAG_LAST | KATCP_FLAG_STRING, command, NULL);

    /* result is 0 if the reply returns "ok", 1 if it failed and -1 if things went wrong doing IO or otherwise */
#if 0
    int i = 0;
    char *ptr = NULL;
    printf("result of request is %d\n", retVal);

    /* you can examine the content of the reply with the following functions */
    int total = arg_count_katcl(l);
    printf("have %d arguments in reply\n", total);

    for (i = 0; i < total; i++) {
        /* for binary data use the arg_buffer_katcl, string will stop at the first occurrence of a \0 */
        ptr = arg_string_katcl(l, i);
        printf("reply[%d] is <%s>\n", i, ptr);
    }

    fflush(stdout);
#endif

    destroy_rpc_katcl(l);

    return retVal;
}

int alarm_handler(const char *chipName, const char *label)
{
    int retVal = 0;

	if (strcmp(chipName, "max1668-i2c-0-18") == 0) {
    	if (strcmp(label, "temp3") == 0) {
    		/* fpga temp alarm triggered */
    		log_message(KATCP_LEVEL_WARN, "fpga alarm triggered...unloading fpga");
    		retVal = send_katcp_command("?progdev");
    	} else if (strcmp(label, "temp2") == 0) {
    		/* powerpc temp alarm triggered */
    		log_message(KATCP_LEVEL_WARN, "powerpc alarm triggered...shutting down");
    		sync();
    		retVal = reboot(RB_POWER_OFF);
    		if (retVal < 0) {
    			log_message(KATCP_LEVEL_WARN, "could not shut the system down.");
    		}
    	}
    }

#if 0
	if (retVal) {
    	sync();
    	reboot(RB_POWER_OFF);
    }
#endif

    return retVal;
}


