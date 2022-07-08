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

private:
	Debugger(SDL_Window* pParent = nullptr);

	void update();
	void render();

	bool m_visible = true;

	Font::Handle m_font = Font::kInvalidHandle;

	std::unique_ptr<SDLWrapper::Window> m_window;
	std::unique_ptr<SDLWrapper::Renderer> m_renderer;
	std::unique_ptr<TextManager> m_text_manager;

	TextLabel* m_text_label = nullptr;
	TextField* m_text_field = nullptr;
};