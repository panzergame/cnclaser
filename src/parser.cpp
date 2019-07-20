#include "parser.h"
#include "usart.h"

#include <util/delay.h>
#include <stdio.h>

Parser::Command::Type Parser::ParseCommandType() const
{
	switch (m_buffer[0]) {
		case 'M':
		{
			int type;
			sscanf(m_buffer, "M%i", &type);
			switch (type) {
				case 1:
				{
					return Command::LASER_ON;
				}
				case 2:
				{
					return Command::LASER_OFF;
				}
				default:
				{
					return Command::UNKNOWN;
				}
			}
		}
		case 'G':
		{
			int type;
			sscanf(m_buffer, "G%i", &type);
			switch (type) {
				case 0:
				{
					return Command::LINEAR_MOVE_FAST;
				}
				case 1:
				{
					return Command::LINEAR_MOVE;
				}
				default:
				{
					return Command::UNKNOWN;
				}
			}
		}
		default:
		{
			return Command::UNKNOWN;
		}
	}
}

Parser::Command Parser::ParseCommand() const
{
	Command cmd;
	cmd.type = ParseCommandType();

	switch (cmd.type) {
		case Command::LINEAR_MOVE:
		{
			sscanf(m_buffer, "G1 X %lf Y %lf", &cmd.pos[0], &cmd.pos[1]);
			break;
		}
		case Command::LINEAR_MOVE_FAST:
		{
			sscanf(m_buffer, "G0 X %lf Y %lf", &cmd.pos[0], &cmd.pos[1]);
			break;
		}
		default:
		{
			break;
		}
	}

	return cmd;
}

Parser::Parser()
	:m_bufferLen(0),
	m_bufferReady(false)
{
}

void Parser::Received(uint8_t data)
{
	if (!m_bufferReady) {
		m_buffer[m_bufferLen++] = data;

		// Ligne complète reçu.
		if (data == '\n' || m_bufferLen == (BUFFER_SIZE - 1)) {
			m_bufferReady = true;
			m_buffer[m_bufferLen] = '\0';
		}
	}
}

Parser::Command Parser::NextCommand()
{
	Command cmd;

	do {
		while (!m_bufferReady);

		m_bufferLen = 0;
		m_bufferReady = false;

		cmd = ParseCommand();
		MainUsart.Send("ack");
		MainUsart.Send(m_buffer);
	} while (cmd.type == Command::UNKNOWN);

	/*char buf[100];
	sprintf(buf, "cmd type %i X %f Y %f\n", cmd.type, cmd.pos[0], cmd.pos[1]);
	MainUsart.Send(buf);*/

	return cmd;
}
