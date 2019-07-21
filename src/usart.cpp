#include "usart.h"

#define BAUD 28800

#include <util/setbaud.h>
#include <avr/io.h>
#include <avr/interrupt.h>

Usart MainUsart;

void Usart::Init(void (*func)(uint8_t data))
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C |= _BV(UCSZ00);

	m_func = func;
}

void Usart::Send(char data)
{
	m_tx.Add(data);
}

void Usart::Send(const char *datas)
{
	m_tx.Add(datas);
}

void Usart::SendNext()
{
	char *data = m_tx.RemoveBegin();
	if (data) {
		UDR0 = *data;
	}
}

void Usart::Receive(uint8_t data)
{
	m_func(data);
}

ISR(USART_RX_vect)
{
	MainUsart.Receive(UDR0);
}

ISR(USART_UDRE_vect)
{
	MainUsart.SendNext();
}
