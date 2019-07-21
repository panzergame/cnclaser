#include "stepper.h"

#include <util/delay.h>

#define NOP __asm__ __volatile__ ("nop\n\t")

Stepper::Stepper(volatile uint8_t &port, volatile uint8_t &pin, volatile uint8_t &ddr,
		uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t buttonPin, bool invertDir)
	:m_port(port),
	m_pin(pin),
	m_ddr(ddr),
	m_dirPin(dirPin),
	m_stepPin(stepPin),
	m_enablePin(enablePin),
	m_buttonPin(buttonPin),
	m_invertDir(invertDir)
{
}

void Stepper::Init()
{
	// Output
	m_ddr |= _BV(m_stepPin) | _BV(m_dirPin) | _BV(m_enablePin);
	// Input
	m_ddr &= ~(_BV(m_buttonPin));
	// Pull up
	m_port |= _BV(m_buttonPin);
}

void Stepper::Calibrate()
{
	Enable();
	// Recule de 50 pas minimum si déjà en butée.
	SetDir(UP);
	while ((m_pin & _BV(m_buttonPin)) == 0) {
		for (uint8_t i = 0; i < 50; ++i) {
			TicUp();
			NOP;
			TicDown();
			_delay_ms(3);
		}
	}

	SetDir(DOWN);
	while ((m_pin & _BV(m_buttonPin)) != 0) {
		TicUp();
		NOP;
		TicDown();
		_delay_ms(3);
	}
	Disable();
}

void Stepper::SetDir(Dir dir)
{
	if (dir ^ m_invertDir) {
		m_port |= _BV(m_dirPin);
	}
	else {
		m_port &= ~(_BV(m_dirPin));
	}
}

void Stepper::TicUp()
{
	m_port |= _BV(m_stepPin);
}

void Stepper::TicDown()
{
	m_port &= ~(_BV(m_stepPin));
}

void Stepper::Enable()
{
	m_port &= ~(_BV(m_enablePin));
}

void Stepper::Disable()
{
	m_port |= _BV(m_enablePin);
}
