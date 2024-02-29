#pragma once

#include <drivers/drv_hrt.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/rc_channels.h>

class ENJoystick
{
public:
	ENJoystick(const uint32_t device_id);
	// ~ENJoystick();

	uint32_t get_device_id() const { return _device_id; }

	void set_device_id(const uint32_t device_id) { _rc_channels_pub.get().device_id = device_id; };
	void set_device_type(const uint8_t device_type);

	void update(const hrt_abstime &timestamp_sample, const float Xaxis, const float Yaxis, const float MOB, const float leftButton, const float rightButton);

	int get_instance() { return _rc_channels_pub.get_instance(); };

private:
	uORB::PublicationMultiData<rc_channels_s> _rc_channels_pub{ORB_ID(rc_channels)};

	uint32_t		_device_id{0};
};
