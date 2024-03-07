#include "joystickListener.hpp"
#include <systemlib/err.h>

UavcanJoystickListener::UavcanJoystickListener(uavcan::INode &node):
	_node(node),
	_uavcan_pub_joyOutput(node),
	_timer(node)
{

}

UavcanJoystickListener::~UavcanJoystickListener() {

}

int UavcanJoystickListener::init() {
	/*
	 * Setup timer and call back function for periodic updates
	 */
	_timer.setCallback(TimerCbBinder(this, &UavcanJoystickListener::periodic_update));
	return 0;
}

void UavcanJoystickListener::sendTelemetry(int value)
{
	if (!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		memset(&rcChannels, 0, sizeof(rcChannels));
	}
}

void UavcanJoystickListener::periodic_update(const uavcan::TimerEvent &)
{
	static int started = 0;

	if (started == 0) {
		rcOut_sub = orb_subscribe(ORB_ID(rc_channels));
		started ++;
	}

	bool rcChannelsUpdated;
	orb_check(rcOut_sub, &rcChannelsUpdated);

	if(rcChannelsUpdated) {
		orb_copy(ORB_ID(rc_channels), rcOut_sub, &rcChannels);
	}

	int Xaxis = (((int)rcChannels.channels[0] == 1900) ? 255 : (((int)rcChannels.channels[0] - 1100) / 25) * 8);
	int Yaxis = (((int)rcChannels.channels[2] == 1900) ? 255 : (((int)rcChannels.channels[1] - 1100) / 25) * 8);
	int leftButton = (((int)rcChannels.channels[4] == 2000) ? 255 : (((int)rcChannels.channels[5] - 1000) / 125) * 32);
	int rightButton = (((int)rcChannels.channels[5] == 2000) ? 255 : (((int)rcChannels.channels[6] - 1000) / 125) * 32);
	int MOB = (((int)rcChannels.channels[7] == 2000) ? 255 : (((int)rcChannels.channels[4] - 1000) / 125) * 32);

	uavcan::equipment::enautic::JoyCAN msg;

	msg.Xaxis = Xaxis;
	msg.Yaxis = Yaxis;
	msg.leftButton = leftButton;
	msg.rightButton = rightButton;
	msg.MOB = MOB;

	(void)_uavcan_pub_joyOutput.broadcast(msg);
}
