#pragma once

#include "font.h"

class TextBase {
public:
	inline void set_x(int32_t x) {
		m_size.x = x;
	}

	inline void set_y(int32_t y) {
		m_size.y = y;
	}
	void set_text(const std::string& text) {
		m_text = text;
	}

	void set_font(Font::Handle font) {
		m_font = font;
	}

	void set_background_color(SDL_Color color) {
		m_background_color = color;
	}

	void set_background_outline_color(SDL_Color color) {
		m_background_outline_color = color;
	}

	void use_background(bool use) {
		m_use_background = use;
	}

	void use_background_outline(bool use) {
		m_use_background_outline = use;
	}

protected:
	TextBase() = default;
	virtual ~TextBase() = 0;

	virtual void draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager);
	
	std::string m_text = {};
	Font::Handle m_font = Font::kInvalidHandle;
	SDL_Rect m_size = {0, 0, 64, 64};
	SDL_Color m_background_color = {};
	SDL_Color m_background_outline_color = {};
	bool m_use_background = false;
	bool m_use_background_outline = false;

	friend class TextManager;
};

class TextLabel : public TextBase {
public:
	void set_width(int32_t w) {
		m_size.w = w;
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
		m_size.w = w;
	}

	void set_height(int32_t h) {
		m_size.h = h;
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

	inline std::pair<bool, Font::Handle> add_font(SDL_Renderer* renderer, const std::string& path, int32_t size, SDL_Color color = {255, 255, 255, 255}) {
		return m_font_manager->add(renderer, path, size, color);
	}

	TextLabel* create_text_label(int32_t x, int32_t y, int32_t w, Font::Handle font);
	TextField* create_text_field(int32_t x, int32_t y, int32_t w, int32_t h, Font::Handle font);

	void draw(SDL_Renderer* renderer);

private:
	std::unique_ptr<FontManager> m_font_manager;
	std::unordered_set<TextBase*> m_elements;
};