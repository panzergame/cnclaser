#pragma once

#include "stepper.h"
#include "circular_buffer.h"

class Rasterizer
{
public:
	enum {
		NUM_AXIS = 2,
		MAX_LINE = 32
	};

	// 37.5mm pour 4000
	static constexpr float STEP_MM = 37.5 / 4000.0;
	static constexpr float MAX_BOUND = 39.0;

private:
	struct Axis
	{
		uint16_t over;
		uint16_t deltaAbs;
		Stepper::Dir dir;
	};

	struct Line
	{
		Axis axis[NUM_AXIS];
		uint16_t pos[NUM_AXIS];
		uint16_t steps;
		uint16_t stepsLeft;
		float period;
		float elapsed;
	};

	CircularBuffer<Line, MAX_LINE> m_lines;
	Stepper *m_steppers[NUM_AXIS];
	float m_ticPeriod;
	/* Position après la dernière commande.
	 * Utile dans le cas ou les commandes sont consommé plus rapidement que envoyé.
	 */
	uint16_t m_pos[NUM_AXIS];

	bool DrawLineStep(Line &line);

public:
	Rasterizer(Stepper *steppers[NUM_AXIS], float ticPeriod);

	void Tic();

	void AddLine(float pos[NUM_AXIS], float speed);
	void AddLine(uint16_t pos[NUM_AXIS], float speed);
};

#define FOREACH_AXIS \
	for (uint8_t i = 0; i < Rasterizer::NUM_AXIS; ++i)
