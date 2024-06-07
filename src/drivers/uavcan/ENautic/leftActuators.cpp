#include "leftActuators.hpp"
#include <systemlib/err.h>

UavcanLeftActuators::UavcanLeftActuators(uavcan::INode &node):
	_node(node),
	_uavcan_pub_left_actuators_send(node),
	_timer(node)
{

}

UavcanLeftActuators::~UavcanLeftActuators()
{

}

int UavcanLeftActuators::init() {

	/*
	 * Setup timer and call back function for periodic updates
	 */
	_timer.setCallback(TimerCbBinder(this, &UavcanLeftActuators::periodic_update));
	return 0;
}

void UavcanLeftActuators::sendTelemetry(int value) {

	if (!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		//memset(&battStatus, 0, sizeof(battStatus));
		memset(&actuatorOutputs, 0, sizeof(actuatorOutputs));
		//memset(&vehicleAttitude, 0, sizeof(vehicleAttitude));
		//memset(&distanceSensor, 0, sizeof(distanceSensor));
	}
}

void UavcanLeftActuators::periodic_update(const uavcan::TimerEvent &) {

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
	int LM = ((((int) actuatorOutputs.output[0]) - 1500)/5);
	int FLS = ((((int) actuatorOutputs.output[4]) - 1500)/5);
	int RLS = ((((int) actuatorOutputs.output[6]) - 1500)/5);

	uavcan::equipment::enautic::LeftActuators msg;

	// Broadcast command at MAX_RATE_HZ
	//Left actuator commands
	msg.LM = LM;
	msg.FLS = FLS;
	msg.RLS = RLS;
	msg.leftServoReq = 1;
	(void)_uavcan_pub_left_actuators_send.broadcast(msg);
}
