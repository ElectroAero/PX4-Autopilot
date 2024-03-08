#pragma once

#include <drivers/drv_hrt.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/uavcan_joystick.h>

class ENJoystick
{
public:
	ENJoystick(const uint32_t device_id);
	~ENJoystick();

	uint32_t get_device_id() const { return _device_id; }

	void set_device_id(const uint32_t device_id) { _uavcan_joystick_pub.get().device_id = device_id; };
	void set_device_type(const uint8_t device_type);

	void update(const hrt_abstime &timestamp_sample, const int Xaxis, const int Yaxis, const int MOB, const int leftButton, const int rightButton);

	int get_instance() { return _uavcan_joystick_pub.get_instance(); };

private:
	uORB::PublicationMultiData<uavcan_joystick_s> _uavcan_joystick_pub{ORB_ID(uavcan_joystick)};

	uint32_t		_device_id{1311};
};
