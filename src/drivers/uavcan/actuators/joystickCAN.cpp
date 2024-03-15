/**
 * @author Shami Mohdar <shami@enautic.co>
 * Currently does not work
 * For more current version, look under sensor directory
 */

#include "joystickCAN.hpp"
#include <drivers/drv_hrt.h>

using namespace time_literals;

UavcanJoystickController::UavcanJoystickController(uavcan::INode &node) :
	_node(node),
	_uavcan_pub_raw_cmd(node),
	_uavcan_sub_status(node)
{
	_uavcan_pub_raw_cmd.setPriority(uavcan::TransferPriority::NumericallyMin);	// Sets highest priority in work queue
}

int UavcanJoystickController::init()	{
	int res = _uavcan_sub_status.start(JoyCbBinder(this,
	&UavcanJoystickController::joy_status_sub_cb));


	// _joystick_status.timestamp = hrt_absolute_time();
	// _joystick_status.x_axis	= 1; //msg.Xaxis;
	// _joystick_status.y_axis	= 2; //msg.Yaxis;
	// _joystick_status.deadman = 3; //msg.MOB;
	// _joystick_status.lbutton = 4; //msg.leftButton;
	// _joystick_status.rbutton = 5; //msg.rightButton;
	// _joystick_status_pub.publish(_joystick_status);

	// _joystick_status_pub.advertise();

	return res;
}

// void UavcanJoystickController::sendTelemetry(int value) {

// 	if (!_timer.isRunning()) {
// 		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
// 		//memset(&battStatus, 0, sizeof(battStatus));
// 		memset(&actuatorOutputs, 0, sizeof(actuatorOutputs));
// 		//memset(&vehicleAttitude, 0, sizeof(vehicleAttitude));
// 		//memset(&distanceSensor, 0, sizeof(distanceSensor));
// 	}
// }

void UavcanJoystickController::update_outputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs) {

}

// UavcanJoyBridge::update_outputs(uint16_t outputs) {
// 	const auto timestamp = _node.getMonotonicTime();

// 	if ((timestamp - _prev_cmd_pub).toUSec() < (1000000 / MAX_RATE_HZ)) {
// 		return;
// 	}

// 	_prev_cmd_pub = timestamp;

// 	uavcan::equipment::enautic::JoyCAN msg;

// 	_uavcan_pub_raw_cmd.broadcast(msg);
// }



void UavcanJoystickController::joy_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::InputRc> &msg)
{
	 static int test = 0x00;
	// auto &ref = _input_rc.joystick_status;

	// ref.timestamp	= hrt_x	absolute_time();
	// ref.x_axis	= test; //msg.Xaxis;
	// ref.y_axis	= 2; //msg.Yaxis;
	// ref.deadman	= 3; //msg.MOB;
	// ref.lbutton	= 4; //msg.leftButton;
	// ref.rbutton	= 5; //msg.rightButton;

	// _input_rc.timestamp = hrt_absolute_time();
	// _input_rc.input_source = input_rc_s::RC_INPUT_SOURCE_CAN;
	// _joystick_status_pub.publish(_input_rc);



	_joystick_status.timestamp = hrt_absolute_time();
	_joystick_status.x_axis	= test; //msg.Xaxis;
	_joystick_status.y_axis	= 2; //msg.Yaxis;
	_joystick_status.deadman = 3; //msg.MOB;
	_joystick_status.lbutton = 4; //msg.leftButton;
	_joystick_status.rbutton = 5; //msg.rightButton;
	_joystick_status_pub.publish(_joystick_status);

	test++;
}
