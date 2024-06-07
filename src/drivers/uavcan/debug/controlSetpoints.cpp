#include "controlSetpoints.hpp"
#include <systemlib/err.h>

UavcanJoystickCommand::UavcanJoystickCommand(uavcan::INode &node):
	_node(node),
	_uavcan_pub_joycommand(node),
	_timer(node)
{

}

UavcanJoystickCommand::~UavcanJoystickCommand() {

}

int UavcanJoystickCommand::init() {

	_timer.setCallback(TimerCbBinder(this, &UavcanJoystickCommand::periodic_update));
	return 0;

}

void UavcanJoystickCommand::sendTelemetry(int value)
{
	if (!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		memset(&controlSetpoints, 0, sizeof(controlSetpoints));
	}
}

void UavcanJoystickCommand::periodic_update(const uavcan::TimerEvent &)
{
	static int started = 0;

	if (started == 0) {
		controlSet_sub = orb_subscribe(ORB_ID(manual_control_setpoint));
		started ++;
	}

	bool controlSetpointUpdated;
	orb_check(controlSet_sub, &controlSetpointUpdated);

	if(controlSetpointUpdated) {
		orb_copy(ORB_ID(manual_control_setpoint), controlSet_sub, &controlSetpoints);
	}

	int roll = (int)(controlSetpoints.roll * 100);
	int pitch = (int)(controlSetpoints.pitch * 100);
	int yaw = (int)(controlSetpoints.yaw * 100);
	int throttle = (int)(controlSetpoints.throttle * 100);

	uavcan::equipment::enautic::ControlSet msg;

	msg.roll = roll;
	msg.pitch = pitch;
	msg.yaw = yaw;
	msg.throttle = throttle;

	(void)_uavcan_pub_joycommand.broadcast(msg);
}
