#pragma once

#include <avr/io.h>

namespace Config
{
	// 62.6mm for 40000
	static constexpr float STEP_MM = 62.6 / 40000;
	// 1mm par segment.
	static constexpr float ARC_PRECISION = 0.3;
	/// Maximal line queue size.
	static constexpr uint8_t LINE_QUEUE_SIZE = 32;
	/// Speed when laser is off.
	static constexpr float LASER_OFF_SPEED = 1.0;
	/// Laser power off.
	static constexpr uint8_t LASER_OFF_INTENSITY = 0;
	/// Maximum command line size.
	static constexpr uint8_t CMD_LINE_SIZE = 48;
	/// Command queue length.
	static constexpr uint8_t CMD_QUEUE_SIZE = 8;
	/// USART tx queue size.
	static constexpr uint8_t USART_TX_QUEUE_SIZE = 16;
	/// Stepper timer prescaler.
	static constexpr uint16_t TIMER_PRESCALER = 1024;

	// Plate boundaries X, Y (min, max)
	static constexpr float BOUNDARIES[2][2] = {
		{0.0f, 70.0f},
		{0.0f, 70.0f}
	};
}
