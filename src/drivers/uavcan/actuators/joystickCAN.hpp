#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/enautic/JoyCAN.hpp>
#include <uORB/topics/manual_control_setpoint.h>

#include <perf/perf_counter.h>

class UavcanJoystickCAN
{
	public:
		UavcanJoystickCAN(uavcan::INode &node);
		~UavcanJoystickCAN();

		int init();

		void sendTelemetry(int output);

	private:

		struct manual_control_setpoint_s manual_control_setpoint;
		int setpoint_sub;

		// void joystick_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoyCAN> &msg);
		void periodic_update(const uavcan::TimerEvent &);
		typedef uavcan::MethodBinder
		<UavcanJoystickCAN *, void (UavcanJoystickCAN::*)(const uavcan::TimerEvent &)> TimerCbBinder;

		uavcan::INode 		&_node;
		uavcan::Publisher<uavcan::equipment::enautic::JoyCAN> _uavcan_pub_joyCAN_send;
		uavcan::TimerEventForwarder<TimerCbBinder> _timer;
};
