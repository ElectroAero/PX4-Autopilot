#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/enautic/ControlSet.hpp>
#include <uORB/topics/manual_control_setpoint.h>

#include <perf/perf_counter.h>

class UavcanJoystickCommand
{

	public:
		UavcanJoystickCommand(uavcan::INode &node);
		~UavcanJoystickCommand();

		int init();

		void sendTelemetry(int output);

	private:

		struct manual_control_setpoint_s controlSetpoints;
		int controlSet_sub;


		void periodic_update(const uavcan::TimerEvent &);

		typedef uavcan::MethodBinder<UavcanJoystickCommand *,
			void (UavcanJoystickCommand::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::ControlSet>&)> StatusCbBinder;

		typedef uavcan::MethodBinder<UavcanJoystickCommand *,
			void (UavcanJoystickCommand::*)(const uavcan::TimerEvent &)> TimerCbBinder;

		manual_control_setpoint_s _manual_control_setpoint{};

		uavcan::INode	&_node;
		uavcan::Publisher<uavcan::equipment::enautic::ControlSet>	_uavcan_pub_joycommand;
		uavcan::TimerEventForwarder<TimerCbBinder> _timer;
};
