#pragma once

#include "stepper.h"
#include "circular_buffer.h"

class Rasterizer
{
public:
	enum {
		NUM_AXIS = 2,
		MAX_LINE = 16
	};

	enum ArcDir
	{
		ARC_CW = 1,
		ARC_CCW = -1
	};

	// 37.5mm pour 4000
	static constexpr float STEP_MM = 37.5 / 4000.0;
	static constexpr float MAX_BOUND = 39.0;
	// 1mm par segment.
	static constexpr float ARC_PRECISION = 0.2;


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

	void AddLine(const float pos[NUM_AXIS], float speed);
	void AddLine(const uint16_t pos[NUM_AXIS], float speed);

	void AddCircle(const float pos[NUM_AXIS], const float rel[NUM_AXIS], ArcDir dir, float speed);
};

#define FOREACH_AXIS \
	for (uint8_t i = 0; i < Rasterizer::NUM_AXIS; ++i)
