#pragma once

#include "circular_buffer.h"

/// USART controller for reception and emission.
class Usart
{
private:
	enum {
		/// Maximum queue size.
		QUEUE_SIZE = 256
	};

	/// Character queue to send until empty.
	CircularBuffer<char, QUEUE_SIZE> m_tx;
	/// Custom function called when receiving character.
	void (*m_func)(uint8_t data);

public:
	void Init(void (*func)(uint8_t data));

	/// Schedule send one character.
	void Send(char data);
	/// Schedule send null terminated character array.
	void Send(const char *datas);

	/// Process send of first schedule character.
	void SendNext();

	/// Process reception of last received character.
	void Receive(uint8_t data);
};

extern Usart MainUsart;
