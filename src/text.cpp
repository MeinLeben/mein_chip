#include "text.h"

#include "font.h"

void Text::draw(const char* pText, int32_t x, int32_t y, Font* pFont) {
	if (!pFont) {
		pFont = m_pDefaultFont;
	}

	const Font::Glyph* pGlyphs = pFont->GetGlyphs();
	SDL_Rect dst = {};
	int32_t x_start = x;
	char c = *pText++;
	while (c) {
		if (c == '\n') {
			y += dst.h;
			x = x_start;
		}

		if (c == '\t') {
			x += pGlyphs[' '].w * 4;
		}

		dst.x = x;
		dst.y = y;
		dst.w = pGlyphs[c].w;
		dst.h = pGlyphs[c].h;

		SDL_RenderCopy(pFont->GetRenderer(), pFont->GetAtlas(), &pGlyphs[c], &dst);
		x += dst.w;
		c = *pText++;
	}
}