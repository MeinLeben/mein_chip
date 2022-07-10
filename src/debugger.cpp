#include "debugger.h"

const SDL_Color background = {31, 31, 31, 255};
const SDL_Color text_background = {46, 46, 46, 255};
const SDL_Color text_color = {206, 185, 114, 255};
const SDL_Color error_text_color = {165, 93, 89, 255};

Debugger::Debugger(SDL_Window* pParent) {
	int32_t x = SDL_WINDOWPOS_UNDEFINED;
	int32_t y = SDL_WINDOWPOS_UNDEFINED;

	if (pParent) {
		int32_t w, h;
		SDL_GetWindowPosition(pParent, &x, &y);
		SDL_GetWindowSize(pParent, &w, &h);

		x += w + 20;
	}

	m_window = std::make_unique<SDLWrapper::Window>("Debugger", x, y, 1, 1, 0);
	
	m_renderer = std::make_unique<SDLWrapper::Renderer>(m_window->get(), -1, SDL_RENDERER_ACCELERATED);
 
 	m_text_manager = std::make_unique<TextManager>();
	
	const std::string font_path = "data/font/cascadia/cascadiacode.ttf";
	m_default_font = m_text_manager->add_font(m_renderer->get(), font_path, 14, text_color).second;
	if (m_default_font == Font::kInvalidHandle) {
		throw std::runtime_error("Failed to load font: " + font_path);
	}

	m_error_font = m_text_manager->add_font(m_renderer->get(), font_path, 14, error_text_color).second;
	if (m_default_font == Font::kInvalidHandle) {
		throw std::runtime_error("Failed to load font: " + font_path);
	}

	const uint32_t default_font_height = m_text_manager->get_font(m_default_font)->get_height();
	const uint32_t default_error_font_height = m_text_manager->get_font(m_error_font)->get_height();

	const uint32_t border = 10;
	int32_t offset_y = border;
	m_instruction = m_text_manager->create_text_label(offset_y, border, 1, m_default_font);
	m_instruction->set_background_color(text_background);
	m_instruction->use_background(true);

	offset_y += default_font_height + border;
	m_instruction_description = m_text_manager->create_text_label(border, offset_y, 1, m_default_font);
	m_instruction_description->set_background_color(text_background);
	m_instruction_description->use_background(true);

	offset_y += default_font_height + border;
	int32_t label_offset_y = offset_y;
	int32_t offset_x = border;
	const int32_t gp_registers_h = (1 + 16) * default_font_height;
	const int32_t gp_registers_w = 96;
	m_gp_registers = m_text_manager->create_text_field(offset_x, offset_y, gp_registers_w, gp_registers_h, m_default_font);
	m_gp_registers->set_background_color(text_background);
	m_gp_registers->use_background(true);

	offset_x += gp_registers_w + border;
	const int32_t stack_h = (1 + 16) * default_font_height;
	const int32_t stack_w = 144;
	m_stack = m_text_manager->create_text_field(offset_x, offset_y, stack_w, stack_h, m_default_font);
	m_stack->set_background_color(text_background);
	m_stack->use_background(true);

	offset_y += gp_registers_h + border;
	const int32_t error_h = default_error_font_height * 8;
	m_error = m_text_manager->create_text_field(border, offset_y, 1, error_h, m_error_font);
	m_error->set_background_color(text_background);
	m_error->use_background(true);

	offset_x += stack_w + border;
	const int32_t label_w = 104;
	m_program_counter = m_text_manager->create_text_label(offset_x, label_offset_y, label_w, m_default_font);
	m_program_counter->set_background_color(text_background);
	m_program_counter->use_background(true);

	label_offset_y += default_font_height + border;
	m_index_register = m_text_manager->create_text_label(offset_x, label_offset_y, label_w , m_default_font);
	m_index_register->set_background_color(text_background);
	m_index_register->use_background(true);

	label_offset_y += default_font_height + border;
	m_stack_pointer = m_text_manager->create_text_label(offset_x, label_offset_y, label_w, m_default_font);
	m_stack_pointer->set_background_color(text_background);
	m_stack_pointer->use_background(true);

	label_offset_y += default_font_height + border;
	m_delay_timer = m_text_manager->create_text_label(offset_x, label_offset_y, label_w, m_default_font);
	m_delay_timer->set_background_color(text_background);
	m_delay_timer->use_background(true);

	label_offset_y += default_font_height + border;
	m_sound_timer = m_text_manager->create_text_label(offset_x, label_offset_y, label_w, m_default_font);
	m_sound_timer->set_background_color(text_background);
	m_sound_timer->use_background(true);

	const int32_t max_text_length_w = offset_x + label_w - border;
	m_instruction->set_width(max_text_length_w);
	m_instruction_description->set_width(max_text_length_w);
	m_error->set_width(max_text_length_w);

	const int32_t window_w = offset_x + label_w + border;
	const int32_t window_h = offset_y + error_h + border;
	
	SDL_SetWindowSize(m_window->get(), window_w, window_h);
}

void Debugger::handle_event(SDL_Event* pEvent) {
	if (!pEvent || pEvent->window.windowID != SDL_GetWindowID(m_window->get())) {
		return;
	}

	if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_CLOSE) {
		show(false);
	}
}
	
void Debugger::update_instruction(uint16_t instruction) {
	std::stringstream ss;
	ss << "0x" << std::hex << std::setw(sizeof(uint16_t) * 2) 
		<< std::setfill('0') << std::uppercase 
		<< instruction << std::endl;

	m_instruction->set_text("instruction: " + ss.str());
}

void Debugger::update_instruction_description(const std::string& description) {
	m_instruction_description->set_text("description: " + description);
}

void Debugger::update_gp_registers(uint8_t* v, size_t size) {
	std::string str = "[GP REG]:\n";
	for (size_t i = 0; i < size; i++) {
		std::stringstream ss;
		ss << "v[" << std::setfill('0') 
			<< std::setw(sizeof(char) * 2) 
			<< i << "] = " 
			<< "0x" << std::setfill('0') 
			<< std::setw(sizeof(char) * 2) 
			<< std::hex << uint32_t(v[i]) << std::endl;
		str += ss.str();
	}

	str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
	m_gp_registers->set_text(str);
}

void Debugger::update_index_register(uint16_t i) {
	std::stringstream ss;
	ss << "index: " << "0x" << std::setfill('0') 
		<< std::setw(sizeof(uint16_t) * 2) 
		<< std::hex << uint32_t(i) << std::endl;
	m_index_register->set_text(ss.str());
}

void Debugger::update_delay_timer(uint8_t dt) {
	std::stringstream ss;
	ss << "dt:    " << "  0x" << std::setfill('0') 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(dt) << std::endl;
	m_delay_timer->set_text(ss.str());
}

void Debugger::update_sound_timer(uint8_t st) {
	std::stringstream ss;
	ss << "st:    " << "  0x" << std::setfill('0') 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(st) << std::endl;
	m_sound_timer->set_text(ss.str());
}

void Debugger::update_program_counter(uint16_t pc) {
	std::stringstream ss;
	ss << "pc:    " << "0x" << std::setfill('0') 
		<< std::setw(sizeof(uint16_t) * 2) 
		<< std::hex << uint32_t(pc) << std::endl;
	m_program_counter->set_text(ss.str());
}

void Debugger::update_stack_pointer(uint8_t sp) {
	std::stringstream ss;
	ss << "sp:    " << "  0x" << std::setfill('0') 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(sp) << std::endl;
	m_stack_pointer->set_text(ss.str());
}

void Debugger::update_stack(uint16_t* stack, size_t size) {
	std::string str = "[STACK]:\n";
	for (size_t i = 0; i < size; i++) {
		std::stringstream ss;
		ss << "stack[" << std::setfill('0') 
			<< std::setw(sizeof(uint8_t) * 2) 
			<< i << "] = " 
			<< "0x" << std::setfill('0') 
			<< std::setw(sizeof(uint16_t) * 2) 
			<< std::hex << uint32_t(stack[i]) << std::endl;
		str += ss.str();
	}

	str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
	m_stack->set_text(str);
}

void Debugger::update_error(const std::string& message) {
	m_error_text = message + "\n" + m_error_text;
	m_error->set_text(m_error_text);
}

void Debugger::tick() {
	render();
}

void Debugger::render() {
	SDL_SetRenderDrawColor(m_renderer->get(), background.r, background.g, background.b, background.a);
	SDL_RenderClear(m_renderer->get());
	m_text_manager->draw(m_renderer->get());
	SDL_RenderPresent(m_renderer->get());
}