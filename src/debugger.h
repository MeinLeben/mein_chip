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

private:
	bool m_initialized = false;

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	TTF_Font* m_pFont = nullptr;
};