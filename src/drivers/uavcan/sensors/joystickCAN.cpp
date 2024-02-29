/**
 * @author Shami Mohdar <shami@enautic.co>
 */

#include "joystickCAN.hpp"
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
