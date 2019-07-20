#if 1
#include "stepper.h"
#include "rasterizer.h"
#include "timer.h"
#include "parser.h"
#include "circular_buffer.h"
#include "usart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

/*static Stepper stepper1(PORTC, PINC, DDRC, 0, 1, 2, 3, false);
static Stepper stepper2(PORTD, PIND, DDRD, 2, 3, 4, 5, false);
static Stepper *steppers[2] = {&stepper1, &stepper2};

static Rasterizer rasterizer(steppers, Timer::PERIOD_US);*/
// static Parser parser;
static Usart usart;

void tic()
{
// 	rasterizer.Tic();
}

void receive(uint8_t data)
{
// 	parser.Received(data);
}

void welcome()
{
	char buf[64];
	sprintf(buf, "Begin %f\n", Timer::PERIOD_US);
	MainUsart.Send(buf);
}

void setup()
{
// 	MainTimer.Init(tic);
	MainUsart.Init(receive);

	/*FOREACH_AXIS {
		Stepper *stepper = steppers[i];
		stepper->Init();
// 		stepper->Calibrate();
	}*/

	welcome();

	sei();
}

void loop()
{
	/*double p0[] = {30., 30.};
	double p1[] = {0., 0.};
	double speed = 10.0;

	for (uint8_t i = 0; i < 8; ++i) {
		rasterizer.AddLine(p0, speed);
		rasterizer.AddLine(p1, speed);
	}*/
}

int main()
{
	setup();
	while(1) {
		loop();
	}
}

#else

#define BAUD 9600

#include <util/setbaud.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= _BV(RXCIE0) | /*_BV(UDRIE0) |*/ _BV(RXEN0) | _BV(TXEN0);
	UCSR0C |= _BV(UCSZ00);

	TCCR1A = 0;
	TCCR1B = _BV(WGM13);
	TCNT1 = 0;
	TCCR1B |= _BV(CS12) | _BV(CS10); // 1024 prescaler
	TIMSK1 |= _BV(TOIE1); // enable timer compare interrupt

	sei();

	while (1) {
	}
}

ISR(TIMER1_OVF_vect)
{
    PORTB ^= _BV(PORTB5);
}

ISR(USART_RX_vect)
{
	uint8_t v = UDR0;
	UDR0 = v;
    PORTB ^= _BV(PORTB5);
}

#endif
