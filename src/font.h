#pragma once

class Font {
public:
	typedef int32_t Handle;
	typedef SDL_Rect Glyph;

	static const Handle kInvalidHandle;
	static const int32_t kNumCharacters = 217;

	inline const Glyph* get_glyphs() const {
		return m_glyphs;
	}

	inline SDL_Texture* get_atlas() const {
		return m_pAtlas;
	}

	inline const std::string& get_path() const {
		return m_path;
	}

	inline int32_t get_size() const {
		return m_size;
	}

	inline int32_t get_height() const {
		return m_height;
	}

	inline SDL_Color get_color() const {
		return m_color;
	}

private:
	Font(TTF_Font* pFont, const std::string& path, int32_t size, SDL_Color color, SDL_Renderer* pRenderer);
	~Font() {
		SDL_DestroyTexture(m_pAtlas);
	}

	Glyph m_glyphs[kNumCharacters] = {};
	SDL_Texture* m_pAtlas = nullptr;
	std::string m_path = {};
	int32_t m_size = 0;
	int32_t m_height = 0;
	SDL_Color m_color = {};
	Handle m_handle = kInvalidHandle;

	friend class FontManager;
};

class FontManager {
public:
	~FontManager() {
		for (auto iter : m_fonts) {
			delete iter.second;
		}
	}

	std::pair<bool, Font::Handle> add(SDL_Renderer* renderer, const std::string& path, int32_t size = 12, SDL_Color color = {255, 255, 255, 255});

	inline Font::Handle find(const std::string& path, int32_t size) const {
		for (auto iter : m_fonts) {
			if ((iter.second)->m_path == path &&
				(iter.second)->m_size == size) {
				return iter.first;
			}
		}

		return Font::kInvalidHandle;
	}

	inline const Font* get(Font::Handle handle) const {
		auto iter = m_fonts.find(handle);
		if (iter == m_fonts.end()) {
			return nullptr;
		}

		return iter->second;
	}

private:
	static const int32_t kFontSize = 16;

	Font::Handle m_lastHandle = Font::kInvalidHandle;
	std::unordered_map<Font::Handle, Font*> m_fonts;
};
