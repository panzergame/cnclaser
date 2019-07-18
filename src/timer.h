#pragma once

class Timer
{
	void (*m_func)();

public:
	static constexpr uint32_t PRESCALER = 64;
	static constexpr double FREQUENCY = F_CPU / (PRESCALER * 2.0);
	static constexpr double PERIOD_US = 1.0e6 * (PRESCALER * 2.0) / F_CPU;

	void Init(void (*func)());

	void Tic();
};

extern Timer MainTimer;
