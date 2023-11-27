/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

#include "UavcanSubscriberBase.hpp"

#include <uavcan/equipment/joystick/JoystickListener.hpp>

#include <uORB/Publication.hpp>
#include <uORB/manual_control_setpoint.h>

namespace uavcannode
{

class JoystickListener;

typedef uavcan::MethodBinder<JoystickListener *,
	void (JoystickListener::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener>&)>
	JoystickListenerBinder;

class JoystickListener :
	public UavcanSubscriberBase,
	private uavcan::Subscriber<uavcan::equipment::joystick::JoystickListener, JoystickListenerBinder>
{
public:
	JoystickListener(uavcan::INode &node) :
		UavcanSubscriberBase(uavcan::equipment::joystick::JoystickListener::DefaultDataTypeID),
		uavcan::Subscriber<uavcan::equipment::joystick::JoystickListener, JoystickListenerBinder>(node)
	{}

	bool init()
	{
		if (start(JoystickListenerBinder(this, &JoystickListener::callback)) < 0) {
			PX4_ERR("uavcan::equipment::joystick::JoystickListener subscription failed");
			return false;
		}

		return true;
	}

	void PrintInfo() const override
	{
		printf("\t%s:%d -> %s\n",
		       uavcan::equipment::joystick::JoystickListener::getDataTypeFullName(),
		       uavcan::equipment::joystick::JoystickListener::DefaultDataTypeID,
		       _manual_control_setpoint_pub.get_topic()->o_name);
	}

private:
	void callback(const uavcan::ReceivedDataStructure<uavcan::equipment::joystick::JoystickListener> &msg)
	{

		manual_control_setpoint_s manual_control_setpoint;

		manual_control_setpoint.timestamp = hrt_absolute_time();
		manual_control_setpoint.timestamp_sample = manual_control_setpoint.timestamp;

		manual_control_setpoint.roll = msg.roll;
		manual_control_setpoint.pitch = msg.pitch;
		manual_control_setpoint.yaw = msg.yaw;
		manual_control_setpoint.throttle = msg.throttle;

		_manual_control_setpoint_pub.publish(manual_control_setpoint);

	}

	uORB::Publication<manual_control_setpoint_s> _manual_control_setpoint_pub{ORB_ID(manual_control_setpoint)};
};
} // namespace uavcannode
