#pragma once

#include "stepper.h"
#include "circular_buffer.h"
#include "config.h"

class Laser;

class Rasterizer
{
public:
	enum {
		/// Number of axis managed.
		NUM_AXIS = 2
	};

	enum ArcDir
	{
		ARC_CW,
		ARC_CCW
	};

private:
	/// Axis movement information
	struct Axis
	{
		/// Micro step count.
		uint16_t over;
		/// Threshold to make a step.
		uint16_t deltaAbs;
		/// Movement direction.
		Stepper::Dir dir;
	};

	/// Line information
	struct Line
	{
		Axis axis[NUM_AXIS];
		/// Position after movement.
		uint16_t pos[NUM_AXIS];
		/// Steps count.
		uint16_t steps;
		/// Steps to reach.
		uint16_t stepsLeft;
		/// Time between steps.
		float period;
		/// Timer count.
		float elapsed;
		/// Laser intensity during this line.
		uint8_t laserIntensity;
		/// Laser pause before moving used to heat surface.
		float laserPause;
	};

	/// Line queue.
	CircularBuffer<Line, Config::LINE_QUEUE_SIZE> m_lines;
	/// Stepper motor controllers.
	Stepper *m_steppers[NUM_AXIS];
	/// Laser controller.
	Laser *m_laser;

	/// Time bewteen tics.
	float m_ticPeriod;
	/// Position of last scheduled line.
	uint16_t m_lastPos[NUM_AXIS];
	/// Last sheduled line laser intensity.
	uint16_t m_lastLaserIntensity;
	/// Laser intensity used for next line.
	uint8_t m_laserIntensity;

	/** Proceed a line step.
	 * @return True if finished
	 */
	bool DrawLineStep(Line &line);

	/** Start laser and manage heat time.
	 * @return True when laser is started and ready to move
	 */
	bool StartLaser(Line &line);

public:
	Rasterizer(Stepper *steppers[NUM_AXIS], Laser *laser, float ticPeriod);

	/// Proceed a line step and pass to next when finished.
	void Tic();

	/// Schedule line to position in mm.
	void AddLine(const float pos[NUM_AXIS], float speed);
	/// Schedule line to position in steps.
	void AddLine(const uint16_t pos[NUM_AXIS], float speed);

	/// Schedule circle cutted into lines.
	void AddCircle(const float pos[NUM_AXIS], const float rel[NUM_AXIS], ArcDir dir, float speed);

	/// Enable laser for next lines.
	void EnableLaser(uint8_t intensity);
	/// Disable laser for next lines.
	void DisableLaser();
};

#define FOREACH_AXIS \
	for (uint8_t i = 0; i < Rasterizer::NUM_AXIS; ++i)
