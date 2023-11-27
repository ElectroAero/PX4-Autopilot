#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/joystick/1313.JoystickListener.hpp>
#include <uORB/topics/manual_control_setpoint.h>

#include <perf/perf_counter.h>

class UavcanJoystickListener
{

	public:
		UavcanJoystickListener(uavcan::INode &node);
		~UavcanJoystickListener() = default;

		int init();

		void update_outputs()

		manual_control_setpoint_s &manual_control_setpoint() { return _manual_control_setpoint; }

	private:
		/**
		* Joystick status message reception will be reported via this callback
		*/
		void joystick_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener> &msg);

		/**
		* Checks joystick is not randomly disconnected
		*/
		uint8_t check_joystick_status();

		typedef uavcan::MethodBinder<UavcanJoystickListener *,
			void (UavcanJoystickListener::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener>&)> StatusCbBinder;

		typedef uavcan::MethodBinder<UavcanJoystickListener *,
			void (UavcanJoystickListener::*)(const uavcan::TimerEvent &)> TimerCbBinder;

		manual_control_setpoint_s _manual_control_setpoint{};

		uORB::PublicationMulti<manual_control_setpoint_s> _joystick_status_pub{ORB_ID(manual_control_setpoint)};

		uavcan::MonotonicTime									_prev_cmd_pub;
		uavcan::INode										&_node;
		uavcan::Publisher<uavcan::equipment::joystick::JoystickListener>			_uavcan_pub_raw_cmd;
		uavcan::Subscriber<uavcan::equipment::joystick::JoystickListener, StatusCbBinder>	_uavcan_sub_status;
};
