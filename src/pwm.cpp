#include "pwm.h"

Pwm MainPwm;

void Pwm::Init()
{
	DDRD |= _BV(DDD6);

	TCCR0A |= _BV(WGM01) | _BV(WGM00) | _BV(COM0A1);
	TCCR0B |= _BV(CS02) | _BV(CS00);

	SetDutyCycle(0);
}

void Pwm::SetDutyCycle(uint8_t duty)
{
	OCR0A = duty;
}
