#pragma once

#ifdef DEBUG
#define USE_DEBUGGER 1
#else
#define USE_DEBUGGER 0
#endif

class Debugger {
public:
	~Debugger();

	bool initialize(SDL_Window* pWindow = nullptr);
	void destroy();

	void handle_event(SDL_Event* pEvent);
	void tick();

	inline void show(bool show) {
		if (show) {
			SDL_ShowWindow(m_pWindow);
		} else {
			SDL_HideWindow(m_pWindow);
		}
		m_is_visible = show;
	}

	inline void minimize() {
		if (m_is_visible) {
			SDL_MinimizeWindow(m_pWindow);
		}
	}

	inline void restore() {
		if (m_is_visible) {
			SDL_RestoreWindow(m_pWindow);
		}
	}

	inline bool is_visible() const {
		return m_is_visible;
	}

private:
	bool m_initialized = false;
	bool m_is_visible = true;

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	class Font* m_pFont = nullptr;
	class Text* m_pText = nullptr;

	void update();
	void render();
};