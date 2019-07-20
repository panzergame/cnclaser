#pragma once

#include "circular_buffer.h"

class Usart
{
private:
	CircularBuffer<char, 256> m_tx;
	void (*m_func)(uint8_t data);

public:
	void Init(void (*func)(uint8_t data));

	void Send(char data);
	void Send(const char *datas);

	void SendNext();

	void Receive(uint8_t data);
};

extern Usart MainUsart;
