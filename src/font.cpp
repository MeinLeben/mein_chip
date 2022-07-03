#include "font.h"

static const int32_t kFontSize = 16;
static const int32_t kFontAtlasSize = 512;

Font::Font(const char* pFontPath, SDL_Renderer* pRenderer)
	: Font(TTF_OpenFont(pFontPath, kFontSize), pRenderer) {
}

Font::Font(TTF_Font* pFont, SDL_Renderer* pRenderer)
	: m_pRenderer(pRenderer) {
	if (!pFont) {
		std::runtime_error("No valid TTF font.");
	}

	SDL_Surface* pAtlasSurface = SDL_CreateRGBSurface(0, kFontAtlasSize, kFontAtlasSize, 32, 0, 0, 0, 255);
	SDL_SetColorKey(pAtlasSurface, SDL_TRUE, SDL_MapRGBA(pAtlasSurface->format, 0, 0, 0, 0));

	SDL_Rect dst = {};
	SDL_Surface* pSurface = nullptr;
	const char start = ' '; const char end = 'z';
	for (char c = start; c <= end;c++) {
		char text[2] = {c, 0};

		pSurface = TTF_RenderUTF8_Blended(pFont, text, {255, 255, 255, 255});

		TTF_SizeText(pFont, text, &dst.w, &dst.h);

		if (dst.x + dst.w >= kFontAtlasSize) {
			dst.x = 0;
			dst.y += dst.h + 1;
			if (dst.y + dst.h >= kFontAtlasSize) {
				SDL_FreeSurface(pSurface);
				std::runtime_error("Failed to generate font texture atlas");
			}
		}

		SDL_BlitSurface(pSurface, nullptr, pAtlasSurface, &dst);
		SDL_FreeSurface(pSurface);

		m_glyphs[c] = dst;

		dst.x += dst.w;
	}

	m_pAtlas = SDL_CreateTextureFromSurface(pRenderer, pAtlasSurface);

	SDL_FreeSurface(pAtlasSurface);
}

Font::~Font() {
	SDL_DestroyTexture(m_pAtlas);
}
