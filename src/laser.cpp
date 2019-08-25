#include "laser.h"
#include "pwm.h"

void Laser::Enable(uint8_t intensity)
{
	MainPwm.SetDutyCycle(intensity);
}

void Laser::Disable()
{
	MainPwm.SetDutyCycle(0);
}
