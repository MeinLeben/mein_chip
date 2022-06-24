#pragma once

class Memory
{
public:
	Memory(size_t size);

	void write_byte(uint16_t address, uint8_t data) {
		assert((size_t)address < m_size);
		m_pMemory[address] = data;
	}

	uint8_t read_byte(uint16_t address) const {
		assert((size_t)address < m_size);
		return m_pMemory[address];
	}

	void write(uint16_t address, uint8_t* pData, size_t size) {
		assert((size_t)address + size < m_size);
#ifdef WIN32
		errno_t err = memcpy_s(m_pMemory + address, m_size, pData, size);
		if (err != 0) {
			throw std::runtime_error("Failed to write to memory.");
		}
#else
		memcpy(m_pMemory + address, pData, size);
#endif
	}

	uint16_t read_font_address(uint8_t value) {
		assert(value < 16);
		return 0x50 + (5 * value);
	}

private:
	size_t m_size = 0;
	uint8_t* m_pMemory = nullptr;
	uint8_t m_fontAddress[0xF];
};

