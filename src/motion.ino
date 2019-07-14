#include "motion.h"
#include "stepper.h"

AxisController::AxisController(Stepper *stepper, uint32_t stepPerMeter, uint32_t maxBound)
	:m_stepper(stepper),
	m_stepPerMeter(stepPerMeter),
	m_maxBound(maxBound)
{
}

void AxisController::Calibrate()
{
	m_stepper->MoveUntilBound(Stepper::DOWN, 5e3);
}

void AxisController::Move(double value, double speed)
{
	const uint32_t period = (uint32_t)(1.0e6 / speed);

	m_stepper->Move((uint32_t)value, Stepper::UP, period);
}

MotionController::MotionController(AxisController *axisX, AxisController *axisY)
	:m_axisX(axisX),
	m_axisY(axisY)
{
}

void MotionController::Calibrate()
{
	m_axisX->Calibrate();
	m_axisY->Calibrate();
}

void MotionController::Move(double x, double y, double speed)
{
	
}
