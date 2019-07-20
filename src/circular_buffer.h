#pragma once

#include <stdint.h>

template <class Data, uint16_t _Size>
class CircularBuffer
{
public:
	static constexpr uint16_t Size = _Size;

private:
	volatile uint16_t m_begin;
	volatile uint16_t m_next;
	volatile uint16_t m_len;


	static uint8_t Next(uint8_t index)
	{
		return (index + 1) % Size;
	}

	static uint8_t Prev(uint8_t index)
	{
		return (index - 1) % Size;
	}

public:
	Data m_data[Size];
	CircularBuffer()
		:m_begin(0),
		m_next(0),
		m_len(0)
	{
	}

	bool Empty() const
	{
		return (m_len == 0);
	}

	bool Full() const
	{
		return (m_len == Size);
	}

	bool Add(const Data& data)
	{
		if (Full()) {
			return false;
		}

		m_data[m_next] = data;
		m_next = Next(m_next);
		++m_len;

		return true;
	}

	bool Add(const Data *datas)
	{
		uint8_t i = 0;
		while (datas[i]) {
			if (!Add(datas[i])) {
				return false;
			}

			++i;
		}

		return true;
	}

	Data *RemoveBegin()
	{
		if (Empty()) {
			return nullptr;
		}

		Data *data = &m_data[m_begin];
		m_begin = Next(m_begin);
		--m_len;

		return data;
	}

	Data *Begin()
	{
		if (Empty()) {
			return nullptr;
		}

		return &m_data[m_begin];
	}

	Data *End()
	{
		if (Empty()) {
			return nullptr;
		}

		return &m_data[Prev(m_next)];
	}
};
