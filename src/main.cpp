#if 1
#include "stepper.h"
#include "laser.h"
#include "rasterizer.h"
#include "timer.h"
#include "pwm.h"
#include "parser.h"
#include "circular_buffer.h"
#include "usart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

static Stepper stepper1(PORTC, PINC, DDRC, 0, 1, 2, 3, true);
static Stepper stepper2(PORTD, PIND, DDRD, 2, 3, 4, 5, false);
static Stepper *steppers[2] = {&stepper1, &stepper2};

static Laser laser;

static Rasterizer rasterizer(steppers, &laser, Timer::PERIOD_US);
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
	MainPwm.Init();
	MainUsart.Init(receive);

	FOREACH_AXIS {
		Stepper *stepper = steppers[i];
		stepper->Init();
		stepper->Calibrate();
	}

	/*steppers[0]->Calibrate();
	steppers[1]->Calibrate();*/

	sei();

	welcome();

	/*const float speed = 2.0;

	float pos[2] = {20.0, 38.0};
	float rel1[2] = {0.0, 16.5};
	float rel2[2] = {0.0, -16.5};
	float start[2] = {20.0, 5.0};
	rasterizer.AddLine(start, 2.0);
	rasterizer.AddCircle(pos, rel1, Rasterizer::ARC_CCW, speed);
	rasterizer.AddCircle(start, rel2, Rasterizer::ARC_CCW, speed);*/
}

void loop()
{
	const float speed = 0.4;

	const Parser::Command cmd = parser.NextCommand();

	switch (cmd.type) {
		case Parser::Command::LINEAR_MOVE:
		case Parser::Command::LINEAR_MOVE_FAST:
		{
			rasterizer.AddLine(cmd.pos, speed);
			break;
		}
		case Parser::Command::CW_ARC_MOVE:
		{
			rasterizer.AddCircle(cmd.pos, cmd.rel, Rasterizer::ARC_CW, speed);
			break;
		}
		case Parser::Command::CCW_ARC_MOVE:
		{
			rasterizer.AddCircle(cmd.pos, cmd.rel, Rasterizer::ARC_CCW, speed);
			break;
		}
		case Parser::Command::LASER_ON:
		{
			rasterizer.EnableLaser();
			break;
		}
		case Parser::Command::LASER_OFF:
		{
			rasterizer.DisableLaser();
			break;
		}
		default:
		{
			break;
		}
	}
}

int main()
{
	setup();

	while (true) {
		loop();
	}
}

#else

#include <avr/io.h>
#include <avr/interrupt.h>

/*#include "pwm.h"

void tic(bool enable)
{
	if (enable) {
		PORTC |= _BV(PORTC4);
	}
	else {
		PORTC &= ~(_BV(PORTC4));
	}
}*/

int main()
{
	DDRD |= _BV(DDD6);

	TCCR0A |= _BV(WGM01) | _BV(WGM00) | _BV(COM0A1);
	TCCR0B |= _BV(CS00) | _BV(CS01);
	OCR0A = 0;

// 	MainPwm.Init(tic);
// 	MainPwm.SetDutyCycle(100);

	/*TCCR0A |= _BV(WGM01) | _BV(WGM00);
	TCCR0B |= _BV(CS02) | _BV(CS00);
	OCR0A = 200;

	TIMSK0 |= _BV(OCIE0A) | _BV(TOIE0);*/


	sei();

	while (1) {
// 		PORTC |= _BV(PORTC4);
	}
}

/*ISR(TIMER0_OVF_vect)
{
	PORTC &= ~(_BV(PORTC4));
//     PORTC |= _BV(PORTC4);
}

ISR(TIMER0_COMPA_vect)
{
    PORTC |= _BV(PORTC4);
}*/

#endif
