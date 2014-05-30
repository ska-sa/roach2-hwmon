/*
 * sensors.c
 *
 *  Created on: 30 May 2014
 *      Author: rijandn
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "katcp_sensors.h"
#include "r2hwmond.h"
#include "log.h"

int katcp_sensors_register(void)
{
	struct katcl_line *l = NULL;

	l = create_name_rpc_katcl(R2HWMOND_SERVER);
	if (l == NULL) {
        fprintf(stderr, "Unable to create client connection to server: %s\n", strerror(errno));
        return -1;
    }

	log_message(KATCP_LEVEL_INFO, "Registering hardware sensors");

	/* register sensor(s) */
	append_string_katcl(l, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "?sensor-list");
	append_string_katcl(l, KATCP_FLAG_STRING, "r2hwmond.fpgatemp");
	append_string_katcl(l, KATCP_FLAG_STRING, "fpga temperature");
	append_string_katcl(l, KATCP_FLAG_STRING, "millidegrees");
	append_string_katcl(l, KATCP_FLAG_STRING, "integer");
	append_string_katcl(l, KATCP_FLAG_STRING, "0");   // integer type?
	append_string_katcl(l, KATCP_FLAG_LAST | KATCP_FLAG_STRING, "80000");   // integer type?

	if (l) {
		destroy_rpc_katcl(l);
	}

	return 0;
}
