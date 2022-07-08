#include "font.h"

const Font::Handle Font::kInvalidHandle = -1;

Font::Font(TTF_Font* pFont, const std::string& path, int32_t size, SDL_Renderer* pRenderer) 
	: m_path(path)
	, m_size(size) {

	const int32_t height = TTF_FontHeight(pFont);
	const int32_t font_atlas_size = height * (int32_t)sqrt((float)kNumCharacters);
	SDL_Surface* pAtlasSurface = SDL_CreateRGBSurface(0, font_atlas_size, font_atlas_size, 32, 0, 0, 0, 255);
	SDL_SetColorKey(pAtlasSurface, SDL_TRUE, SDL_MapRGBA(pAtlasSurface->format, 0, 0, 0, 0));

	SDL_Rect dst = {};
	SDL_Surface* pSurface = nullptr;
	const char start = ' '; const char end = 'z';
	for (char c = start; c <= end;c++) {
		char text[2] = {c, 0};

		pSurface = TTF_RenderUTF8_Blended(pFont, text, {255, 255, 255, 255});

		TTF_SizeText(pFont, text, &dst.w, &dst.h);

		if (dst.x + dst.w >= font_atlas_size) {
			dst.x = 0;
			dst.y += dst.h + 1;
			if (dst.y + dst.h >= font_atlas_size) {
				SDL_FreeSurface(pSurface);
				throw std::runtime_error("Failed to generate font texture atlas");
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

std::pair<bool, Font::Handle> FontManager::add(SDL_Renderer* renderer, const std::string& font_path, int32_t font_size) {
	Font::Handle handle = find(font_path, font_size);
	if (handle != Font::kInvalidHandle) {
		return {false, handle}; 
	}

	TTF_Font* pTTF_Font = TTF_OpenFont(font_path.c_str(), font_size);
	if (!pTTF_Font) {
		return {false, Font::kInvalidHandle};
	}

	Font* pFont = new Font(pTTF_Font, font_path, font_size, renderer);
	
	TTF_CloseFont(pTTF_Font);

	m_fonts.insert({++m_lastHandle, pFont});

	return {true, m_lastHandle};
}
