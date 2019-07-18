Rasterizer::Line *Rasterizer::GetLastLine() const
{
	if (BufferEmpty()) {
		return nullptr;
	}

	return &m_lines[(m_nextLine - 1) % MAX_LINE];
}

void Rasterizer::NextLine()
{
	m_curLine = (m_curLine + 1) % MAX_LINE;
	--m_nbLine;
}

bool Rasterizer::BufferFull()
{
	return m_nbLine == MAX_LINE;
}

bool Rasterizer::BufferEmpty()
{
	return m_nbLine == 0;
}

bool Rasterizer::DrawLineStep(Line &line)
{
	line.elapsed += Timer::PERIOD_US;

	if (line.elapsed >= line.period) {
		line.elapsed -= line.period;

		/*Serial.print(m_curLine);
		Serial.print(", ");
		Serial.println(line.stepsLeft);*/

		PORTB ^= _BV(PORTB5);

		FOREACH_AXIS {
			Axis &axis = line.axis[i];
			m_steppers[i]->SetDir(axis.dir);

			axis.over += axis.deltaAbs;
			if (axis.over >= line.steps) {
				/*Serial.print("Step ");
				Serial.print(axis.dir);
				Serial.print(", ");
				Serial.println(i);*/
				m_steppers[i]->TicUp();
				axis.over -= line.steps;
// 				++axis.debug;
				m_steppers[i]->TicDown();
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
	if (!BufferEmpty()) {
		Line &line = m_lines[m_curLine];
		if (DrawLineStep(line)) {
			NextLine();
		}
	}
}

Rasterizer::Rasterizer(Stepper *steppers[NUM_AXIS])
	:m_nextLine(0),
	m_curLine(0),
	m_nbLine(0)
{
	FOREACH_AXIS {
		m_steppers[i] = steppers[i];
	}
}

void Rasterizer::AddLine(double pos[NUM_AXIS], double speed)
{
	uint32_t ipos[NUM_AXIS];
	FOREACH_AXIS {
		ipos[i] = (uint32_t)pos[i] / STEP_MM;
	}

	AddLine(ipos, speed / STEP_MM);
}

void Rasterizer::AddLine(uint32_t pos[NUM_AXIS], double speed)
{
	while (BufferFull()) {
		/*Serial.println("overflow");
		Serial.println(m_nextLine);*/
		delay(1000);
	}

	Line &line = m_lines[m_nextLine];
	line.elapsed = 0;
	line.steps = 0;

	Line *lastLine = GetLastLine();

	double dist2 = 0.0;
	FOREACH_AXIS {
		line.pos[i] = pos[i];

		Axis& axis = line.axis[i];
		axis.delta = ((int32_t)line.pos[i]) - ((int32_t)((lastLine) ? lastLine->pos[i] : 0));
		axis.deltaAbs = abs(axis.delta);
		axis.dir = (axis.delta > 0) ? Stepper::UP : Stepper::DOWN;
		axis.debug = 0;

		dist2 += axis.deltaAbs * axis.deltaAbs;

		// Recherche du max.
		if (axis.deltaAbs > line.steps) {
			line.steps = axis.deltaAbs;
		}
	}

	// Distance en nombre de pas.
	const double dist = sqrt(dist2);
	const double time = dist * 1e6 / speed;
	line.period = time / line.steps;
	line.stepsLeft = line.steps;

	/*Serial.print("dist ");
	Serial.print(dist);
	Serial.print(", time ");
	Serial.print(time);
	Serial.print(", speed ");
	Serial.print(speed);

	char buf[200];
	sprintf(buf, "Line %i steps %i period ", m_nextLine, line.steps);
	Serial.print(buf);
	Serial.print(line.period);

	Serial.print(" X :");
	Serial.print(pos[0]);
	Serial.print(", Y :");
	Serial.println(pos[1]);*/

	FOREACH_AXIS {
		Axis& axis = line.axis[i];
		axis.over = line.steps / 2;

		/*sprintf(buf, "Axis %i delta %i abs %i", i, axis.delta, (int32_t)axis.deltaAbs);
		Serial.println(buf);
		Serial.println(axis.deltaAbs);*/
	}

	m_nextLine = (m_nextLine + 1) % MAX_LINE;
	++m_nbLine;
}
