#pragma once

#include <cassert>
#include <span>
#include <thread>

#define THE_SHARED_MEMORY_NAME "MySharedMemory_WithSomeRandomStuff_3vT$J6exS9&n^$"
#define THE_SHARED_MEMORY_SIZE 65'536

enum State : u8 {
	STATE_INPROCESS_BY_ASSIGNED_CLIENT = 0,
	STATE_ASSINED_TO_CLIENT_1 = 1,
	STATE_ASSINED_TO_CLIENT_2,
	STATE_ASSINED_TO_CLIENT_3,
	STATE_ASSINED_TO_CLIENT_4,
	STATE_ASSINED_TO_CLIENT_5,

	STATE_ERROR = 0Xfe,
	STATE_COMPLETED_BY_ASSIGNED_CLIENT = 0Xff,
};

class CircularMemoryQueue_UNSAFE
{
private:
	using ptr_type  = u8*;
	using size_type = u32;
	static constexpr size_type DATA_PTR_OFFSET = (sizeof(size_type) + sizeof(size_type)) / sizeof(std::remove_pointer_t<ptr_type>);

	u8*        const m_DataPtr;
	size_type  const m_Capacity;
	size_type&       m_Start;
	size_type&       m_Size;
public:
	explicit CircularMemoryQueue_UNSAFE(u8* mem, size_type capacity)
		: m_DataPtr{mem + DATA_PTR_OFFSET}
		, m_Capacity(capacity - DATA_PTR_OFFSET)
		, m_Start(*reinterpret_cast<size_type*>(mem))
		, m_Size(*reinterpret_cast<size_type*>(mem + sizeof(m_Start)))
	{
		ASSERT(capacity != 0);
		ASSERT(mem != nullptr);
	}

	const u32 capacity() const {
		return m_Capacity;
	}

	const u32 size() const {
		return m_Size;
	}

	const u32 available_space() {
		return m_Capacity - m_Size;
	}

	b8 push_back(std::span<u8> data) {
		if (data.size() > available_space()) {
			return false;
		}

		// first part
		const u32 first_part_from = m_Start + m_Size % m_Capacity;
		const u32 first_part_size = std::min<u32>(data.size(), m_Capacity - first_part_from);
		std::copy(data.begin(), data.begin() + first_part_size, m_DataPtr + first_part_from);

		if (first_part_size != data.size()) {
			// second part
			const u32 second_part_size = data.size() - first_part_size;
			std::copy(data.begin() + first_part_size, data.end(), m_DataPtr);
		}

		m_Size += data.size();
		return true;
	}

	b8 pop_front(const u32 size) {
		if (size > m_Size) {
			return false;
		}

		m_Start = (m_Start + size) % m_Capacity;
		m_Size -= size;
		return true;
	}

	b8 pop_back(const u32 size) {
		if (size > m_Size) {
			return false;
		}

		m_Size -= size;
		return true;
	}

	u8& operator[](const u32 index) {
		ASSERT(index < m_Size);
		return m_DataPtr[(m_Start + index) % m_Capacity];
	}

	const std::pair<ptr_type, size_type> get_raw_construct_data() {
		return {m_DataPtr - DATA_PTR_OFFSET, m_Capacity + DATA_PTR_OFFSET};
	}
};