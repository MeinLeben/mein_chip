#pragma once

class Display
{
public:
	Display(int32_t x, int32_t y, uint32_t pixel_scale, uint32_t pixel_width = 64, uint32_t pixel_height = 32);
	~Display();

	void clear();
	bool plot_pixel(uint32_t x, uint32_t y);

	void draw(SDL_Renderer* pRenderer);

private:
	uint32_t* m_pGrid = nullptr;
	int32_t m_x = 0, m_y = 0;
	uint32_t m_width = 0, m_height = 0;
	uint32_t m_scale = 0;
};

