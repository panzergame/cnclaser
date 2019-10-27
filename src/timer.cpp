#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>

Timer MainTimer;

void Timer::Init(void (*func)())
{
	m_func = func;

	TCCR1A = 0;
	TCCR1B = _BV(WGM13);
	TCNT1 = 0;

	if (Config::TIMER_PRESCALER == 8) {
		TCCR1B |= _BV(CS11); // 8 prescaler
	}
	else if (Config::TIMER_PRESCALER == 64) {
		TCCR1B |= _BV(CS11) | _BV(CS10); // 64 prescaler
	}
	else if (Config::TIMER_PRESCALER == 256) {
		TCCR1B |= _BV(CS12); // 256 prescaler
	}
	else {
		TCCR1B |= _BV(CS12) | _BV(CS10); // 1024 prescaler
	}

	TIMSK1 |= _BV(TOIE1); // enable timer compare interrupt
}

void Timer::Tic()
{
	m_func();
}

ISR(TIMER1_OVF_vect)
{
	MainTimer.Tic();
}
