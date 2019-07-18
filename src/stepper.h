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

public:
	Stepper() = default;
	Stepper(volatile uint8_t &port, volatile uint8_t &pin, volatile uint8_t &ddr, volatile uint8_t &pcmsk, uint8_t pcie,
			uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t buttonPin, bool invertDir);

	void Init();

	void SetDir(Dir dir);
	void TicUp();
	void TicDown();
};
