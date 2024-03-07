#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/enautic/JoyCAN.hpp>
#include <uORB/topics/rc_channels.h>

#include <perf/perf_counter.h>

class UavcanJoystickListener
{

	public:
		UavcanJoystickListener(uavcan::INode &node);
		~UavcanJoystickListener();

		int init();

		void sendTelemetry(int output);

	private:

		struct rc_channels_s rcChannels;
		int rcOut_sub;


		// /**
		// * Joystick status message reception will be reported via this callback
		// */
		// void joystick_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener> &msg);

		// /**
		// * Checks joystick is not randomly disconnected
		// */
		// uint8_t check_joystick_status();

		// typedef uavcan::MethodBinder<UavcanJoystickListener *,
		// 	void (UavcanJoystickListener::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener>&)> StatusCbBinder;

		// typedef uavcan::MethodBinder<UavcanJoystickListener *,
		// 	void (UavcanJoystickListener::*)(const uavcan::TimerEvent &)> TimerCbBinder;

		// rc_channels_s _rc_channels{};

		void periodic_update(const uavcan::TimerEvent &);
		typedef uavcan::MethodBinder
		<UavcanJoystickListener *, void (UavcanJoystickListener::*)(const uavcan::TimerEvent &)> TimerCbBinder;

		uavcan::INode		&_node;
		uavcan::Publisher<uavcan::equipment::enautic::JoyCAN> _uavcan_pub_joyOutput;
		uavcan::TimerEventForwarder<TimerCbBinder> _timer;
};
