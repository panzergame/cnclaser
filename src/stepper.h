#pragma once

class Stepper
{
public:
	enum Dir {
		DOWN = true,
		UP = false
	};

private:
	volatile uint8_t &m_port;
	volatile uint8_t &m_pin;
	volatile uint8_t &m_ddr;
	volatile uint8_t &m_pcmsk;
	uint8_t m_pcie;

	uint8_t m_dirPin;
	uint8_t m_stepPin;
	uint8_t m_enablePin;
	uint8_t m_buttonPin;
	bool m_invertDir;

	class Task
	{
	public:
		// Nombre d'interruption actuelle
		uint32_t tics;
		// Nombre maximal de demi pas
		uint32_t maxTics;
		// Direction des pas
		Dir dir;
		// Periode avant demi pas en nombre d'interruption
		uint16_t period;

		enum Type {
			NONE,
			FIXED_STEPS,
			UNTIL_BOUND
		} type;
	};

	volatile Task m_task;

public:
	Stepper() = default;
	Stepper(volatile uint8_t &port, volatile uint8_t &pin, volatile uint8_t &ddr, volatile uint8_t &pcmsk, uint8_t pcie,
			uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t buttonPin, bool invertDir);

	void Init();

	void Tic();
	bool HasTask();

	void ButtonPressed();

	// Bloquant si une tache existe
	void Move(uint32_t steps, Dir dir, uint32_t periodUs);
	void MoveUntilBound(Dir dir, uint32_t periodUs);
};

class StepperEngine
{
private:
	enum {
		MAX_STEPPERS = 2
	};

	Stepper *m_steppers[MAX_STEPPERS];
	uint8_t m_numStepper;

public:
	enum {
		PRESCALER = 256,
		FREQUENCY = F_CPU / (PRESCALER * 2),
		PERIOD_US = ((uint32_t)(1e6)) * (PRESCALER * 2) / F_CPU
	};

	StepperEngine(Stepper *steppers[MAX_STEPPERS], uint8_t numStepper);

	static StepperEngine *singleton;

	void Register(Stepper *stepper);
	void Init();
	void Update();
	void ButtonPressed();
};
