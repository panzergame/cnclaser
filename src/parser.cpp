#include "parser.h"
#include "usart.h"

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>

Parser::Command::Type Parser::ParseCommandType(Buffer &buffer) const
{
	switch (buffer.data[0]) {
		case 'M':
		{
			int type;
			sscanf_P(buffer.data, PSTR("M%i"), &type);
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
			sscanf_P(buffer.data, PSTR("G%i"), &type);
			switch (type) {
				case 0:
				{
					return Command::LINEAR_MOVE_FAST;
				}
				case 1:
				{
					return Command::LINEAR_MOVE;
				}
				case 2:
				{
					return Command::CW_ARC_MOVE;
				}
				case 3:
				{
					return Command::CCW_ARC_MOVE;
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
		case Command::LASER_ON:
		{
			sscanf_P(buffer.data, PSTR("M1 S %i"), &cmd.intensity);
		}
		case Command::LINEAR_MOVE:
		{
			sscanf_P(buffer.data, PSTR("G1 X %f Y %f F %i"), &cmd.pos[0], &cmd.pos[1], &cmd.feed);
			break;
		}
		case Command::LINEAR_MOVE_FAST:
		{
			sscanf_P(buffer.data, PSTR("G0 X %f Y %f"), &cmd.pos[0], &cmd.pos[1]);
			break;
		}
		case Command::CW_ARC_MOVE:
		{
			sscanf_P(buffer.data, PSTR("G2 X %f Y %f I %f J %f F %i"), &cmd.pos[0], &cmd.pos[1], &cmd.rel[0], &cmd.rel[1], &cmd.feed);
			break;
		}
		case Command::CCW_ARC_MOVE:
		{
			sscanf_P(buffer.data, PSTR("G3 X %f Y %f I %f J %f F %i"), &cmd.pos[0], &cmd.pos[1], &cmd.rel[0], &cmd.rel[1], &cmd.feed);
			break;
		}
		default:
		{
			cmd.pos[0] = 0.0f;
			cmd.pos[1] = 0.0f;
			cmd.rel[0] = 0.0f;
			cmd.rel[1] = 0.0f;
			break;
		}
	}

	return cmd;
}

void Parser::SendAck()
{
	MainUsart.SendP(PSTR("ak\n"));
}

Parser::Parser()
	:m_bufferLen(0)
{
}

void Parser::Received(uint8_t data)
{
	if (!m_buffers.Full()) {
		m_buffer.data[m_bufferLen++] = data;

		// Ligne complète reçu.
		if (data == '\n') {
			m_buffer.data[m_bufferLen] = '\0';
			m_bufferLen = 0;

			// Aucun envoie d'un ack si le buffer va être plein avec ce nouveau buffer.
			m_buffer.ack = (m_buffers.Len() < (m_buffers.Size - 1));
			if (m_buffer.ack) {
				SendAck();
			}

			m_buffers.Add(m_buffer);
		}
	}
}

Parser::Command Parser::NextCommand()
{
	Buffer *buffer;
	// Attendre jusqu'à avoir un buffer.
	do {
		buffer = m_buffers.RemoveBegin();
	} while (!buffer);

	// Envoie du ack si le buffer était plein.
	if (!buffer->ack) {
		buffer->ack = true;
		SendAck();
	}

	Command cmd = ParseCommand(*buffer);

	return cmd;
}
