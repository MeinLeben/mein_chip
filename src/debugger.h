#pragma once

#ifdef DEBUG
#define USE_DEBUGGER 1
#else
#define USE_DEBUGGER 0
#endif

#include "sdl_wrapper.h"
#include "font.h"
#include "text.h"

class Debugger {
public:
	static std::unique_ptr<Debugger> create(SDL_Window* pParent) {
		try {
			std::unique_ptr<Debugger> debugger(new Debugger(pParent));
			return debugger;
		} catch (std::exception& e) {
			std::cerr << "Failed to create the debugger window: " << e.what() << std::endl;
		}
		return nullptr;
	}

	void handle_event(SDL_Event* pEvent);
	void tick();

	inline void show(bool show) {
		if (show) {
			SDL_ShowWindow(m_window->get());
		} else {
			SDL_HideWindow(m_window->get());
		}
		m_visible = show;
	}

	inline void minimize() {
		if (m_visible) {
			SDL_MinimizeWindow(m_window->get());
		}
	}

	inline void restore() {
		if (m_visible) {
			SDL_RestoreWindow(m_window->get());
		}
	}

	inline bool is_visible() const {
		return m_visible;
	}

	void update_instruction(uint16_t instruction);
	void update_instruction_description(const std::string& description);
	void update_gp_registers(uint8_t* v, size_t size);
	void update_index_register(uint16_t i);
	void update_delay_timer(uint8_t dt);
	void update_sound_timer(uint8_t st);
	void update_program_counter(uint16_t pc);
	void update_stack_pointer(uint8_t sp);
	void update_stack(uint16_t* stack, size_t size);
	void update_error(const std::string& message);
	void update_keypad(bool* keys_state, size_t num_keys);

private:
	Debugger(SDL_Window* pParent = nullptr);

	void render();

	bool m_visible = true;

	Font::Handle m_default_font = Font::kInvalidHandle;
	Font::Handle m_error_font = Font::kInvalidHandle;

	std::unique_ptr<SDLWrapper::Window> m_window;
	std::unique_ptr<SDLWrapper::Renderer> m_renderer;
	std::unique_ptr<TextManager> m_text_manager;

	std::string m_error_text;

	TextLabel* m_instruction = nullptr;
	TextLabel* m_instruction_description = nullptr;
	TextField* m_gp_registers = nullptr;
	TextLabel* m_index_register = nullptr;
	TextLabel* m_delay_timer = nullptr;
	TextLabel* m_sound_timer = nullptr;
	TextLabel* m_program_counter = nullptr;
	TextLabel* m_stack_pointer = nullptr;
	TextField* m_stack = nullptr;
	TextField* m_error = nullptr;
	TextField* m_keypad[2] = {};
};