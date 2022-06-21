#include "pch.h"
#include "cpu.h"
#include "display.h"
#include "input.h"
#include "memory.h"

#define VERBOSE 1

CPU::CPU() {
	m_instructions[0x00E0] = &CPU::CLS;  m_instructions[0x00EE] = &CPU::RET;  m_instructions[0x0000] = &CPU::SYS;  m_instructions[0x1000] = &CPU::JP;   m_instructions[0x2000] = &CPU::CALL;
	m_instructions[0x3000] = &CPU::SE;   m_instructions[0x4000] = &CPU::SNE;  m_instructions[0x5000] = &CPU::SE;   m_instructions[0x6000] = &CPU::LD;   m_instructions[0x7000] = &CPU::ADD;
	m_instructions[0x8000] = &CPU::LD;   m_instructions[0x8001] = &CPU::OR;   m_instructions[0x8002] = &CPU::AND;  m_instructions[0x8003] = &CPU::XOR;  m_instructions[0x8004] = &CPU::ADD;
	m_instructions[0x8005] = &CPU::SUB;  m_instructions[0x8006] = &CPU::SHR;  m_instructions[0x8007] = &CPU::SUBN; m_instructions[0x800E] = &CPU::SHL;  m_instructions[0x9000] = &CPU::SNE;
	m_instructions[0xA000] = &CPU::LD;   m_instructions[0xB000] = &CPU::JP;   m_instructions[0xC000] = &CPU::RND;  m_instructions[0xD000] = &CPU::DRW;  m_instructions[0xE09E] = &CPU::SKP;
	m_instructions[0xE0A1] = &CPU::SKNP; m_instructions[0xF007] = &CPU::LD;   m_instructions[0xF00A] = &CPU::LD;   m_instructions[0xF015] = &CPU::LD;   m_instructions[0xF018] = &CPU::LD;
	m_instructions[0xF01E] = &CPU::ADD;  m_instructions[0xF029] = &CPU::LD;   m_instructions[0xF033] = &CPU::LD;   m_instructions[0xF055] = &CPU::LD;   m_instructions[0xF065] = &CPU::LD;
}

void CPU::tick(Bus* pBus) {
	if (!pBus || !pBus->pMemory) {
		return;
	}

	if (m_dt > 0) {
		m_dt--;
	}

	uint16_t instruction = fetch(pBus->pMemory);
	execute(instruction, pBus);
}

uint16_t CPU::fetch(Memory* pMemory) {
	uint16_t instruction = pMemory->read_byte(m_pc) << 8;
	instruction |= pMemory->read_byte(m_pc + 1);
	m_pc += 2;
	return instruction;
}

void CPU::execute(uint16_t instruction, Bus* pBus) {
#if VERBOSE
	log_instruction(instruction);
#endif
	uint16_t opcode = instruction;
	switch (instruction >> 12) {
	case 0x0:
		if (opcode != 0x00E0 && opcode != 0x00EE) {
			opcode &= 0xF000;
		}
		break;
	case 0x8:
		opcode &= 0xF00F;
		break;
	case 0xE:
	case 0xF:
		opcode &= 0xF0FF;
		break;
	default:
		opcode &= 0xF000;
	}

	auto iter = m_instructions.find(opcode);
	if (iter != m_instructions.end()) {
		int8_t result = std::invoke(iter->second, this, instruction, pBus);
	}
	else {
		assert(false);
	}
}

void CPU::log_instruction(uint16_t instruction) {
	std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << instruction << std::endl;
}

uint8_t CPU::ADD(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x7:
#if VERBOSE
		std::cout << "7xkk - ADD Vx, byte" << std::endl;
#endif
		m_v[(instruction & 0x0F00) >> 8] += (uint8_t)(instruction & 0x00FF);
		break;
	case 0xF:
#if VERBOSE
		std::cout << "Fx1E - ADD I, Vx" << std::endl;
#endif
		m_i += m_v[(instruction & 0x0F00) >> 8];
		break;
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::AND(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::CALL(uint16_t instruction, Bus* pBus) {
#if VERBOSE
	std::cout << "CALL addr" << std::endl;
#endif
	m_stack[m_sp++] = m_pc;
	m_pc = instruction & 0x0FFF;

	return 0;
}

uint8_t CPU::CLS(uint16_t instruction, Bus* pBus) {
#if VERBOSE
		std::cout << "00E0 - CLS" << std::endl;
#endif
	if (!pBus->pDisplay) {
		std::cerr << "No display on found." << std::endl;
		return 0;
	}

	pBus->pDisplay->clear();

	return 0;
}

uint8_t CPU::DRW(uint16_t instruction, Bus* pBus) {
#if VERBOSE
	std::cout << "Dxyn - DRW Vx, Vy, nibble" << std::endl;
#endif
	if (!pBus->pDisplay) {
		std::cerr << "No display found." << std::endl;
		return 0;
	}

	uint32_t x_start = m_v[(instruction & 0x0F00) >> 8];
	uint32_t y_start = m_v[(instruction & 0x00F0) >> 4];
	uint16_t nibble = instruction & 0x000F;
	uint16_t offset = m_i;
	for (uint16_t y = 0; y < nibble; y++) {
		uint8_t byte = pBus->pMemory->read_byte(offset + y);
		for (int8_t i = 7, x = 0; i >= 0; i--, x++) {
			int8_t value = byte >> i & 0x0001;
			if (value == 1) {
				m_v[0xF] = !pBus->pDisplay->plot_pixel(x_start + x, y_start + y) ? 1 : 0;
			}
		}
	}

	return 0;
}

uint8_t CPU::JP(uint16_t instruction, Bus* pBus) {
#if VERBOSE
		std::cout << "1nnn - JP addr" << std::endl;
#endif
	m_pc = instruction & 0x0FFF;

	return 0;
}

uint8_t CPU::LD(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0xA:
#if VERBOSE
		std::cout << "Annn - LD I, addr" << std::endl;
#endif
		m_i = instruction & 0x0FFF;
		break;
	case 0x6:
#if VERBOSE
		std::cout << "6xkk - LD Vx, byte" << std::endl;
#endif
		m_v[(instruction & 0x0F00) >> 8] = (uint8_t)(instruction & 0x00FF);
		break;
	case 0xF:
		switch (instruction & 0x00FF) {
		case 0x07:
#if VERBOSE
			std::cout << "Fx07 - LD Vx, DT" << std::endl;
#endif
			m_v[(instruction & 0x0F00) >> 8] = m_dt;
			break;
		case 0x0A:
		{
#if VERBOSE
			std::cout << "Fx0A - LD Vx, K" << std::endl;
#endif
			int8_t key = 0;
			if (pBus->pInput && pBus->pInput->is_any_key_pressed(key)) {
				m_v[(instruction & 0x0F00) >> 8] = key;
			} else {
				m_pc -= 2;
			}
			break;
		}
		case 0x15:
#if VERBOSE
			std::cout << "Fx15 - LD DT, Vx" << std::endl;
#endif
			m_dt = m_v[(instruction & 0x0F00) >> 8];
			break;
		case 0x18:
#if VERBOSE
			std::cout << "Fx18 - LD ST, Vx" << std::endl;
#endif
			m_st = m_v[(instruction & 0x0F00) >> 8];
			break;
		case 0x29:
#if VERBOSE
			std::cout << "Fx29 - LD F, Vx" << std::endl;
#endif
			m_i = pBus->pMemory->read_font_address(m_v[(instruction & 0x0F00) >> 8]);
			break;
		case 0x55:
		{
#if VERBOSE
			std::cout << "Fx55 - LD [I], Vx" << std::endl;
#endif
			uint8_t n = (instruction & 0x0F00) >> 8;
			for (uint8_t i = 0; i <= n; i++) {
				pBus->pMemory->write_byte(m_i + i, m_v[i]);
			}
		} break;
		case 0x65:
		{
#if VERBOSE
			std::cout << "Fx65 - LD Vx, [I]" << std::endl;
#endif
			uint8_t n = (instruction & 0x0F00) >> 8;
			for (uint8_t i = 0; i <= n; i++) {
				m_v[i] = pBus->pMemory->read_byte(m_i + i);
			}
		} break;
		default:
			assert(false);
		}
	}

	return 0;
}

uint8_t CPU::OR(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::RET(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x0:
#if VERBOSE
		std::cout << "00EE - RET" << std::endl;
#endif
		m_pc = m_stack[--m_sp];
		break;
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::RND(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SE(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x3:
#if VERBOSE
		std::cout << "3xkk - SE Vx, byte" << std::endl;
#endif
		if (m_v[(instruction & 0x0F00) >> 8] == (uint8_t)(instruction & 0x00FF)) {
			m_pc += 2;
		}
		break;
	default:
		assert(false);
	}
	return 0;
}

uint8_t CPU::SHL(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x8:
	{
#if VERBOSE
		std::cout << "8xyE - SHL Vx {, Vy}." << std::endl;
#endif
		uint8_t x = m_v[(instruction & 0x0F00) >> 8];
		m_v[0xF] = x & 0x80;
		m_v[(instruction & 0x0F00) >> 8] = x << 1;
	} break;
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SHR(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x8:
	{
#if VERBOSE
		std::cout << "8xy6 - SHR Vx {, Vy}" << std::endl;
#endif
		uint8_t x = m_v[(instruction & 0x0F00) >> 8];
		m_v[0xF] = x & 0x01;
		m_v[(instruction & 0x0F00) >> 8] = x >> 1;
	} break;
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SNE(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	case 0x4:
#if VERBOSE
		std::cout << "4xkk - SNE Vx, byte" << std::endl;
#endif
		if (m_v[(instruction & 0x0F00) >> 8] != (uint8_t)(instruction & 0x00FF)) {
			m_pc += 2;
		}
		break;
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SKP(uint16_t instruction, Bus* pBus) {
	if (!pBus->pInput) {
		std::cerr << "No input found." << std::endl;
		return 0;
	}

#if VERBOSE
	std::cout << "Ex9E - SKP Vx" << std::endl;
#endif
	if (pBus->pInput->is_key_pressed(m_v[(instruction & 0x0F00) >> 8])) {
		m_pc += 2;
	}

	return 0;
}

uint8_t CPU::SKNP(uint16_t instruction, Bus* pBus) {
	if (!pBus->pInput) {
		std::cerr << "No input found." << std::endl;
		return 0;
	}

#if VERBOSE
	std::cout << "ExA1 - SKNP Vx" << std::endl;
#endif
	uint8_t key = m_v[(instruction & 0x0F00) >> 8];
	if (!pBus->pInput->is_key_pressed(key)) {
		m_pc += 2;
	}

	return 0;
}

uint8_t CPU::SUB(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SUBN(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::SYS(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}

uint8_t CPU::XOR(uint16_t instruction, Bus* pBus) {
	switch (instruction >> 12) {
	default:
		assert(false);
	}

	return 0;
}
