#pragma once

class Stepper;

class AxisController
{
private:
	Stepper *m_stepper;
	const uint32_t m_stepPerMeter;
	const uint32_t m_maxBound;
	uint32_t m_position;

public:
	AxisController(Stepper *stepper, uint32_t stepPerMeter, uint32_t maxBound);

	void Calibrate();
	void Move(double position, double speed);
};

class MotionController
{
private:
	AxisController *m_axisX;
	AxisController *m_axisY;

public:
	MotionController(AxisController *axisX, AxisController *axisY);

	void Calibrate();
	void Move(double x, double y, double speed);
};
