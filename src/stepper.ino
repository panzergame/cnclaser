#include "stepper.h"

void Stepper::Tic()
{
	if (m_task.tics++ == m_task.period) {
		m_task.tics = 0;

		PORTB ^= _BV(PORTB5);

		if (m_task.dir ^ m_invertDir) {
			m_port |= _BV(m_dirPin);
		}
		else {
			m_port &= ~(_BV(m_dirPin));
		}

		if (m_task.maxTics % 2 == 0) {
			m_port |= _BV(m_stepPin);
		}
		else {
			m_port &= ~(_BV(m_stepPin));
		}

// 		m_port ^= _BV(m_stepPin); Pas de XOR sinon il y a un décalage entre deux mouvements.

		if (m_task.type == Task::FIXED_STEPS && m_task.maxTics <= 0) {
			m_task.type = Task::NONE;
		}

		--m_task.maxTics;
	}
}

bool Stepper::HasTask()
{
	return (m_task.type != Task::NONE);
}

void Stepper::ButtonPressed()
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
}

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
	m_task.type = Task::NONE;
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

void Stepper::Move(uint32_t steps, Dir dir, uint32_t periodUs)
{
	while (HasTask());

	m_task.maxTics = steps * 2;
	m_task.tics = 0;
	m_task.dir = dir;
	m_task.type = Task::FIXED_STEPS;
	m_task.period = periodUs / (StepperEngine::PERIOD_US / 2);
}

void Stepper::MoveUntilBound(Dir dir, uint32_t periodUs)
{
	while (HasTask());

	m_task.maxTics = 0;
	m_task.tics = 0;
	m_task.dir = dir;
	m_task.type = Task::UNTIL_BOUND;
	m_task.period = periodUs / (StepperEngine::PERIOD_US / 2);
}

StepperEngine *StepperEngine::singleton = nullptr;

StepperEngine::StepperEngine(Stepper *steppers[MAX_STEPPERS], uint8_t numStepper)
	:m_numStepper(numStepper)
{
	for (uint8_t i = 0; i < m_numStepper; ++i) {
		m_steppers[i] = steppers[i];
	}
}

void StepperEngine::Init()
{
	for (uint8_t i = 0; i < m_numStepper; ++i) {
		m_steppers[i]->Init();
	}

	singleton = this;

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	OCR1A = 0;
	TCCR1B |= _BV(WGM12); // CTC mode

	if (PRESCALER == 8) {
		TCCR1B |= _BV(CS11); // 8 prescaler
	}
	else if (PRESCALER == 64) {
		TCCR1B |= _BV(CS11) | _BV(CS10); // 64 prescaler
	}
	else if (PRESCALER == 256) {
		TCCR1B |= _BV(CS12); // 256 prescaler
	}
	else {
		TCCR1B |= _BV(CS12) | _BV(CS10); // 1024 prescaler
	}

	TIMSK1 |= _BV(OCIE1A); // enable timer compare interrupt
}

void StepperEngine::Register(Stepper *stepper)
{
	if (m_numStepper == MAX_STEPPERS) {
		return;
	}

	m_steppers[m_numStepper++] = stepper;
}

void StepperEngine::Update()
{
	for (uint8_t i = 0; i < m_numStepper; ++i) {
		Stepper *stepper = m_steppers[i];
		if (stepper->HasTask()) {
			stepper->Tic();
		}
	}
}

void StepperEngine::ButtonPressed()
{
	for (uint8_t i = 0; i < m_numStepper; ++i) {
		m_steppers[i]->ButtonPressed();
	}
}

ISR(TIMER1_COMPA_vect)
{
	StepperEngine::singleton->Update();
}

ISR(PCINT0_vect)
{
	StepperEngine::singleton->ButtonPressed();
}

ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
