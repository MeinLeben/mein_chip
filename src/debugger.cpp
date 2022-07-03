#include "debugger.h"

#include "font.h"
#include "text.h"

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

	m_pWindow = SDL_CreateWindow("Debugger", x, y, 240, 320, 0);

	if (!m_pWindow) {
		std::cerr << "[Debugger] Failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

	if (!m_pRenderer) {
		std::cerr << "[Debugger] Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pFont = new Font("data/font/punk-mono/punk-mono-regular.ttf", m_pRenderer);
	m_pText = new Text(m_pFont);

	m_initialized = true;
	
	return true;
}

void Debugger::destroy() {
	if (!m_initialized) {
		return;
	}

	delete m_pText;
	delete m_pFont;

	SDL_DestroyRenderer(m_pRenderer);
	m_pRenderer = nullptr;

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	m_initialized = false;
}

void Debugger::handle_event(SDL_Event* pEvent) {
	if (!pEvent || pEvent->window.windowID != SDL_GetWindowID(m_pWindow)) {
		return;
	}

	if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_CLOSE) {
		show(false);
	}
}

void Debugger::tick() {
	render();
}

void Debugger::render() {
	SDL_SetRenderDrawColor(m_pRenderer, 32, 32, 64, 255);
	SDL_RenderClear(m_pRenderer);
	m_pText->draw("Hello!\nHello MeinLEBEN", 10, 10);
	SDL_RenderPresent(m_pRenderer);
}