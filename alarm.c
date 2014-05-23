/*
 * alarmhandler.c
 *
 *  Created on: 23 May 2014
 *      Author: rijandn
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "alarm.h"
#include "log.h"
#include "katcl.h"
#include "katcp.h"

static int send_katcp_command(const char *command)
{
	struct katcl_line *l = NULL;
	int result, total, i;
	char *ptr;

	/* connect to a remote machine, arg is "server:port" where ":port is optional" */
	l = create_name_rpc_katcl("localhost:7147");
	if (l == NULL) {
		fprintf(stderr, "Unable to create client connection to server: %s\n", strerror(errno));
		return 1;
	}

	/* send the request, with 5000ms timeout. Here we don't pass any parameters */
	result = send_rpc_katcl(l, 5000, KATCP_FLAG_FIRST | KATCP_FLAG_LAST | KATCP_FLAG_STRING, command, NULL);

	/* result is 0 if the reply returns "ok", 1 if it failed and -1 if things went wrong doing IO or otherwise */
	printf("result of request is %d\n", result);

	/* you can examine the content of the reply with the following functions */
	total = arg_count_katcl(l);
	printf("have %d arguments in reply\n", total);
	for (i = 0; i < total; i++) {
		/* for binary data use the arg_buffer_katcl, string will stop at the first occurrence of a \0 */
		ptr = arg_string_katcl(l, i);
		printf("reply[%d] is <%s>\n", i, ptr);
	}

	fflush(stdout);

	destroy_rpc_katcl(l);

	return 0;
}

int alarm_handler(const char *chipName, const char *label)
{


	/* fpga temp alarm triggered */
	if (!(strcmp(chipName, "max1668-i2c-0-18") && strcmp(label, "temp3"))) {
		log_message(KATCP_LEVEL_WARN, "fpga alarm triggered...unloading fpga");
		send_katcp_command("?progdev");
	}

	return 0;
}


