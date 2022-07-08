#pragma once

#include "font.h"

class TextBase {
public:
	inline void set_x(int32_t x) {
		m_x = x;
	}

	inline void set_y(int32_t y) {
		m_y = y;
	}
	void set_text(const std::string& text) {
		m_text = text;
	}

	void set_font(Font::Handle font) {
		m_font = font;
	}

protected:
	TextBase() = default;
	virtual ~TextBase() = 0;

	virtual void draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager);
	
	int32_t m_x = 0;
	int32_t m_y = 0;
	int32_t m_w = 64;
	int32_t m_h = 64;

	std::string m_text = {};
	Font::Handle m_font = Font::kInvalidHandle;

	friend class TextManager;
};

class TextLabel : public TextBase {
public:
	void set_width(int32_t w) {
		m_w = w;
	}

protected:
	TextLabel() = default;
	~TextLabel() = default;

	virtual void draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager) override;

	friend class TextManager;
};

class TextField : public TextBase {
public:
	void set_width(int32_t w) {
		m_w = w;
	}

	void set_height(int32_t h) {
		m_h = h;
	}

protected:
	TextField() = default;
	~TextField() = default;

	friend class TextManager;
};

class TextManager {
public:
	TextManager();
	~TextManager();

	inline std::pair<bool, Font::Handle> add_font(SDL_Renderer* renderer, const std::string& font_path, int32_t font_size) {
		return m_font_manager->add(renderer, font_path, font_size);
	}

	TextLabel* create_text_label(const std::string& text, int32_t x, int32_t y, int32_t w, Font::Handle font);
	TextField* create_text_field(const std::string& text, int32_t x, int32_t y, int32_t w, int32_t h, Font::Handle font);

	void draw(SDL_Renderer* renderer);

private:
	std::unique_ptr<FontManager> m_font_manager;

	std::unordered_set<TextBase*> m_elements;
};