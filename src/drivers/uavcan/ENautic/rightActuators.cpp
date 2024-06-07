#include "rightActuators.hpp"
#include <systemlib/err.h>

UavcanRightActuators::UavcanRightActuators(uavcan::INode &node):
	_node(node),
	_uavcan_pub_right_actuators_send(node),
	_timer(node)
{

}

UavcanRightActuators::~UavcanRightActuators()
{

}

int UavcanRightActuators::init() {

	/*
	 * Setup timer and call back function for periodic updates
	 */
	_timer.setCallback(TimerCbBinder(this, &UavcanRightActuators::periodic_update));
	return 0;
}

void UavcanRightActuators::sendTelemetry(int value) {

	if (!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		//memset(&battStatus, 0, sizeof(battStatus));
		memset(&actuatorOutputs, 0, sizeof(actuatorOutputs));
		//memset(&vehicleAttitude, 0, sizeof(vehicleAttitude));
		//memset(&distanceSensor, 0, sizeof(distanceSensor));
	}
}

void UavcanRightActuators::periodic_update(const uavcan::TimerEvent &) {

	static int started = 0;

	if(started == 0) {
		actOut_sub = orb_subscribe(ORB_ID(actuator_outputs));
		started++;
	}

	//check for any new update
	bool actOutputsUpdated;
	orb_check(actOut_sub, &actOutputsUpdated);

	//copy a local copy, can also check for any change with above boolean
	if(actOutputsUpdated) {
		orb_copy(ORB_ID(actuator_outputs), actOut_sub, &actuatorOutputs);
	}

	//Remap motor & servo to 0-200
	int RM = ((((int) actuatorOutputs.output[1]) - 1500)/5);
	int FRS = ((((int) actuatorOutputs.output[5]) - 1500)/5);
	int RRS = ((((int) actuatorOutputs.output[7]) - 1500)/5);

	uavcan::equipment::enautic::RightActuators msg;

	// Broadcast command at MAX_RATE_HZ
	msg.RM = RM;
	msg.FRS = FRS;
	msg.RRS = RRS;
	msg.rightServoReq = 1;
	(void)_uavcan_pub_right_actuators_send.broadcast(msg);
}
