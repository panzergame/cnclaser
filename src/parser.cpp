#include "parser.h"
#include "usart.h"

#define BAUD 9600

#include <util/delay.h>
#include <util/setbaud.h>

Parser::Parser()
	:m_bufferLen(0),
	m_bufferReady(false)
{
}

void Parser::Received(uint8_t data)
{
	if (!m_bufferReady) {
		m_buffer[m_bufferLen++] = data;

		if (data == '\n') {
			m_bufferReady = true;
			m_buffer[m_bufferLen] = '\0';
		}
	}
}

Parser::Command Parser::NextCommand()
{
	while (!m_bufferReady);

	m_bufferLen = 0;
	m_bufferReady = false;

	_delay_ms(50);

	Command cmd;
	return cmd;
}
