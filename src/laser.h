#pragma once

#include <avr/io.h>

/// Laser controller
class Laser
{
public:
	/// Enable laser with 0 to 255 intensity
	void Enable(uint8_t intensity);
	void Disable();
};
