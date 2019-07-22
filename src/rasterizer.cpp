#include "rasterizer.h"
#include "timer.h"
#include "usart.h"

#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static float atan3(float dy,float dx)
{
	float a = atan2(dy, dx);
	if (a < 0.0) {
		a = (M_PI * 2.0) + a;
	}
	return a;
}

bool Rasterizer::DrawLineStep(Line &line)
{
	line.elapsed += m_ticPeriod;

	if (line.elapsed >= line.period) {
		line.elapsed -= line.period;

		PORTB ^= _BV(PORTB5);

		FOREACH_AXIS {
			Axis &axis = line.axis[i];
			Stepper *stepper = m_steppers[i];

			stepper->SetDir(axis.dir);

			axis.over += axis.deltaAbs;
			if (axis.over >= line.steps) {
				stepper->TicUp();
				axis.over -= line.steps;
				stepper->TicDown();
			}
		}

		--line.stepsLeft;
	}

	return (line.stepsLeft == 0);
}

void Rasterizer::Tic()
{
	if (!m_lines.Empty()) {
		// Allumer les moteurs.
		FOREACH_AXIS {
			m_steppers[i]->Enable();
		}

		// Dessin de la ligne actuelle.
		Line &line = *m_lines.Begin();
		if (DrawLineStep(line)) {
			// Suppression lorsque fini.
// 			MainUsart.Send("Done\n");
			m_lines.RemoveBegin();

			// Si plus de commande éteindre les moteurs.
			if (m_lines.Empty()) {
				FOREACH_AXIS {
					m_steppers[i]->Disable();
				}
			}
		}
	}
}

Rasterizer::Rasterizer(Stepper *steppers[NUM_AXIS], float ticPeriod)
	:m_ticPeriod(ticPeriod)
{
	FOREACH_AXIS {
		m_steppers[i] = steppers[i];
		m_pos[i] = 0;
	}
}

void Rasterizer::AddLine(const float pos[NUM_AXIS], float speed)
{
	uint16_t ipos[NUM_AXIS];
	FOREACH_AXIS {
		ipos[i] = (uint16_t)(pos[i] / STEP_MM);
	}

	AddLine(ipos, speed / STEP_MM);
}

void Rasterizer::AddLine(const uint16_t pos[NUM_AXIS], float speed)
{
	// On évite les lines vide.
	bool equal = true;
	FOREACH_AXIS {
		if (pos[i] != m_pos[i]) {
			equal = false;
			break;
		}
	}

	// Attendre si le buffer est plein.
	while (m_lines.Full());

	Line line;
	line.elapsed = 0;
	line.steps = 0;

	uint32_t dist2 = 0;
	FOREACH_AXIS {
		line.pos[i] = pos[i];

		// Pas à réaliser.
		const int16_t delta = ((int16_t)line.pos[i]) - ((int16_t)m_pos[i]);

		Axis& axis = line.axis[i];
		axis.deltaAbs = abs(delta);
		axis.dir = (delta > 0) ? Stepper::UP : Stepper::DOWN;

		// Calcule de la distance total parcouru par les moteurs.
		dist2 += ((int32_t)axis.deltaAbs) * ((int32_t)axis.deltaAbs);

		// Recherche du max.
		if (axis.deltaAbs > line.steps) {
			line.steps = axis.deltaAbs;
		}

		// Actualisation de la dernière position.
		m_pos[i] = pos[i];
	}

	// Distance en nombre de pas.
	const float dist = sqrt((float)dist2);
	const float time = dist * 1e6 / speed;
	line.period = time / line.steps;
	line.stepsLeft = line.steps;

	/*char buf[100];
	sprintf(buf, "%i %i %i\n\r", pos[0], pos[1], line.steps);
	MainUsart.Send(buf);*/

	FOREACH_AXIS {
		Axis& axis = line.axis[i];
		// Dépassement initiale avant de faire un pas.
		axis.over = line.steps / 2;
	}

	/*char buf[200];
	sprintf(buf, "%i %f\n\r", line.steps, dist);
	MainUsart.Send(buf);*/

	m_lines.Add(line);
}

void Rasterizer::AddCircle(const float pos[NUM_AXIS], const float rel[NUM_AXIS], ArcDir dir, float speed)
{
	float radius2 = 0.0;
	// Vecteur centre début.
	float dstart[2];
	// Vecteur centre fin.
	float dend[2];
	float center[2];

	FOREACH_AXIS {
		radius2 += rel[i] * rel[i];
		dstart[i] = -rel[i];
		center[i] = m_pos[i] * STEP_MM + rel[i];
		dend[i] = pos[i] - center[i];
	}

	const float radius = sqrt(radius2);

	// Angle start.
	float angle1 = atan3(dstart[1], dstart[0]);
	// Angle end.
	float angle2 = atan3(dend[1], dend[0]);
	// Arc angle.
	float theta = angle2 - angle1;

	if (dir > 0 && theta < 0.0) {
		angle2 += 2.0 * M_PI;
	}
	else if (dir < 0 && theta > 0.0) {
		angle1 += 2.0 * M_PI;
	}

	theta = angle2 - angle1;

	const float len = fabs(theta) * radius;
	const uint32_t segments = ceil(len / ARC_PRECISION);

// 	char buf[200];
	/*char buf[200];
	sprintf(buf, "a1 %f a2 %f nb %li (%f %f) (%f %f)\n\r", angle1, angle2, segments, dstart[0], dstart[1], dend[0], dend[1]);
	MainUsart.Send(buf);*/

	for (uint32_t i = 1; i < segments; ++i) {
		const float scale = ((float)i) / ((float)segments);
		const float angle3 = (theta * scale) + angle1;

		// Position d'un point sur l'arc.
		const float spos[2] = {center[0] + cos(angle3) * radius,
							   center[1] + sin(angle3) * radius};

// 		_delay_ms(10);

		AddLine(spos, speed);

		/*sprintf(buf, "x %f y %f\n\r", spos[0], spos[1]);
		MainUsart.Send(buf);*/
	}


	/*sprintf(buf, "x %f y %f\n\r", pos[0], pos[1]);
	MainUsart.Send(buf);*/
	AddLine(pos, speed);
}
