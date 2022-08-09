#pragma once
class Memory;
class Debugger;
class Display;
class Input;

struct Bus {
	Memory* pMemory = nullptr;
	Display* pDisplay = nullptr;
	Input* pInput = nullptr;
};

class CPU
{
public:
	CPU(std::unique_ptr<Debugger>& debugger);

	void Tick(Bus* pBus, std::unique_ptr<Debugger>& debugger);

	void Reset();

private:
	uint8_t m_v[16] = {};
	uint16_t m_i = 0;
	uint8_t m_dt = 0;
	uint8_t m_st = 0;

	uint16_t m_pc = 0x0200;
	uint8_t m_sp = 0;

	uint16_t m_stack[16] = {};

	std::unordered_map <uint16_t, uint8_t(CPU::*)(uint16_t, Bus*, std::unique_ptr<Debugger>&)> m_instructions;

	uint16_t fetch(Memory* pMemory);
	void execute(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);

	void log_instruction(uint16_t instruction);

	uint8_t ADD(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t AND(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t CALL(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t CLS(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t DRW(uint16_t instruciton, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t JP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t LD(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t OR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t RET(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t RND(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SE(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SHL(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SHR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SKP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SKNP(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SNE(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SUB(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SUBN(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t SYS(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);
	uint8_t XOR(uint16_t instruction, Bus* pBus, std::unique_ptr<Debugger>& debugger);

};
