#pragma once

#include <avr/io.h>

/// Pwm controller, used mainly for laser on pin D6.
class Pwm
{
public:
	/// Initialize timer and pwm.
	void Init();
	/// Change duty cycle from 0 to 255.
	void SetDutyCycle(uint8_t duty);
};

/// Pwm singleton.
extern Pwm MainPwm;
