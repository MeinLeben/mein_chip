#pragma once

#include "debugger.h"

class MeinChip : public wxApp {
public:
	~MeinChip();

	bool OnInit();
	void Update();

	void OnMainWindowClose(wxCloseEvent& event);

private:
	void render();

	void OnOpen(const void* pData);
	void update_window_title();

	bool m_initialized = false;
	class MainWindow* m_pMainWindow = nullptr;

	SDL_Renderer* m_pRenderer = nullptr;

	struct Rom {
		uint8_t* pData = nullptr;
		size_t size = 0;
	};

	Rom m_rom = {};

	class UpdateTimer* m_pUpdateTimer = nullptr;
	class Display* m_pDisplay = nullptr;
	class Input* m_pInput = nullptr;
	class Memory* m_pMemory = nullptr;
	class CPU* m_pCPU = nullptr;

	uint32_t m_num_cycles_per_tick = 1;
	uint32_t m_onOpenId = 0;

	std::unique_ptr<Debugger> m_debugger;
};
