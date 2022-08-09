#include "cpu.h"
#include "display.h"
#include "debugger.h"
#include "input.h"
#include "memory.h"

std::string instruction_to_string(uint16_t instruction) {
	std::stringstream ss;
	ss << "0x" << std::setfill('0') << std::uppercase
		<< std::setw(sizeof(uint16_t) * 2)
		<< std::hex << uint32_t(instruction);
	return ss.str();
}

CPU::CPU(std::unique_ptr<Debugger>& debugger) {
	m_instructions[0x00E0] = &CPU::CLS;  m_instructions[0x00EE] = &CPU::RET;  m_instructions[0x0000] = &CPU::SYS;  m_instructions[0x1000] = &CPU::JP;   m_instructions[0x2000] = &CPU::CALL;
	m_instructions[0x3000] = &CPU::SE;   m_instructions[0x4000] = &CPU::SNE;  m_instructions[0x5000] = &CPU::SE;   m_instructions[0x6000] = &CPU::LD;   m_instructions[0x7000] = &CPU::ADD;
	m_instructions[0x8000] = &CPU::LD;   m_instructions[0x8001] = &CPU::OR;   m_instructions[0x8002] = &CPU::AND;  m_instructions[0x8003] = &CPU::XOR;  m_instructions[0x8004] = &CPU::ADD;
	m_instructions[0x8005] = &CPU::SUB;  m_instructions[0x8006] = &CPU::SHR;  m_instructions[0x8007] = &CPU::SUBN; m_instructions[0x800E] = &CPU::SHL;  m_instructions[0x9000] = &CPU::SNE;
	m_instructions[0xA000] = &CPU::LD;   m_instructions[0xB000] = &CPU::JP;   m_instructions[0xC000] = &CPU::RND;  m_instructions[0xD000] = &CPU::DRW;  m_instructions[0xE09E] = &CPU::SKP;
	m_instructions[0xE0A1] = &CPU::SKNP; m_instructions[0xF007] = &CPU::LD;   m_instructions[0xF00A] = &CPU::LD;   m_instructions[0xF015] = &CPU::LD;   m_instructions[0xF018] = &CPU::LD;
	m_instructions[0xF01E] = &CPU::ADD;  m_instructions[0xF029] = &CPU::LD;   m_instructions[0xF033] = &CPU::LD;   m_instructions[0xF055] = &CPU::LD;   m_instructions[0xF065] = &CPU::LD;

	if (debugger) {
		debugger->update_instruction(0, "Unknown");
		debugger->update_gp_registers(m_v, 16);
		debugger->update_index_register(m_i);
		debugger->update_delay_timer(m_dt);
		debugger->update_sound_timer(m_st);
		debugger->update_program_counter(m_pc);
		debugger->update_stack_pointer(m_sp);
		debugger->update_stack(m_stack, 16);
	}
}

void CPU::Tick(Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (!pBus || !pBus->pMemory) {
		return;
	}

	if (m_dt > 0) {
		m_dt--;
	}

	uint16_t instruction = fetch(pBus->pMemory);
	execute(instruction, pBus, debugger);

	if (debugger) {
		debugger->update_gp_registers(m_v, 16);
		debugger->update_index_register(m_i);
		debugger->update_delay_timer(m_dt);
		debugger->update_sound_timer(m_st);
		debugger->update_program_counter(m_pc);
		debugger->update_stack_pointer(m_sp);
		debugger->update_stack(m_stack, 16);
	}
}

void CPU::Reset() {
	memset(m_v, 0, sizeof(uint8_t) * 16);
	m_i = 0;
	m_dt = 0;
	m_st = 0;

	m_pc = 0x0200;
	m_sp = 0;

	memset(m_stack, 0, sizeof(uint16_t) * 16);
}

uint16_t CPU::fetch(Memory* pMemory) {
	uint16_t instruction = pMemory->read_byte(m_pc) << 8;
	instruction |= pMemory->read_byte(m_pc + 1);
	m_pc += 2;
	return instruction;
}

void CPU::execute(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	uint16_t opcode = instruction;
	switch (instruction >> 12) {
		case 0x0: {
			if (opcode != 0x00E0 && opcode != 0x00EE) {
				opcode &= 0xF000;
			}
		} break;
		case 0x8: {
			opcode &= 0xF00F;
		} break;
		case 0xE:
		case 0xF: {
			opcode &= 0xF0FF;
		} break;
		default: {
			opcode &= 0xF000;
		}
	}

	auto iter = m_instructions.find(opcode);
	if (iter != m_instructions.end()) {
		int8_t result = std::invoke(iter->second, this, instruction, pBus, debugger);
	}

	else {
		if (debugger) {
			debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
		}
	}
}

uint8_t CPU::ADD(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x7: {
			if (debugger) {
				debugger->update_instruction(instruction, "7xkk - ADD Vx, byte");
			}
	
			m_v[(instruction & 0x0F00) >> 8] += (uint8_t)(instruction & 0x00FF);
		} break;
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xy4 - ADD Vx, Vy");
			}
	
			uint16_t t = m_v[(instruction & 0x0F00) >> 8] + m_v[(instruction & 0x00F0) >> 4];
			m_v[0xF] = t > 0xFF;
			m_v[(instruction & 0x0F00) >> 8] = (uint8_t)t;
		} break;
		case 0xF: {
			if (debugger) {
				debugger->update_instruction(instruction, "Fx1E - ADD I, Vx");
			}
		
			m_i += m_v[(instruction & 0x0F00) >> 8];
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::AND(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x8:
		{
		if (debugger) {
				debugger->update_instruction(instruction, "8xy2 - AND Vx, Vy");
			}
	
			m_v[(instruction & 0x0F00) >> 8] &= m_v[(instruction & 0x00F0) >> 4];
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::CALL(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (debugger) {
		debugger->update_instruction(instruction, "CALL addr");
	}

	m_stack[m_sp++] = m_pc;
	m_pc = instruction & 0x0FFF;

	return 0;
}

uint8_t CPU::CLS(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (debugger) {
		debugger->update_instruction(instruction, "00E0 - CLS");
	}

	if (!pBus->pDisplay) {
		if (debugger) {
			debugger->update_error("No display found.");
		}
		else {
			std::cerr << "No display found." << std::endl;
		}
		return 0;
	}

	pBus->pDisplay->clear();

	return 0;
}

uint8_t CPU::DRW(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (debugger) {
		debugger->update_instruction(instruction, "Dxyn - DRW Vx, Vy, nibble");
	}

	if (!pBus->pDisplay) {
		if (debugger) {
			debugger->update_error("No display found.");
		}
		else {
			std::cerr << "No display found." << std::endl;
		}
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

uint8_t CPU::JP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (debugger) {
		debugger->update_instruction(instruction, "1nnn - JP addr");
	}

	m_pc = instruction & 0x0FFF;

	return 0;
}

uint8_t CPU::LD(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0xA: {
			if (debugger) {
				debugger->update_instruction(instruction, "Annn - LD I, addr");
			}
			m_i = instruction & 0x0FFF;
		} break;
		case 0x6: {
			if (debugger) {
				debugger->update_instruction(instruction, "6xkk - LD Vx, byte");
			}
			m_v[(instruction & 0x0F00) >> 8] = (uint8_t)(instruction & 0x00FF);
		} break;
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xy0 - LD Vx, Vy");
			}
			m_v[(instruction & 0x0F00) >> 8] = m_v[(instruction & 0x00F0) >> 4];
		} break;
		case 0xF: {
			switch (instruction & 0x00FF) {
				case 0x07: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx07 - LD Vx, DT");
					}
		
					m_v[(instruction & 0x0F00) >> 8] = m_dt;
				} break;
				case 0x0A: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx0A - LD Vx, K");
					}
		
					int8_t key = 0;
					if (pBus->pInput && pBus->pInput->is_any_key_pressed(key)) {
						m_v[(instruction & 0x0F00) >> 8] = key;
					} else {
						m_pc -= 2;
					}
				} break;
				case 0x15: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx15 - LD DT, Vx");
					}
		
					m_dt = m_v[(instruction & 0x0F00) >> 8];
				} break;
				case 0x18: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx18 - LD ST, Vx");
					}
		
					m_st = m_v[(instruction & 0x0F00) >> 8];
				} break;
				case 0x29: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx29 - LD F, Vx");
					}
		
					m_i = pBus->pMemory->read_font_address(m_v[(instruction & 0x0F00) >> 8]);
				} break;
				case 0x33: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx33 - LD B, Vx");
					}

					uint8_t v = m_v[(instruction & 0x0F00) >> 8];

					uint8_t o = v % 10;
					uint8_t t = v % 100 - o;
					uint8_t h = v % 1000 - t - o;

					t /= 10;
					h /= 100;

					pBus->pMemory->write(m_i + 2, &o, sizeof(uint8_t));
					pBus->pMemory->write(m_i + 1, &t, sizeof(uint8_t));
					pBus->pMemory->write(m_i, &h, sizeof(uint8_t));
				} break;
				case 0x55: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx55 - LD [I], Vx");
					}
		
					uint8_t n = (instruction & 0x0F00) >> 8;
					for (uint8_t i = 0; i <= n; i++) {
						pBus->pMemory->write_byte(m_i + i, m_v[i]);
					}
				} break;
				case 0x65: {
					if (debugger) {
						debugger->update_instruction(instruction, "Fx65 - LD Vx, [I]");
					}
		
					uint8_t n = (instruction & 0x0F00) >> 8;
					for (uint8_t i = 0; i <= n; i++) {
						m_v[i] = pBus->pMemory->read_byte(m_i + i);
					}
				} break;
				default: {
					if (debugger) {
						debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
					}
				}
			}
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::OR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::RET(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x0: {
			if (debugger) {
				debugger->update_instruction(instruction, "00EE - RET");
			}
			m_pc = m_stack[--m_sp];
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::RND(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0xC: {
			if (debugger) {
				debugger->update_instruction(instruction, "Cxkk - RND Vx, byte");
			}

			m_v[(instruction & 0x0F00) >> 8] = ((uint8_t)m_v[(instruction & 0x00FF)]) & std::rand() % 256;
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SE(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x3: {
			if (debugger) {
				debugger->update_instruction(instruction, "3xkk - SE Vx, byte");
			}
	
			if (m_v[(instruction & 0x0F00) >> 8] == (uint8_t)(instruction & 0x00FF)) {
				m_pc += 2;
			}
		} break;
		case 0x5: {
			if (debugger) {
				debugger->update_instruction(instruction, "5xy0 - SE Vx, Vy");
			}
	
			if (m_v[(instruction & 0x0F00) >> 8] == m_v[(instruction & 0x00F0) >> 4]) {
				m_pc += 2;
			}
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SHL(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xyE - SHL Vx {, Vy}.");
			}
	
			uint8_t x = m_v[(instruction & 0x0F00) >> 8];
			m_v[0xF] = x & 0x80;
			m_v[(instruction & 0x0F00) >> 8] = x << 1;
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SHR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xy6 - SHR Vx {, Vy}");
			}
	
			uint8_t x = m_v[(instruction & 0x0F00) >> 8];
			m_v[0xF] = x & 0x01;
			m_v[(instruction & 0x0F00) >> 8] = x >> 1;
		} break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SNE(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x4: {
			if (debugger) {
				debugger->update_instruction(instruction, "4xkk - SNE Vx, byte");
			}
		
			if (m_v[(instruction & 0x0F00) >> 8] != (uint8_t)(instruction & 0x00FF)) {
				m_pc += 2;
			}
		} break;
		case 0x9: {
			if (debugger) {
				debugger->update_instruction(instruction, "9xy0 - SNE Vx, Vy");
			}
		
			if (m_v[(instruction & 0x0F00) >> 8] != m_v[(instruction & 0x00F0) >> 4]) {
				m_pc += 2;
			}
		} break;

		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SKP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (!pBus->pInput) {
		if (debugger) {
			debugger->update_error("No input found.");
		}
		else {
			std::cerr << "No input found." << std::endl;
		}
		return 0;
	}

	if (debugger) {
		debugger->update_instruction(instruction, "Ex9E - SKP Vx");
	}

	if (pBus->pInput->is_key_pressed(m_v[(instruction & 0x0F00) >> 8])) {
		m_pc += 2;
	}

	return 0;
}

uint8_t CPU::SKNP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	if (!pBus->pInput) {
		if (debugger) {
			debugger->update_error("No input found.");
		}
		else {
			std::cerr << "No input found." << std::endl;
		}
		return 0;
	}

	if (debugger) {
		debugger->update_instruction(instruction, "ExA1 - SKNP Vx");
	}

	uint8_t key = m_v[(instruction & 0x0F00) >> 8];
	if (!pBus->pInput->is_key_pressed(key)) {
		m_pc += 2;
	}

	return 0;
}

uint8_t CPU::SUB(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xy5 - SUB Vx, Vy");
			}

			int8_t x = m_v[(instruction & 0x0F00) >> 8];
			int8_t y = m_v[(instruction & 0x00F0) >> 4];
			m_v[0xF] = (x > y) ? 1 : 0;
			m_v[(instruction & 0x0F00) >> 8] -= y; 
		}break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SUBN(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::SYS(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}

uint8_t CPU::XOR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger) {
	switch (instruction >> 12) {
		case 0x8: {
			if (debugger) {
				debugger->update_instruction(instruction, "8xy3 - XOR Vx, Vy");
			}
	
			m_v[(instruction & 0x0F00) >> 8] ^= m_v[(instruction & 0x00F0) >> 4];
		}break;
		default: {
			if (debugger) {
				debugger->update_error("Instruction: " + instruction_to_string(instruction) + ", not implemented.");
			}
		}
	}

	return 0;
}
