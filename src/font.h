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

private:
	Font(TTF_Font* pFont, const std::string& path, SDL_Renderer* pRenderer);
	~Font();

	Glyph m_glyphs[kNumCharacters] = {};
	SDL_Texture* m_pAtlas = nullptr;
	std::string m_path = {};
	Handle m_handle = kInvalidHandle;

	friend class FontManager;
};

class FontManager {
public:
	FontManager(SDL_Renderer* pRenderer)
		: m_pRenderer(pRenderer) {
	}

	~FontManager();

	std::pair<bool, Font::Handle> add(const std::string& font_path);

	inline Font::Handle find(const std::string& font_path) const {
		for (auto iter : m_fonts) {
			if ((iter.second)->m_path == font_path) {
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

	inline SDL_Renderer* get_renderer() const {
		return m_pRenderer;
	}

private:
	static const int32_t kFontSize = 16;


	Font::Handle m_lastHandle = Font::kInvalidHandle;
	SDL_Renderer* m_pRenderer = nullptr;

	std::unordered_map<Font::Handle, Font*> m_fonts;
};
