/****************************************************************************
 *
 *   Copyright (c) 2014-2022 PX4 Development Team. All rights reserved.
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

/**
 * @file joystick.cpp
 *
 * @author Shami Mohdar <shami@enautic.co>
 *
 */

#include "joystick.hpp"

#include <cstdint>

#include <drivers/drv_hrt.h>
#include <systemlib/err.h>
#include <mathlib/mathlib.h>
#include <lib/parameters/param.h>

using namespace time_literals;

const char *const UavcanJoystick::NAME = "joy";

UavcanJoystick::UavcanJoystick(uavcan::INode &node) :
	UavcanSensorBridgeBase("uavcan_joy", ORB_ID(sensor_gps)),
	_node(node),
	_sub_joy(node),
	_timer(node)
{
}

UavcanJoystick::~UavcanJoystick()
{
}

int UavcanJoystick::init()
{
	// The line below will only work once UAVCAN NodeID has been established
	// int res = _sub_joy.start(JoyCbBinder(this, &UavcanJoystick::joy_sub_cb));

	// Testing uORB topic updating
	// _timer.setCallback(TimerCbBinder(this, &UavcanJoystick::periodic_update));
	joy_sub_cb();

	return 0;
}

void UavcanJoystick::joy_timer(int value) {
	if(!_timer.isRunning()) {
		_timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(value));
		memset(&manual_control_setpoint, 0, sizeof(manual_control_setpoint));
	}
}

void UavcanJoystick::joy_sub_cb(
	// const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &msg
	)
{
	static int testJoy = 3;
	// //Shami - Testing .start(MethodBinder) works for CAN RX

	_joystick_status.x_axis = testJoy;
	_joystick_status_pub.publish(_joystick_status);
	if(testJoy > 30) {
		testJoy--;
	}
	else {
		testJoy++;
	}
}
