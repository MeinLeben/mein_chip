#pragma once

#include "debugger.h"

class MeinChip : public wxApp {
public:
	~MeinChip();

	bool OnInit();
	void Update();

	class Input* GetInput() const {
		return m_pInput;
	}

private:
	struct Rom {
		uint8_t* pData = nullptr;
		size_t size = 0;
	};

	void OnKeyUp(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnMainWindowClose(wxCloseEvent& event);

	void Reset();
	void Render();

	void UpdateWindowTitle();

	bool m_initialized = false;
	bool m_romLoaded = false;
	bool m_pause = true;
	bool m_step = false;

	class MainWindow* m_pMainWindow = nullptr;

	SDL_Renderer* m_pRenderer = nullptr;

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
