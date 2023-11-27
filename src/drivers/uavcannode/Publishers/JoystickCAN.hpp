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

#include "UavcanPublisherBase.hpp"

#include <uavcan/equipment/joystick/JoyCAN.hpp>

#include <uORB/SubscriptionCallback.hpp>
#include <uORB/manual_control_setpoint.h>

namespace uavcannode
{

class JoyCAN :
	public UavcanPublisherBase,
	private uORB::SubscriptionCallbackWorkItem,
	private uavcan::Publisher<uavcan::equipment::joystick::JoyCAN>
{
public:
	JoyCAN(px4::WorkItem *work_item, uavcan::INode &node) :
		UavcanPublisherBase(uavcan::equipment::joystick::JoyCAN::DefaultDataTypeID),
		uORB::SubscriptionCallbackWorkItem(work_item, ORB_ID(manual_control_setpoint)),
		uavcan::Publisher<uavcan::equipment::joystick::JoyCAN>(node)
	{
		this->setPriority(uavcan::TransferPriority::MiddleLower);
	}

	void PrintInfo() override
	{
		if (uORB::SubscriptionCallbackWorkItem::advertised()) {
			printf("\t%s -> %s:%d\n",
				uORB::SubscriptionCallbackWorkItem::get_topic()->o_name,
				uavcan::equipment::joystick::JoyCAN::getDataTypeFullName(),
				uavcan::equipment::joystick::JoyCAN::DefaultDataTypeID);
		}
	}

	void BroadcastAnyUpdates() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		if(uORB::SubscriptionCallbackWorkItem::update(%manual_control_setpoint)) {
			uavcan::equipment::joystick::JoyCAN JoystickCAN{};
			JoystickCAN.roll = manual_control_setpoint.roll;
			JoystickCAN.pitch = manual_control_setpoint.pitch;
			JoystickCAN.yaw = manual_control_setpoint.yaw;
			JoystickCAN.throttle = manual_control_setpoint.throttle;

		uavcan::Publisher<uavcan::equipment::joystick::JoyCAN>::broadcast(JoystickCAN);
		uORB::SubscriptionCallbackWorkItem::registerCallback();
		}
	}
};
}
