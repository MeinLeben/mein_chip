#include "text.h"

TextBase::~TextBase() {}

void TextBase::draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager) {
	const Font* font = font_manager->get(m_font);
	if (!font) {
		std::cerr << "Invalid font handle: " << m_font << std::endl;
		return;
	}

	SDL_Rect background = {m_x, m_y, m_w, m_h};
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &background);
 
	const char* text = m_text.c_str();
	const Font::Glyph* pGlyphs = font->get_glyphs();
	SDL_Rect dst = {};
	int32_t x = m_x, y = m_y;
	char c = *text++;
	while (c) {
		if (c == '\n') {
			y += dst.h;
			x = m_x;
		}

		if (c == '\t') {
			x += pGlyphs[' '].w * 4;
		}

		if (y >= m_y + m_h) {
			break;
		}

		if (x < m_x + m_w) {
			dst.x = x;
			dst.y = y;
			dst.w = pGlyphs[c].w;
			dst.h = pGlyphs[c].h;
	
			SDL_RenderCopy(renderer, font->get_atlas(), &pGlyphs[c], &dst);
		}
		x += dst.w;
		c = *text++;
	}
}

void TextLabel::draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager) {
	const Font* font = font_manager->get(m_font);
	if (!font) {
		std::cerr << "Invalid font handle: " << m_font << std::endl;
		return;
	}

	m_h = font->get_size();
	TextBase::draw(renderer, font_manager);
}

TextManager::TextManager() {
	m_font_manager = std::make_unique<FontManager>();
}

TextManager::~TextManager() {
	for (auto element : m_elements) {
		delete element;
	}
}

TextLabel* TextManager::create_text_label(const std::string& text, int32_t x, int32_t y, int32_t w, Font::Handle font) {
	TextLabel* text_label = new TextLabel;
	text_label->m_text = text;
	text_label->m_x = x;
	text_label->m_y = y;
	text_label->m_w = w;
	text_label->m_font = font;

	m_elements.insert(text_label);
	return text_label;
}

TextField* TextManager::create_text_field(const std::string& text, int32_t x, int32_t y, int32_t w, int32_t h, Font::Handle font) {
	TextField* text_field = new TextField;
	text_field->m_text = text;
	text_field->m_x = x;
	text_field->m_y = y;
	text_field->m_w = w;
	text_field->m_h = h;
	text_field->m_font = font;

	m_elements.insert(text_field);
	return text_field;
}

void TextManager::draw(SDL_Renderer* renderer) {
	for (auto element : m_elements) {
		element->draw(renderer, m_font_manager);
	}
}
