#pragma once

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

		double pos[2];
	};

private:
	enum {
		BUFFER_SIZE = 128
	};

	char m_buffer[BUFFER_SIZE];
	uint8_t m_bufferLen;
	volatile bool m_bufferReady;

	Command::Type ParseCommandType() const;
	Command ParseCommand() const;

public:

	Parser();

	void Received(uint8_t data);

	Command NextCommand();
};
