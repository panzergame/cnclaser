#pragma once

#include "stepper.h"
#include "circular_buffer.h"

class Rasterizer
{
public:
	enum {
		NUM_AXIS = 2,
		MAX_LINE = 2
	};

	// 37.5mm pour 4000
	static constexpr double STEP_MM = 37.5 / 4000.0;
	static constexpr double MAX_BOUND = 39.0;

private:
	struct Axis
	{
		uint32_t over;
		uint32_t deltaAbs;
		int32_t delta;
		Stepper::Dir dir;
	};

	struct Line
	{
		Axis axis[NUM_AXIS];
		uint32_t pos[NUM_AXIS];
		uint32_t steps;
		uint32_t stepsLeft;
		double period;
		double elapsed;
	};

	CircularBuffer<Line, MAX_LINE> m_lines;
	Stepper *m_steppers[NUM_AXIS];
	double m_ticPeriod;

	bool DrawLineStep(Line &line);

public:
	Rasterizer(Stepper *steppers[NUM_AXIS], double ticPeriod);

	void Tic();

	void AddLine(double pos[NUM_AXIS], double speed);
	void AddLine(uint32_t pos[NUM_AXIS], double speed);
};

#define FOREACH_AXIS \
	for (uint8_t i = 0; i < Rasterizer::NUM_AXIS; ++i)
