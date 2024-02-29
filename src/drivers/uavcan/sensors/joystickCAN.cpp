/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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
 * @author Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

// #include "joystickCAN.hpp"
#include <lib/drivers/joystick/ENJoystick.hpp>

const char *const UavcanJoyBridge::NAME = "joy";

UavcanJoyBridge::UavcanJoyBridge(uavcan::INode &node) :
	UavcanSensorBridgeBase("uavcan_joy", ORB_ID(rc_channels)),
	_sub_joy_data(node)
{ }

int UavcanJoyBridge::init()
{
	int res = _sub_joy_data.start(JoyCbBinder(this, &UavcanJoyBridge::joy_sub_cb));

	if (res < 0) {
		DEVICE_LOG("failed to start uavcan sub: %d", res);
		return res;
	}

	return 0;
}

void UavcanJoyBridge::joy_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &msg)
{
	uavcan_bridge::Channel *channel = get_channel_for_node(msg.getSrcNodeID().get());

	const hrt_abstime timestamp_sample = hrt_absolute_time();

	if (channel == nullptr) {
		// Something went wrong - no channel to publish on; return
		return;
	}

	// Cast our generic CDev pointer to the sensor-specific driver class
	ENJoystick *joy = (ENJoystick *)channel->h_driver;

	if (joy == nullptr) {
		return;
	}
	joy->update(timestamp_sample, msg.Xaxis, msg.Yaxis, msg.MOB, msg.leftButton, msg.rightButton);
}

int UavcanJoyBridge::init_driver(uavcan_bridge::Channel *channel)
{
	// update device id as we now know our device node_id
	DeviceId device_id{_device_id};

	device_id.devid_s.devtype = DRV_JOY_DEVTYPE_INPUT;
	device_id.devid_s.address = static_cast<uint8_t>(channel->node_id);

	channel->h_driver = new ENJoystick(device_id.devid);

	if (channel->h_driver == nullptr) {
		return PX4_ERROR;
	}

	ENJoystick *joy = (ENJoystick *)channel->h_driver;

	channel->instance = joy->get_instance();

	if (channel->instance < 0) {
		PX4_ERR("UavcanJoy: Unable to get a class instance");
		delete joy;
		channel->h_driver = nullptr;
		return PX4_ERROR;
	}

	return PX4_OK;
}
