#include "stepper.h"
#include "motion.h"

static Stepper stepper1(PORTC, PINC, DDRC, PCMSK1, PCIE1, 0, 1, 2, 3, false);
static Stepper stepper2(PORTD, PIND, DDRD, PCMSK2, PCIE2, 2, 3, 4, 5, false);
static AxisController axisX(&stepper1, 0, 0);
static AxisController axisY(&stepper2, 0, 0);

static Stepper *steppers[2] = {&stepper1, &stepper2};
static StepperEngine engine(steppers, 2);

void setup()
{
	engine.Init();
// 	stepper1.MoveUntilBound(Stepper::DOWN, 1e6);
// 	stepper2.MoveUntilBound(Stepper::DOWN, 1e6);
	axisX.Calibrate();
	axisY.Calibrate();
}
 
void loop()
{
	/*stepper1.Move(1300, Stepper::UP, 1);
	stepper2.Move(1300, Stepper::UP, 1);
	stepper1.Move(1300, Stepper::DOWN, 1);
	stepper2.Move(1300, Stepper::DOWN, 1);*/
}
