#include "debugger.h"

Debugger::~Debugger() {
	destroy();
}

bool Debugger::initialize(SDL_Window* pWindow) {
	int32_t x = SDL_WINDOWPOS_UNDEFINED;
	int32_t y = SDL_WINDOWPOS_UNDEFINED;

	if (pWindow) {
		int32_t w, h;
		SDL_GetWindowPosition(pWindow, &x, &y);
		SDL_GetWindowSize(pWindow, &w, &h);

		x += w + 20;
	}

	m_pWindow = SDL_CreateWindow(
		"Debugger",
		x,
		y,
		240,
		320,
		0);

	if (!m_pWindow) {
		std::cerr << "[Debugger] Failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

	if (!m_pRenderer) {
		std::cerr << "[Debugger] Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pFont = TTF_OpenFont("data/font/punk-mono/punk-mono-regular.ttf", 12);
	if (!m_pFont) {
		std::cerr << "[Debugger] Failed to open font: " << TTF_GetError() << std::endl;
	}

	m_initialized = true;
	
	return true;
}

void Debugger::destroy() {
	if (!m_initialized) {
		return;
	}

	SDL_DestroyRenderer(m_pRenderer);
	m_pRenderer = nullptr;

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	m_initialized = false;
}