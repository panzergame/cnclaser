#include "usart.h"

#define BAUD 28800

#include <util/setbaud.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

Usart MainUsart;

void Usart::Init(void (*func)(uint8_t data))
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C |= _BV(UCSZ00);

	m_func = func;
}

void Usart::Send(char data, bool blocking)
{
	// While the data can't be added and we are blocking in thsi case.
	while (!m_tx.Add(data) && blocking);
}

void Usart::Send(const char *datas, bool blocking)
{
	for (const char *c = datas; *c != '\0'; ++c) {
		Send(*c, blocking);
	}
}

void Usart::SendP(const char *datas, bool blocking)
{
	for (const char *c = datas; pgm_read_byte(c) != '\0'; ++c) {
		Send(pgm_read_byte(c), blocking);
	}
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
