#include "text.h"

Text::Text(SDL_Renderer* pRenderer) {
	m_pFontManager = new FontManager(pRenderer);
}

Text::~Text() {
	delete m_pFontManager;
}

void Text::draw(const char* pText, int32_t x, int32_t y, Font::Handle font_handle ) {
	const Font* pFont = m_pFontManager->get(font_handle);
	if (!pFont) {
		std::cerr << "Invalid font handle: " << font_handle << std::endl;
		return;
	}

	const Font::Glyph* pGlyphs = pFont->get_glyphs();
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

		SDL_RenderCopy(m_pFontManager->get_renderer(), pFont->get_atlas(), &pGlyphs[c], &dst);
		x += dst.w;
		c = *pText++;
	}
}
