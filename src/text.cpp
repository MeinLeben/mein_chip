#include "text.h"

TextBase::~TextBase() {}

void TextBase::draw(SDL_Renderer* renderer, const std::unique_ptr<FontManager>& font_manager) {
	const Font* font = font_manager->get(m_font);
	if (!font) {
		std::cerr << "Invalid font handle: " << m_font << std::endl;
		return;
	}

	if (m_use_background) {
		SDL_SetRenderDrawColor(renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
		SDL_RenderFillRect(renderer, &m_size);
	}

	if (m_use_background_outline) {
		SDL_SetRenderDrawColor(renderer, m_background_outline_color.r, m_background_outline_color.g, m_background_outline_color.b, m_background_outline_color.a);
		SDL_RenderDrawRect(renderer, &m_size);
	}
 
	const char* text = m_text.c_str();
	const Font::Glyph* pGlyphs = font->get_glyphs();
	SDL_Rect dst = {};
	int32_t x = m_size.x, y = m_size.y;
	char c = *text++;
	while (c) {
		if (c == '\n') {
			y += dst.h;
			x = m_size.x;
			c = *text++;
			continue;
		}

		if (c == '\t') {
			x += pGlyphs[' '].w * 4;
		}

		if (y >= m_size.y + m_size.h) {
			break;
		}

		if (x < m_size.x + m_size.w) {
			if (pGlyphs[c].x == 0 && pGlyphs[c].y == 0 && pGlyphs[c].w == 0 && pGlyphs[c].h == 0) {
				c = ' ';
			}

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

	m_size.h = font->get_height();
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

TextLabel* TextManager::create_text_label(int32_t x, int32_t y, int32_t w, Font::Handle font) {
	TextLabel* text_label = new TextLabel;
	text_label->m_size = {x, y, w, text_label->m_size.h};
	text_label->m_font = font;

	m_elements.insert(text_label);
	return text_label;
}

TextField* TextManager::create_text_field(int32_t x, int32_t y, int32_t w, int32_t h, Font::Handle font) {
	TextField* text_field = new TextField;
	text_field->m_size = {x, y, w, h};
	text_field->m_font = font;

	m_elements.insert(text_field);
	return text_field;
}

void TextManager::draw(SDL_Renderer* renderer) {
	for (auto element : m_elements) {
		element->draw(renderer, m_font_manager);
	}
}
