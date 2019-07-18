#include "stepper.h"
#include "rasterizer.h"
#include "timer.h"

static Stepper stepper1(PORTC, PINC, DDRC, PCMSK1, PCIE1, 0, 1, 2, 3, false);
static Stepper stepper2(PORTD, PIND, DDRD, PCMSK2, PCIE2, 2, 3, 4, 5, false);

static Stepper *steppers[2] = {&stepper1, &stepper2};
static Rasterizer rasterizer(steppers);

void tic()
{
	rasterizer.Tic();
}

void setup()
{
	FOREACH_AXIS {
		steppers[i]->Init();
	}

	Serial.begin(9600);
	MainTimer.Init(tic);
}
 
void loop()
{
	uint32_t p0[] = {1000, 300};
	uint32_t p1[] = {1, 1};

	for (uint8_t i = 0; i < 8; ++i) {
		rasterizer.AddLine(p0, 1e1);
		rasterizer.AddLine(p1, 1e1);
	}
}
