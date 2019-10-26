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
static Stepper stepper2(PORTD, PIND, DDRD, 2, 3, 4, 5, true);
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

	sei();

	welcome();
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
