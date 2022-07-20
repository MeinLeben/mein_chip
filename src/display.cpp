#include "display.h"

Display::Display(int32_t x, int32_t y, uint32_t scale, uint32_t width, uint32_t height) :
	m_x(x),
	m_y(y),
	m_width(width),
	m_height(height),
	m_scale(scale) {

	assert(m_width > 0 && m_height > 0);
	m_pGrid = new uint32_t[m_width * m_height];
	clear();
}

Display::~Display() {
	delete[] m_pGrid;
}

void Display::update_mouse_position(int32_t x, int32_t y) {
	if (x == m_mouse_x && y == m_mouse_y) {
		return;
	}
	m_mouse_x = x; m_mouse_y = y;
	SDL_Rect rect = { m_x, m_y, (int32_t)(m_width * m_scale), (int32_t)(m_height * m_scale) };
	if (m_mouse_x >= rect.x && m_mouse_y >= rect.y &&
		m_mouse_x < rect.x + rect.w && m_mouse_y < rect.y + rect.h) {
		int32_t grid_mouse_x = (m_mouse_x - rect.x) / m_scale;
		int32_t grid_mouse_y = (m_mouse_y - rect.y) / m_scale;
		std::cout << "grid mouse position: "  << std::hex << std::setfill('0') << std::uppercase << std::setw(2) <<  grid_mouse_x << 
			", " << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << grid_mouse_y << std::endl;
	}
}

void Display::clear() {
	memset(m_pGrid, 0x0, m_width * m_height * sizeof(uint32_t));
}

bool Display::plot_pixel(uint32_t x, uint32_t y) {
	if (x > m_width || y > m_height) {
		return false;
	}

	m_pGrid[y * m_width + x] ^= 1;
	return m_pGrid[y * m_width + x] == 1;
}

void Display::draw(SDL_Renderer* pRenderer) {
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

	SDL_Rect rect = { m_x, m_y, (int32_t)(m_width * m_scale), (int32_t)(m_height * m_scale) };
	SDL_RenderFillRect(pRenderer, &rect);

	for (uint32_t x = 0; x < m_width; x++) {
		for (uint32_t y = 0; y < m_height; y++) {
			if (m_pGrid[y * m_width + x] == 1) {
				SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
			}
			else {
				SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
			}

			rect = { int32_t(m_x + x * m_scale), int32_t(m_y + y * m_scale), (int32_t)m_scale, (int32_t)m_scale };
			SDL_RenderFillRect(pRenderer, &rect);
		}
	}
}
