#include "font.h"

const Font::Handle Font::kInvalidHandle = -1;

static const int32_t kFontAtlasSize = 512;

Font::Font(TTF_Font* pFont, const std::string& path, SDL_Renderer* pRenderer) 
	: m_path(path) {
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

FontManager::~FontManager() {
	for (auto iter : m_fonts) {
		delete iter.second;
	}
}

std::pair<bool, Font::Handle> FontManager::add(const std::string& font_path) {
	Font::Handle handle = find(font_path);
	if (handle != Font::kInvalidHandle) {
		return {false, handle}; 
	}

	/* CFR TODO removal of kFontSize:
	 * Still using a fixed font size here, font size should become variable instead.
	 * Problems that need to be resolved are:
	 * Fixed atlas size:
	 * 	The current fixed atlas size which might become to small.
	 * 	This should be solvable by calculating the atlas size since we know the glyh size and count.
	 * Unique key:
	 * 	Currently we use the font path as the key identifier.
	 * 	Since we could now have multiple fonts with the same font path.
	 */

	Font* pFont = new Font(TTF_OpenFont(font_path.c_str(), kFontSize), font_path, m_pRenderer);
	m_fonts.insert({++m_lastHandle, pFont});

	return {true, m_lastHandle};
}
