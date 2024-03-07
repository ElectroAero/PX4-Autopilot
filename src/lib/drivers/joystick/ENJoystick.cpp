#include "ENJoystick.hpp"
#include <lib/drivers/device/Device.hpp>

ENJoystick::ENJoystick(uint32_t device_id) :
	_device_id{device_id}
{
	// advertise immediately to keep instance numbering in sync
	_rc_channels_pub.advertise();
}

ENJoystick::~ENJoystick()
{
	_rc_channels_pub.unadvertise();
}

void ENJoystick::set_device_type(uint8_t devtype)
{
	// current DeviceStructure
	union device::Device::DeviceId device_id;
	device_id.devid = _device_id;

	// update to new device type
	device_id.devid_s.devtype = devtype;

	// copy back
	_device_id = device_id.devid;
}

// void ENJoystick::set_scale()

void ENJoystick::update(const hrt_abstime &timestamp_sample, const float Xaxis, const float Yaxis, const float MOB, const float leftButton, const float rightButton)
{

	// publish
	rc_channels_s report;

	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	// report.error_count = _error_count;
	report.channels[0] = ((Xaxis / 128) - 1) * 400 + 1500;
	report.channels[1] = ((Yaxis / 128) - 1) * 400 + 1500;
	report.channels[2] = MOB;
	report.channels[3] = leftButton;
	report.channels[4] = rightButton;
	report.timestamp = hrt_absolute_time();

	_rc_channels_pub.publish(report);
}
