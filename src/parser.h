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
			CW_ARC_MOVE,
			CCW_ARC_MOVE,
			LASER_ON,
			LASER_OFF,
			UNKNOWN
		} type;

		float pos[2];
		// Pour les arcs.
		float rel[2];
	};

private:
	enum {
		BUFFER_SIZE = 48
	};

	struct Buffer
	{
		char data[BUFFER_SIZE];
		bool ack;

		operator bool() const
		{
			return true;
		}
	};

	CircularBuffer<Buffer, 8> m_buffers;
	Buffer m_buffer;
	uint8_t m_bufferLen;

	Command::Type ParseCommandType(Buffer &buffer) const;
	Command ParseCommand(Buffer &buffer) const;

	void SendAck();

public:

	Parser();

	void Received(uint8_t data);

	Command NextCommand();
};
