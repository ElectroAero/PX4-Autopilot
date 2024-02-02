#include "JoystickCan.hpp"

extern orb_advert_t mavlink_log_pub;

JoystickCan::JoystickCan() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::uavcan)
{
}

JoystickCan::~JoystickCan()
{
}

void JoystickCan::Run()
{
	if (should_exit()) {
		exit_and_cleanup();
		return;
	}

	if (!_initialized) {

		_fd = ::open("/dev/can0", O_RDWR);

		if (_fd < 0) {
			PX4_INFO("FAILED TO OPEN /dev/can0");
			return;
		}

		_initialized = true;
	}

	uint8_t data[64] {};
	CanFrame received_frame{};
	received_frame.payload = &data;

	JoystickCanMessage joystick_message = {};

	while (receive(&received_frame) > 0) {

		// Find the start of a transferr
		if ((received_frame.payload_size == 8) && ((uint8_t *)received_frame.payload)[7] == TAIL_BYTE_START_OF_TRANSFER) {
		} else {
			continue;
		}

		// We have the start of a transfer
		size_t offset = 5;
		memcpy(&joystick_message, &(((uint8_t *)received_frame.payload)[2]), offset);

		while (receive(&received_frame) > 0) {

			size_t payload_size = received_frame.payload_size - 1;
			// TODO: add check to prevent buffer overflow from a corrupt 'payload_size' value
			// TODO: AND look for TAIL_BYTE_START_OF_TRANSFER to indicate end of transfer. Untested...
			memcpy(((char *)&joystick_message) + offset, received_frame.payload, payload_size);
			offset += payload_size;
		}

		manual_control_setpoint_s manual_control_setpoint = {};

		manual_control_setpoint.roll = static_cast<float>(joystick_message.roll);
		manual_control_setpoint.pitch = static_cast<float>(joystick_message.pitch);
		manual_control_setpoint.yaw = static_cast<float>(joystick_message.yaw);
		manual_control_setpoint.throttle = static_cast<float>(joystick_message.throttle);

		_manual_control_setpoint_pub.publish(manual_control_setpoint);
	}
}

int16_t JoystickCan::receive(CanFrame *received_frame)
{
	if ((_fd < 0) || (received_frame == nullptr)) {
		PX4_INFO("fd < 0");
		return -1;
	}

	struct pollfd fds {};

	fds.fd = _fd;

	fds.events = POLLIN;

	// Jake: this doesn't block even in blocking mode, dunno if we need it
	::poll(&fds, 1, 0);

	// Only execute this part if can0 is changed.
	if (fds.revents & POLLIN) {

		// Try to read.
		struct can_msg_s receive_msg;
		const ssize_t nbytes = ::read(fds.fd, &receive_msg, sizeof(receive_msg));

		if (nbytes < 0 || (size_t)nbytes < CAN_MSGLEN(0) || (size_t)nbytes > sizeof(receive_msg)) {
			PX4_INFO("error");
			return -1;

		} else {
			received_frame->extended_can_id = receive_msg.cm_hdr.ch_id;
			received_frame->payload_size = receive_msg.cm_hdr.ch_dlc;
			memcpy((void *)received_frame->payload, receive_msg.cm_data, receive_msg.cm_hdr.ch_dlc);
			return nbytes;
		}
	}

	return 0;
}

int JoystickCan::start()
{
	// There is a race condition at boot that sometimes causes opening of
	// /dev/can0 to fail. We will delay 0.5s to be safe.
	uint32_t delay_us = 500000;
	ScheduleOnInterval(1000000 / SAMPLE_RATE, delay_us);
	return PX4_OK;
}

int JoystickCan::task_spawn(int argc, char *argv[])
{
	JoystickCan *instance = new JoystickCan();

	if (!instance) {
		PX4_ERR("driver allocation failed");
		return PX4_ERROR;
	}

	_object.store(instance);
	_task_id = task_id_is_work_queue;

	instance->start();
	return 0;
}

int JoystickCan::print_usage(const char *reason)
{
	if (reason) {
		printf("%s\n\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Driver for reading data from a CAN Joystick.

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("joystick_can_test", "system");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

int JoystickCan::custom_command(int argc, char *argv[])
{
	if (!is_running()) {
		PX4_INFO("not running");
		return PX4_ERROR;
	}

	return print_usage("Unrecognized command.");
}

extern "C" __EXPORT int joystick_can_main(int argc, char *argv[])
{
	return JoystickCan::main(argc, argv);
}
