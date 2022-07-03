#pragma once

class Font;
class Text {
public:
	Text(Font* pDefaultFont)
		: m_pDefaultFont(pDefaultFont) {};

	void draw(const char* pText, int32_t x, int32_t y, Font* pFont = nullptr);

private:
	Font* m_pDefaultFont = nullptr;
};