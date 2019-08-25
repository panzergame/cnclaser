#pragma once

#include <avr/io.h>

/// Stepper motor controller
class Stepper
{
public:
	enum Dir {
		DOWN = true,
		UP = false
	};

private:
	/** @{
	 * Register used to manipulate port controller stepper driver.
	 */
	volatile uint8_t &m_port;
	volatile uint8_t &m_pin;
	volatile uint8_t &m_ddr;
	/// @}

	/** @{
	 * Pin number for direction, step, enable and button.
	 */
	uint8_t m_dirPin;
	uint8_t m_stepPin;
	uint8_t m_enablePin;
	uint8_t m_buttonPin;
	/// @}

	/// True if direction is inverted.
	bool m_invertDir;

public:
	Stepper() = default;
	Stepper(volatile uint8_t &port, volatile uint8_t &pin, volatile uint8_t &ddr,
			uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t buttonPin, bool invertDir);

	/// Initialize port.
	void Init();
	/// Execute moving sequence until pressing stop button.
	void Calibrate();

	/// Setup direction
	void SetDir(Dir dir);

	/** @{
	 * Process step by enabling and disabling step pin.
	 */
	void TicUp();
	void TicDown();
	/// @}

	/** @{
	 * Control stepper driver power management.
	 */
	void Enable();
	void Disable();
	/// @}
};
