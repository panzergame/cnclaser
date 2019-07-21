#pragma once

#include "circular_buffer.h"

#include <avr/io.h>

class Parser
{
public:
	struct Command
	{
		enum Type {
			LINEAR_MOVE,
			LINEAR_MOVE_FAST,
			LASER_ON,
			LASER_OFF,
			UNKNOWN
		} type;

		float pos[2];
	};

private:
	enum {
		BUFFER_SIZE = 32
	};

	struct Buffer
	{
		char data[BUFFER_SIZE];

		operator bool() const
		{
			return false;
		}
	};

	CircularBuffer<Buffer, 2> m_buffers;
	Buffer m_buffer;
	uint8_t m_bufferLen;

	Command::Type ParseCommandType(Buffer &buffer) const;
	Command ParseCommand(Buffer &buffer) const;

public:

	Parser();

	void Received(uint8_t data);

	Command NextCommand();
};
