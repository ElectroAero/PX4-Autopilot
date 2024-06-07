/**
 * @author Shami Mohdar <shami@enautic.co>
 * @file joystick.hpp
 */

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/gps_inject_data.h>

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/enautic/JoyCAN.hpp>
#include <uORB/topics/manual_control_setpoint.h>

#include <uORB/topics/joystick_status.h>
#include <uORB/PublicationMulti.hpp>

#include <lib/perf/perf_counter.h>

#include "sensor_bridge.hpp"

class UavcanJoystick : public UavcanSensorBridgeBase
{
public:

	joystick_status_s &joystick_status() { return _joystick_status; }

	static const char *const NAME;

	UavcanJoystick(uavcan::INode &node);
	~UavcanJoystick();

	void joy_timer(int value);

	const char *get_name() const override { return NAME; }

	int init() override;

private:

	// void periodic_update(const uavcan::TimerEvent &);

	joystick_status_s	_joystick_status{};
	uORB::PublicationMulti<joystick_status_s> _joystick_status_pub{ORB_ID(joystick_status)};

	struct manual_control_setpoint_s manual_control_setpoint;
	int setpoint_sub;

	/**
	 * GNSS fix message will be reported via this callback.
	 */
	void joy_sub_cb(
		// const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &msg
		);


	typedef uavcan::MethodBinder
	<UavcanJoystick *, void (UavcanJoystick::*)(const uavcan::TimerEvent &)> TimerCbBinder;
	typedef uavcan::MethodBinder
	<UavcanJoystick *, void (UavcanJoystick::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &)> JoyCbBinder;

	uavcan::INode &_node;
	uavcan::Subscriber<uavcan::equipment::enautic::JoyCAN, JoyCbBinder> _sub_joy;
	uavcan::TimerEventForwarder<TimerCbBinder> _timer;
};
