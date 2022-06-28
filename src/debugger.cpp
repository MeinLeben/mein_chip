#include "debugger.h"

class TextField {
public:
	bool initialize(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
	void set_text(const char* pText);
	void render(SDL_Renderer* pRenderer);
private:
	uint32_t m_x = 0, m_y = 0, m_w = 0, m_h = 0;

	void format_text();

	TTF_Font* m_pFont = nullptr;

	SDL_Texture* m_pTextTexture = nullptr;
	SDL_Rect m_textSize = {};
};

bool TextField::initialize(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	m_pFont = TTF_OpenFont("data/font/punk-mono/punk-mono-regular.ttf", 12);
	if (!m_pFont) {
		std::cerr << "[Debugger] Failed to open font: " << TTF_GetError() << std::endl;
	}

	SDL_Color color={0,0,0}, bgcolor={0xff,0xff,0xff};
	SDL_Surface* glyphCache[128-20];
	Uint16 ch;
	for(ch=20; ch<128; ++ch) {
		glyphCache[ch-20] = TTF_RenderGlyph_Shaded(m_pFont,ch,color,bgcolor);
	}

	return true;
}

void TextField::render(SDL_Renderer* pRenderer) {
	SDL_Surface* pTextSurface = TTF_RenderText_Shaded(m_pFont, "test text", {255, 255, 255, 255} , {0, 0, 0, 255});
	m_pTextTexture = SDL_CreateTextureFromSurface(pRenderer, pTextSurface);
	m_textSize.w = pTextSurface->w;
	m_textSize.h = pTextSurface->h;
	SDL_FreeSurface(pTextSurface);
	SDL_RenderCopy(pRenderer, m_pTextTexture, nullptr, &m_textSize);
}

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

	m_pTextField = new TextField;
	m_pTextField->initialize(0, 0, 128, 20);

	m_initialized = true;
	
	return true;
}

void Debugger::destroy() {
	if (!m_initialized) {
		return;
	}

	delete m_pTextField;

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
	SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);
	m_pTextField->render(m_pRenderer);
	SDL_RenderPresent(m_pRenderer);
}