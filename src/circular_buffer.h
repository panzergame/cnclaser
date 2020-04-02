#pragma once

#include <stdint.h>

/** Fixed size circular buffer
 * @param Data Data type
 * @param _Size Buffer size
 */
template <class Data, uint16_t _Size>
class CircularBuffer
{
public:
	static constexpr uint16_t Size = _Size;

private:
	volatile uint16_t m_begin;
	volatile uint16_t m_next;
	volatile uint16_t m_len;

	/// Get next wrapped index
	static uint8_t Next(uint8_t index)
	{
		return (index + 1) % Size;
	}

	/// Get prev wrapped index
	static uint8_t Prev(uint8_t index)
	{
		return (index - 1) % Size;
	}

	Data m_data[Size];

public:
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

	uint16_t Len() const
	{
		return m_len;
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

	/** Append a null terminated array
	 * @param datas Array of data null terminated
	 */
	bool Add(const Data *datas)
	{
		for (Data *data = datas; *data; ++data) {
			if (!Add(data)) {
				return false;
			}
		}

		return true;
	}

	/** Remove first element and return its value
	 * @return first element
	 */
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

	/// Return first element or null if empty
	Data *Begin()
	{
		if (Empty()) {
			return nullptr;
		}

		return &m_data[m_begin];
	}

	/// Return last element or null if empty
	Data *End()
	{
		if (Empty()) {
			return nullptr;
		}

		return &m_data[Prev(m_next)];
	}
};
