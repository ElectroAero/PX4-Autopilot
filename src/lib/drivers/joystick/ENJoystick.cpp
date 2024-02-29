#include "ENJoystick.hpp"
#include <lib/drivers/device/Device.hpp>

ENJoystick::ENJoystick(uint32_t device_id) :
	_device_id{device_id}
{
	// advertise immediately to keep instance numbering in sync
	_sensor_pub.advertise();
}

// ENJoystick::~ENJoystick()
// {
// 	_sensor_pub.unadvertise();
// }

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

void ENJoystick::update(const hrt_abstime &timestamp_sample, float x, float y, float z)
{

	// publish
	sensor_joy_s report;

	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	// report.error_count = _error_count;
	report.channels[0] = ((Xaxis / 128) - 1) * 400 + 1500;
	report.channels[1] = ((Yaxis / 128) - 1) * 400 + 1500;
	report.channels[2] = MOB;
	report.channels[3] = leftButton;
	report.channels[4] = rightButton;
	report.timestamp = hrt_absolute_time();

	_sensor_pub.publish(report);
}

// void PX4Accelerometer::updateFIFO(sensor_accel_fifo_s &sample)
// {
// 	// rotate all raw samples and publish fifo
// 	const uint8_t N = sample.samples;

// 	for (int n = 0; n < N; n++) {
// 		rotate_3i(_rotation, sample.x[n], sample.y[n], sample.z[n]);
// 	}

// 	sample.device_id = _device_id;
// 	sample.scale = _scale;
// 	sample.timestamp = hrt_absolute_time();
// 	_sensor_fifo_pub.publish(sample);


// 	// publish
// 	sensor_accel_s report;
// 	report.timestamp_sample = sample.timestamp_sample;
// 	report.device_id = _device_id;
// 	report.temperature = _temperature;
// 	report.error_count = _error_count;

// 	// trapezoidal integration (equally spaced)
// 	const float scale = _scale / (float)N;
// 	report.x = (0.5f * (_last_sample[0] + sample.x[N - 1]) + sum(sample.x, N - 1)) * scale;
// 	report.y = (0.5f * (_last_sample[1] + sample.y[N - 1]) + sum(sample.y, N - 1)) * scale;
// 	report.z = (0.5f * (_last_sample[2] + sample.z[N - 1]) + sum(sample.z, N - 1)) * scale;

// 	_last_sample[0] = sample.x[N - 1];
// 	_last_sample[1] = sample.y[N - 1];
// 	_last_sample[2] = sample.z[N - 1];

// 	report.clip_counter[0] = clipping(sample.x, N);
// 	report.clip_counter[1] = clipping(sample.y, N);
// 	report.clip_counter[2] = clipping(sample.z, N);
// 	report.samples = N;
// 	report.timestamp = hrt_absolute_time();

// 	_sensor_pub.publish(report);
// }

// void PX4Accelerometer::UpdateClipLimit()
// {
// 	// 99.9% of potential max
// 	_clip_limit = math::constrain((_range / _scale) * 0.999f, 0.f, (float)INT16_MAX);
// }
