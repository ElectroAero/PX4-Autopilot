#include "ENJoystick.hpp"
#include <lib/drivers/device/Device.hpp>

ENJoystick::ENJoystick(uint32_t device_id) :
	_device_id{device_id}
{
	// advertise immediately to keep instance numbering in sync
	_uavcan_joystick_pub.advertise();
}

ENJoystick::~ENJoystick()
{
	_uavcan_joystick_pub.unadvertise();
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

void ENJoystick::update(const hrt_abstime &timestamp_sample, const int Xaxis, const int Yaxis, const int MOB, const int leftButton, const int rightButton)
{

	// publish
	uavcan_joystick_s report;

	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	report.x_axis = Xaxis;
	report.y_axis = Yaxis;
	report.deadman = MOB;
	report.lbutton = leftButton;
	report.rbutton = rightButton;
	report.timestamp = hrt_absolute_time();

	_uavcan_joystick_pub.publish(report);
}
