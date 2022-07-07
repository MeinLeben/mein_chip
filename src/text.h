#pragma once

#include "font.h"

class Text {
public:
	Text(SDL_Renderer* pRenderer);
	~Text();

	inline std::pair<bool, Font::Handle> add_font(const std::string& font_path) {
		return m_pFontManager->add(font_path);
	}

	void draw(const char* pText, int32_t x, int32_t y, Font::Handle font);

private:
	FontManager* m_pFontManager = nullptr;
};