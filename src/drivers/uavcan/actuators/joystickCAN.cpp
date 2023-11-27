#include "joystickCAN.hpp"
#include <systemlib/err.h>

UavcanJoystickCAN::UavcanJoystickCAN(uavcan::INode &node):
	_node(node),
	_uavcan_pub_joyCAN_send(node),
	_timer(node)
{

}

UavcanJoystickCAN::~UavcanJoystickCAN() {

}

int UavcanJoystickCAN::init() {

	/*
	 * Setup timer and call back function for periodic updates
	 */
	_timer.setCallback(TimerCbBinder(this, &UavcanJoystickCAN::periodic_update));
	return 0;

	// int res = _uavcan_sub_status.start(StatusCbBinder(this, &UavcanJoystickCAN::joystick_status_sub_cb));

	// if (res < 0) {
	// 	PX4_ERR("Joystick status sub failed %i", res);
	// 	return res;
	// }
}

void UavcanJoystickCAN::sendTelemetry(int value) {

	if (!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		memset(&manual_control_setpoint, 0, sizeof(manual_control_setpoint));
	}
}

void UavcanJoystickCAN::periodic_update(const uavcan::TimerEvent &) {

	static int started = 0;

	if(started == 0) {
		setpoint_sub = orb_subscribe(ORB_ID(manual_control_setpoint));

		started++;
	}

	//check for any new update
	bool setpointUpdated;
	orb_check(setpoint_sub, &setpointUpdated);

	//copy a local copy, can also check for any change with above boolean
	if(setpointUpdated) {
		orb_copy(ORB_ID(manual_control_setpoint), setpoint_sub, &manual_control_setpoint);
	}

	//Test what comes out of this uORB topic
	float moment1 = (((float)manual_control_setpoint.roll) + 1) *128;
	float moment2 = (((float)manual_control_setpoint.pitch) + 1) *128;
	float moment3 = (((float)manual_control_setpoint.yaw) + 1) *128;
	float moment4 = (((float)manual_control_setpoint.throttle) + 1) *128;

	int test1 = ((int)moment1);
	int test2 = ((int)moment2);
	int test3 = ((int)moment3);
	int test4 = ((int)moment4);
	uavcan::equipment::joystick::JoyCAN msg;

	msg.roll = test1;
	msg.pitch = test2;
	msg.yaw = test3;
	msg.throttle = test4;

	// Broadcast command at MAX_RATE_HZ
	(void)_uavcan_pub_joyCAN_send.broadcast(msg);
}
