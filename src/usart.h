#pragma once

#include "circular_buffer.h"
#include "config.h"

/// USART controller for reception and emission.
class Usart
{
private:
	/// Character queue to send until empty.
	CircularBuffer<char, Config::USART_TX_QUEUE_SIZE> m_tx;
	/// Custom function called when receiving caracter.
	void (*m_func)(uint8_t data);

public:
	void Init(void (*func)(uint8_t data));

	/** Schedule one caracter to send.
	 * @param data Caracter to send
	 * @param blocking Blocks until caracter is sheduled,
	 * otherwise ignore the caracter if buffer is full.
	 */
	void Send(char data, bool blocking=true);

	/** Schedule null terminated caracter array to send.
	 * @param datas Null terminated caracter array to send.
	 * @param blocking Blocks until caracters are sheduled,
	 * otherwise ignore some of the caracters if buffer is full.
	 */
	void Send(const char *datas, bool blocking=true);

	/** Schedule null terminated caracter array to send from program memory.
	 * @param datas Null terminated caracter array to send fro program memory.
	 * @param blocking Blocks until caracters are sheduled,
	 * otherwise ignore some of the caracters if buffer is full.
	 */
	void SendP(const char *datas, bool blocking=true);

	/// Process send of first schedule caracter.
	void SendNext();

	/// Process reception of last received caracter.
	void Receive(uint8_t data);
};

extern Usart MainUsart;
