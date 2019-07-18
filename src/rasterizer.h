#pragma once

#include "stepper.h"

class Rasterizer
{
public:
	enum {
		NUM_AXIS = 2,
		MAX_LINE = 16
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
		uint32_t debug;
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

	Line m_lines[MAX_LINE];
	volatile uint8_t m_curLine;
	// Position de la ligne suivante.
	volatile uint8_t m_nextLine;
	volatile uint8_t m_nbLine;

	Stepper *m_steppers[NUM_AXIS];

	Line *GetLastLine() const;
	void NextLine();
	bool BufferFull();
	bool BufferEmpty();

	bool DrawLineStep(Line &line);

public:
	Rasterizer(Stepper *steppers[NUM_AXIS]);

	void Tic();

	void AddLine(double pos[NUM_AXIS], double periodUs);
	void AddLine(uint32_t pos[NUM_AXIS], double period);
};

#define FOREACH_AXIS \
	for (uint8_t i = 0; i < Rasterizer::NUM_AXIS; ++i)
