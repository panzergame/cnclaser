#pragma once

#include <avr/io.h>

class Parser
{
private:
	enum {
		BUFFER_SIZE = 128
	};

	uint8_t m_buffer[BUFFER_SIZE];
	uint8_t m_bufferLen;
	volatile bool m_bufferReady;

public:
	struct Command
	{
		enum Type {
			LINEAR_MOVE,
			CW_ARC_MOVE,
			CCW_ARC_MOVE,
			LASER_ON,
			LASER_OFF
		} type;

		double pos[2];
		double rel[2];
	};

	Parser();

	void Received(uint8_t data);

	Command NextCommand();
};
