#pragma once

#include "config.h"

#include <stdint.h>

/// Timer controller exciting stepper controllers.
class Timer
{
	/// Custom function called on tic.
	void (*m_func)();

public:
	static constexpr float FREQUENCY = F_CPU / (Config::TIMER_PRESCALER * 2.0);
	static constexpr float PERIOD_US = 1.0e6 * (Config::TIMER_PRESCALER * 2.0) / F_CPU;

	void Init(void (*func)());

	void Tic();
};

/// Timer singleton.
extern Timer MainTimer;
