#include "debugger.h"

const SDL_Color background = {31, 31, 31, 255};
const SDL_Color text_background = {46, 46, 46, 255};
const SDL_Color text_color = {206, 185, 114, 255};

Debugger::Debugger(SDL_Window* pParent) {
	int32_t x = SDL_WINDOWPOS_UNDEFINED;
	int32_t y = SDL_WINDOWPOS_UNDEFINED;

	if (pParent) {
		int32_t w, h;
		SDL_GetWindowPosition(pParent, &x, &y);
		SDL_GetWindowSize(pParent, &w, &h);

		x += w + 20;
	}

	m_window = std::make_unique<SDLWrapper::Window>("Debugger", x, y, 320, 321, 0);
	
	m_renderer = std::make_unique<SDLWrapper::Renderer>(m_window->get(), -1, SDL_RENDERER_ACCELERATED);
 
 	m_text_manager = std::make_unique<TextManager>();
	
	const std::string font_path = "data/font/punk-mono/punk-mono-regular.ttf";
	m_font = m_text_manager->add_font(m_renderer->get(), font_path, 12, text_color).second;
	if (m_font == Font::kInvalidHandle) {
		throw std::runtime_error("Failed to load font: " + font_path);
	}

	m_instruction = m_text_manager->create_text_label(10, 10, 278, m_font);
	m_instruction->set_background_color(text_background);
	m_instruction->use_background(true);
	m_instruction_description = m_text_manager->create_text_label(10, 10 + 16 + 5, 278, m_font);
	m_instruction_description->set_background_color(text_background);
	m_instruction_description->use_background(true);

	const int32_t offset_h = 10 + 16 + 5 + 16 + 5;
	m_gp_registers = m_text_manager->create_text_field(10, offset_h, 72, 256, m_font);
	m_gp_registers->set_background_color(text_background);
	m_gp_registers->use_background(true);
	m_stack = m_text_manager->create_text_field(10 + 72 + 10, offset_h, 108, 256, m_font);
	m_stack->set_background_color(text_background);
	m_stack->use_background(true);
	
	const int32_t label_offset_w = 10 + 72 + 10 + 108 + 10;
	const int32_t label_offset_h = 16 + 5;
	
	m_program_counter = m_text_manager->create_text_label(label_offset_w, offset_h, 78, m_font);
	m_program_counter->set_background_color(text_background);
	m_program_counter->use_background(true);
	m_index_register = m_text_manager->create_text_label(label_offset_w, offset_h + label_offset_h, 78, m_font);
	m_index_register->set_background_color(text_background);
	m_index_register->use_background(true);
	m_stack_pointer = m_text_manager->create_text_label(label_offset_w, offset_h + label_offset_h * 2, 78, m_font);
	m_stack_pointer->set_background_color(text_background);
	m_stack_pointer->use_background(true);
	m_delay_timer = m_text_manager->create_text_label(label_offset_w, offset_h + label_offset_h * 3, 78, m_font);
	m_delay_timer->set_background_color(text_background);
	m_delay_timer->use_background(true);
	m_sound_timer = m_text_manager->create_text_label(label_offset_w, offset_h + label_offset_h * 4, 78, m_font);
	m_sound_timer->set_background_color(text_background);
	m_sound_timer->use_background(true);
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
			<< std::setw(sizeof(uint16_t) * 2) 
			<< i << "] = " 
			<< "0x" << std::setfill('0') 
			<< std::setw(sizeof(char) * 4) 
			<< std::hex << uint32_t(stack[i]) << std::endl;
		str += ss.str();
	}

	str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
	m_stack->set_text(str);
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