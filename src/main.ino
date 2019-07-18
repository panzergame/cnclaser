#include "stepper.h"
#include "rasterizer.h"
#include "timer.h"

static Stepper stepper1(PORTC, PINC, DDRC, 0, 1, 2, 3, false);
static Stepper stepper2(PORTD, PIND, DDRD, 2, 3, 4, 5, false);
static Stepper *steppers[2] = {&stepper1, &stepper2};

static Rasterizer rasterizer(steppers);

void tic()
{
	rasterizer.Tic();
}

void setup()
{
	FOREACH_AXIS {
		Stepper *stepper = steppers[i];
		stepper->Init();
		stepper->Calibrate();
	}

	Serial.begin(9600);
// 	Serial.println(Timer::PERIOD_US);

	MainTimer.Init(tic);

	double p0[] = {30., 30.};
	double p1[] = {0., 0.};

	for (uint8_t i = 0; i < 8; ++i) {
		rasterizer.AddLine(p0, 40.);
		rasterizer.AddLine(p1, 40.);
	}
}
 
void loop()
{
// 	tic();
}
