/**
 * @author Shami Mohdar <shami@enautic.co>
 */

#pragma once


#include "sensor_bridge.hpp"

#include <uavcan/equipment/enautic/JoyCAN.hpp>

class UavcanJoyBridge : public UavcanSensorBridgeBase
{
public:
	static const char *const NAME;

	UavcanJoyBridge(uavcan::INode &node);

	const char *get_name() const override { return NAME; }

	int init() override;

private:

	void joy_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &msg);

	int init_driver(uavcan_bridge::Channel *get_channel_for_node) override;

	typedef uavcan::MethodBinder < UavcanJoyBridge *,
		void (UavcanJoyBridge::*)
		(const uavcan::ReceivedDataStructure<uavcan::equipment::enautic::JoyCAN> &) >
		JoyCbBinder;

	uavcan::Subscriber<uavcan::equipment::enautic::JoyCAN, JoyCbBinder> _sub_joy_data;

};
