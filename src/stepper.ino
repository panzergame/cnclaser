#include "stepper.h"

/*void Stepper::ButtonPressed()
{
	if (PCIFR & _BV(m_pcie)) {
		if (m_pin & _BV(m_buttonPin)) {
		}
		else {
			if (m_task.type == Task::UNTIL_BOUND) {
				m_task.type = Task::NONE;
			}
		}
	}
}*/

Stepper::Stepper(volatile uint8_t &port, volatile uint8_t &pin, volatile uint8_t &ddr, volatile uint8_t &pcmsk, uint8_t pcie,
		uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t buttonPin, bool invertDir)
	:m_port(port),
	m_pin(pin),
	m_ddr(ddr),
	m_pcmsk(pcmsk),
	m_pcie(pcie),
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
	// Enable
	m_port &= ~(_BV(m_enablePin));

	// Activer les interruptions pour le port
	PCICR |= _BV(m_pcie);

	m_pcmsk |= _BV(m_buttonPin);
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

ISR(PCINT0_vect)
{
// 	StepperEngine::singleton->ButtonPressed();
}

ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
