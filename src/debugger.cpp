#include "debugger.h"

Debugger::Debugger(SDL_Window* pParent) {
	int32_t x = SDL_WINDOWPOS_UNDEFINED;
	int32_t y = SDL_WINDOWPOS_UNDEFINED;

	if (pParent) {
		int32_t w, h;
		SDL_GetWindowPosition(pParent, &x, &y);
		SDL_GetWindowSize(pParent, &w, &h);

		x += w + 20;
	}

	m_window = std::make_unique<SDLWrapper::Window>("Debugger", x, y, 240, 320, 0);
	
	m_renderer = std::make_unique<SDLWrapper::Renderer>(m_window->get(), -1, SDL_RENDERER_ACCELERATED);
 
 	m_text_manager = std::make_unique<TextManager>();
	
	const std::string font_path = "data/font/punk-mono/punk-mono-regular.ttf";
	m_font = m_text_manager->add_font(m_renderer->get(), font_path, 16).second;
	if (m_font == Font::kInvalidHandle) {
		throw std::runtime_error("Failed to load font: " + font_path);
	}

	m_text_label = m_text_manager->create_text_label("test label\n new line test", 30, 30, 256, m_font);
	m_text_field = m_text_manager->create_text_field("second test\n\n\nWHooah!\nZzZzzz label", 30, 46, 32, 128, m_font);
}

void Debugger::handle_event(SDL_Event* pEvent) {
	if (!pEvent || pEvent->window.windowID != SDL_GetWindowID(m_window->get())) {
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
	SDL_SetRenderDrawColor(m_renderer->get(), 32, 32, 64, 255);
	SDL_RenderClear(m_renderer->get());
	m_text_manager->draw(m_renderer->get());
	SDL_RenderPresent(m_renderer->get());
}