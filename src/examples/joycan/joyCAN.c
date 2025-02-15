/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

 /**
 * @file joyCAN.c
 * Read RC Inputs via UAVCAN
 *
 * @author shami@electronautic.com.au
 */

#include <px4_log.h>
#include <nuttx/config.h>
#include <stdio.h>
#include <errno.h>
#include <uORB/topics/input_rc.h>
#include <uORB/topics/manual_control_setpoint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/tasks.h>
#include <nuttx/sched.h>
//#include <systemlib/systemlib.h>
#include <systemlib/err.h>


static bool thread_should_exit = false; /**< daemon exit flag */
static bool thread_running = false; /**< daemon status flag */
static int daemon_task; /**< Handle of daemon task / thread */


__EXPORT int joyCAN_main(int argc, char *argv[]);

/**
* Mainloop of daemon.
*/
int joyCAN_daemon_app_main(int argc, char *argv[]);

/**
* Print the correct usage.
*/
static void usage(const char *reason);

static void usage(const char *reason) {
	if (reason) {
	 	warnx("%s\n", reason);
	}
 	warnx("usage: daemon {start|stop|status} [-p <additional params>]\n\n");
}


int joyCAN_main(int argc, char *argv[]) {

	if (argc < 2) {
 		usage("Missing command");
 		return 1;
 	}

 	else if (!strcmp(argv[1], "start")) {
 		if (thread_running) {
 			warnx("daemon already running\n");
 			return 0;
 		}

 		thread_should_exit = false;

 		daemon_task = px4_task_spawn_cmd("joyCAN",
 			SCHED_DEFAULT,
 			SCHED_PRIORITY_DEFAULT,
 			2000,
 			joyCAN_daemon_app_main,
 			(argv) ? (char * const *)&argv[2] : (char * const*)NULL);

		return 0;
 	}

 	else if (!strcmp(argv[1], "stop")) {
 		thread_should_exit = true;
 		return 0;
 	}

 	else if (!strcmp(argv[1], "status")) {
 		if (thread_running) {
 			warnx("\trunning\n");
 		}
 		else {
 			warnx("\tnot started\n");
 		}
 		return 0;
 	}
 	else {
 		usage("unrecognized command");
 		return 1;
 	}

 	return 1;
}

int joyCAN_daemon_app_main(int argc, char *argv[]) {

	warnx("[Daemon] starting\n");
	thread_running = true;

	printf("Started Thread! Subscribing to battery UOrb...\n");

	//Structs to contain the system state
	struct input_rc_s rc_input;
	struct manual_control_setpoint_s manual_control_setpoint;
	memset(&rc_input, 0, sizeof(rc_input));
	memset(&manual_control_setpoint, 0, sizeof(manual_control_setpoint));

	//Subscribe to topics
	int rcInput_sub = orb_subscribe(ORB_ID(input_rc));
	int setpoint_sub = orb_subscribe(ORB_ID(manual_control_setpoint));

	while(!thread_should_exit) {

		//check for any new update
		bool rcInputUpdated;
		bool setpointUpdated;
		orb_check(rcInput_sub, &rcInputUpdated);
		orb_check(setpoint_sub, &setpointUpdated);

		//copy a local copy, can also check for any change with above boolean
		orb_copy(ORB_ID(input_rc), rcInput_sub, &rc_input);
		orb_copy(ORB_ID(manual_control_setpoint), setpoint_sub, &manual_control_setpoint);

		printf("Channel 1 = %f | ", (double)rc_input.values[0]);
		printf("Channel 2 = %f | ", (double)rc_input.values[1]);
		printf("Channel 3 = %f | ", (double)rc_input.values[2]);
		printf("Channel 4 = %f | ", (double)rc_input.values[3]);
		printf("Channel 5 = %f | ", (double)rc_input.values[4]);
		printf("Channel 6 = %f\n", (double)rc_input.values[5]);

		printf("Channel 1 = %f | ", (double)manual_control_setpoint.roll);
		printf("Channel 2 = %f | ", (double)manual_control_setpoint.pitch);
		printf("Channel 3 = %f | ", (double)manual_control_setpoint.yaw);
		printf("Channel 4 = %f\n", (double)manual_control_setpoint.throttle);
		printf("\n");
 		sleep(3);
	}

	warnx("[Daemon] exiting.\n");
 	thread_running = false;
 	return OK;
 }
