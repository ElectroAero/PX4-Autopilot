/**
 * @author Shami Mohdar <shami@enautic.co>
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/enautic/JoyCAN.hpp>
#include <uavcan/equipment/enautic/InputRc.hpp>
#include <lib/perf/perf_counter.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/input_rc.h>
#include <uORB/topics/joystick_status.h>
#include <drivers/drv_hrt.h>


class UavcanJoystickController
{
public:
	static constexpr int MAX_ACTUATORS = 8; //same as ESC Status uORB topic
	static constexpr unsigned MAX_RATE_HZ = 400;
	static constexpr uint16_t DISARMED_OUTPUT_VALUE = UINT16_MAX;

	UavcanJoystickController(uavcan::INode &node);
	~UavcanJoystickController() = default;

	int init();

	void sendTelemetry(int output);

	void update_outputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs);

	joystick_status_s &joystick_status() { return _joystick_status; }

private:

	void joy_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::InputRc> &msg);

	typedef uavcan::MethodBinder<UavcanJoystickController *,
		void (UavcanJoystickController::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::InputRc>&)> JoyCbBinder;

	typedef uavcan::MethodBinder<UavcanJoystickController *,
		void (UavcanJoystickController::*)(const uavcan::TimerEvent &)> TimerCbBinder;

	joystick_status_s	_joystick_status{};

	uORB::PublicationMulti<joystick_status_s> _joystick_status_pub{ORB_ID(joystick_status)};

	uavcan::MonotonicTime							_prev_cmd_pub;
	uavcan::INode								&_node;
	uavcan::Publisher<uavcan::equipment::enautic::JoyCAN>			_uavcan_pub_raw_cmd;
	uavcan::Subscriber<uavcan::equipment::enautic::InputRc, JoyCbBinder>	_uavcan_sub_status;
};
