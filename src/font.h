#pragma once

class Font {
public:
	static const int32_t kNumCharacters = 217;

	typedef SDL_Rect Glyph;

	Font(const char* pFontPath, SDL_Renderer* pRenderer);
	Font(TTF_Font* pFont, SDL_Renderer* pRenderer);
	~Font();

	inline SDL_Renderer* GetRenderer() const {
		return m_pRenderer;
	}

	inline const Glyph* GetGlyphs() const {
		return m_glyphs;
	}

	inline SDL_Texture* GetAtlas() const {
		return m_pAtlas;
	}

private:
	SDL_Renderer* m_pRenderer = nullptr;
	Glyph m_glyphs[kNumCharacters] = {};
	SDL_Texture* m_pAtlas = nullptr;
};
