#pragma once

#include "joystickListener.hpp"
#include <systemlib/err.h>

UAVCANJoystickListener::UAVCANJoystickListener(uavcan::INode &node):
	_node(node),
	_uavcan_pub_raw_cmd(node),
	_uavcan_sub_status(node)
{
	_uavcan_pub_raw_cmd.setPriority(uavcan::TransferPriority::NumericallyMin); // Highest priority
}

int UAVCANJoystickListener::init() {

	int res = _uavcan_sub_status.start(StatusCbBinder(this, &UavcanEscController::joystick_status_sub_cb));

	if (res < 0) {
		PX4_ERR("Joystick status sub failed %i", res);
		return res;
	}

	_joystick_status_pub.advertise();

	return res;
}

void UavcanJoystickListener::update_outputs()
{
	const auto timestamp = _node.getMonotonicTime();

	if ((timestamp - _prev_cmd_pub).toUSec() < (1000000 / MAX_RATE_HZ)) {
		return;
	}

	_prev_cmd_pub = timestamp;

	uavcan::equipment::joystick::
}

void UAVCANJoystickListener::joystick_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener> &msg) {
	auto &ref = _joystick_status;

	ref.timestamp	= hrt_absolute_time();
	ref.yaw		= 1000;
	ref.throttle	= 1000;


	_joystick_status.timestamp = hrt_absolute_time();
	_joystick_status_pub.publish(_manual_control_setpoint);
}

uint8_t UAVCANJoystickListener::check_joystick_status()
{
	int joystick_status_flag = 0;
	const hrt_absolute_time now = hrt_absolute_time();

	if(_joystick_status.timestamp > 0 && now - _joystick_status.timestamp < 1200_ms) {
		joystick_status_flag = UINT8_MAX;
	}

	return joystick_status_flag;
}
