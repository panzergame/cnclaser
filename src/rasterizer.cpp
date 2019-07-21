#include "rasterizer.h"
#include "timer.h"
#include "usart.h"

#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

bool Rasterizer::DrawLineStep(Line &line)
{
	line.elapsed += m_ticPeriod;

	if (line.elapsed >= line.period) {
		line.elapsed -= line.period;

		/*Serial.print(m_curLine);
		Serial.print(", ");
		Serial.println(line.stepsLeft);*/

		PORTB ^= _BV(PORTB5);

		FOREACH_AXIS {
			Axis &axis = line.axis[i];
			Stepper *stepper = m_steppers[i];

			stepper->SetDir(axis.dir);

			axis.over += axis.deltaAbs;
			if (axis.over >= line.steps) {
				/*Serial.print("Step ");
				Serial.print(axis.dir);
				Serial.print(", ");
				Serial.println(i);*/
				stepper->TicUp();
				axis.over -= line.steps;
// 				++axis.debug;
				stepper->TicDown();
			}
		}

		--line.stepsLeft;
	}

	/*if (line.stepsLeft == 0) {
		FOREACH_AXIS {
			Axis &axis = line.axis[i];

			Serial.print(axis.deltaAbs);
			Serial.print(", ");
			Serial.println(axis.debug);
		}
	}*/

	return (line.stepsLeft == 0);
}

void Rasterizer::Tic()
{
	if (!m_lines.Empty()) {
		// Dessin de la ligne actuelle.
		Line &line = *m_lines.Begin();
		if (DrawLineStep(line)) {
			// Actualisation de la dernière position.
			FOREACH_AXIS {
				m_pos[i] = line.pos[i];
			}

			// Suppression lorsque fini.
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

void Rasterizer::AddLine(float pos[NUM_AXIS], float speed)
{
	uint16_t ipos[NUM_AXIS];
	FOREACH_AXIS {
		ipos[i] = (uint16_t)(pos[i] / STEP_MM);
	}

	AddLine(ipos, speed / STEP_MM);
}

void Rasterizer::AddLine(uint16_t pos[NUM_AXIS], float speed)
{
	while (m_lines.Full());

	if (m_lines.Empty()) {
		FOREACH_AXIS {
			m_steppers[i]->Enable();
		}
	}

	Line line;
	line.elapsed = 0;
	line.steps = 0;

	Line *lastLine = m_lines.End();

	uint32_t dist2 = 0;
	FOREACH_AXIS {
		line.pos[i] = pos[i];

		const int16_t delta = ((int16_t)line.pos[i]) - ((int16_t)((lastLine) ? lastLine->pos[i] : m_pos[i]));

		Axis& axis = line.axis[i];
		axis.deltaAbs = abs(delta);
		axis.dir = (delta > 0) ? Stepper::UP : Stepper::DOWN;

		// Calcule de la distance total parcouru par les moteurs.
		dist2 += ((int32_t)axis.deltaAbs) * ((int32_t)axis.deltaAbs);

		// Recherche du max.
		if (axis.deltaAbs > line.steps) {
			line.steps = axis.deltaAbs;
		}
	}

	// Distance en nombre de pas.
	const float dist = sqrt((float)dist2);
	const float time = dist * 1e6 / speed;
	line.period = time / line.steps;
	line.stepsLeft = line.steps;

	/*char buf[200];
	sprintf(buf, "steps %i %f (%i %i) ", line.steps, (double)(time / line.steps), line.pos[0], line.pos[1]);
	MainUsart.Send(buf);*/

	FOREACH_AXIS {
		Axis& axis = line.axis[i];
		axis.over = line.steps / 2;

		/*uint32_t last = (lastLine) ? lastLine->pos[i] : 0;
		int32_t diff = ((int32_t)line.pos[i]) - ((int32_t)last);

		sprintf(buf, "Axis %i delta %li abs %li diff %li last %li\n\r", i, axis.delta, (int32_t)axis.deltaAbs, diff, last);
		MainUsart.Send(buf);*/
	}

	m_lines.Add(line);

	/*Line *newLine = m_lines.End();
	sprintf(buf, "Added %li %li steps %li\n\r", newLine->pos[0], newLine->pos[1], newLine->steps);
	MainUsart.Send(buf);*/
}
