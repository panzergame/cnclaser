#pragma once

#include "circular_buffer.h"
#include "config.h"

#include <avr/io.h>

/// Command parser
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
		/// For arcs only.
		float rel[2];
		/// Feed rate.
		uint8_t feed;
		/// Intensity
		uint8_t intensity;
	};

private:
	struct Buffer
	{
		/// Command line
		char data[Config::CMD_LINE_SIZE];
		/// Command status, if ack true then the command is accetped and ack sent back
		bool ack;
	};

	/// Commands queue
	CircularBuffer<Buffer, Config::CMD_QUEUE_SIZE> m_buffers;
	/// Current buffer
	Buffer m_buffer;
	/// Current buffer len, number of character received.
	uint8_t m_bufferLen;

	/// Determine command type based on command line.
	Command::Type ParseCommandType(Buffer &buffer) const;
	/// Determine command type and data from command line.
	Command ParseCommand(Buffer &buffer) const;

	/// Send back a ack to allow sending next command line.
	void SendAck();

public:
	Parser();
	/// Receive one character from USART.
	void Received(uint8_t data);
	/// Wait until valid command and parse it.
	Command NextCommand();
};
