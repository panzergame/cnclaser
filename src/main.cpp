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

static Stepper stepper1(PORTC, PINC, DDRC, 0, 1, 2, 3, false);
static Stepper stepper2(PORTD, PIND, DDRD, 2, 3, 4, 5, false);
static Stepper *steppers[2] = {&stepper1, &stepper2};

static Rasterizer rasterizer(steppers, Timer::PERIOD_US);
static Parser parser;
static Usart usart;

void tic()
{
	rasterizer.Tic();
}

void receive(uint8_t data)
{
	parser.Received(data);
}

void welcome()
{
	char buf[64];
	sprintf(buf, "Begin %f\n", (double)Timer::PERIOD_US);
	MainUsart.Send(buf);
}

void setup()
{
	MainTimer.Init(tic);
	MainUsart.Init(receive);

	FOREACH_AXIS {
		Stepper *stepper = steppers[i];
		stepper->Init();
// 		stepper->Calibrate();
	}

	sei();

	welcome();
}

void loop()
{
	float speed = 5.0;

	Parser::Command cmd = parser.NextCommand();

	switch (cmd.type) {
		case Parser::Command::LINEAR_MOVE:
		case Parser::Command::LINEAR_MOVE_FAST:
		{
			rasterizer.AddLine(cmd.pos, speed);
			break;
		}
		default:
		{
			break; // TODO laser
		}
	}
}

int main()
{
	setup();

	/*steppers[0]->Enable();
	steppers[0]->SetDir(Stepper::UP);
	for (uint16_t i = 0; i < 4000; ++i) {
		_delay_us(100);
		steppers[0]->TicUp();
		steppers[0]->TicDown();
	}
	steppers[0]->Disable();*/

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
