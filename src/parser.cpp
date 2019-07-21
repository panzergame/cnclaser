#include "parser.h"
#include "usart.h"

#include <util/delay.h>
#include <stdio.h>

Parser::Command::Type Parser::ParseCommandType(Buffer &buffer) const
{
	switch (buffer.data[0]) {
		case 'M':
		{
			int type;
			sscanf(buffer.data, "M%i", &type);
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
			sscanf(buffer.data, "G%i", &type);
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

Parser::Command Parser::ParseCommand(Buffer &buffer) const
{
	Command cmd;
	cmd.type = ParseCommandType(buffer);

	switch (cmd.type) {
		case Command::LINEAR_MOVE:
		{
			sscanf(buffer.data, "G1 X %f Y %f", &cmd.pos[0], &cmd.pos[1]);
			break;
		}
		case Command::LINEAR_MOVE_FAST:
		{
			sscanf(buffer.data, "G0 X %f Y %f", &cmd.pos[0], &cmd.pos[1]);
			break;
		}
		default:
		{
			cmd.pos[0] = -1.0;
			cmd.pos[1] = -1.0;
			break;
		}
	}

	return cmd;
}

Parser::Parser()
	:m_bufferLen(0)
{
}

void Parser::Received(uint8_t data)
{
	if (!m_buffers.Full()) {
		m_buffer.data[m_bufferLen++] = data;

		/*char buf[16];
		sprintf(buf, "%p\n\r", &buffer);
		MainUsart.Send(buf);*/

		// Débordement…
		if (m_bufferLen == (BUFFER_SIZE - 1)) {
			m_bufferLen = 0;
		}

		// Ligne complète reçu.
		if (data == '\n') {
			m_buffer.data[m_bufferLen] = '\0';
			m_buffers.Add(m_buffer);
			m_bufferLen = 0;

			if (!m_buffers.Full()) {
				MainUsart.Send("\nack\n");
// 				MainUsart.Send(m_buffer.data);
			}
		}
	}
}

Parser::Command Parser::NextCommand()
{
	Command cmd;

	do {
		const bool full = m_buffers.Full();

		Buffer *buffer;
		do {
			buffer = m_buffers.RemoveBegin();
		} while (!buffer);

		// Si le buffer était plein, valider le dernier message.
		if (full) {
			MainUsart.Send("\nack\n");
		}

		cmd = ParseCommand(*buffer);
		/*char buf[200];
		sprintf(buf, "%i %f %f\n", cmd.type, cmd.pos[0], cmd.pos[1]);
		MainUsart.Send(buf);*/
	} while (cmd.type == Command::UNKNOWN);

	/*char buf[100];
	sprintf(buf, "cmd type %i X %f Y %f\n", cmd.type, cmd.pos[0], cmd.pos[1]);
	MainUsart.Send(buf);*/

	return cmd;
}
