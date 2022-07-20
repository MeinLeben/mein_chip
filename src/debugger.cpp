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
	m_instructions = m_text_manager->create_text_field(border, offset_y, 1, default_font_height * 2, m_default_font);
	m_instructions->set_background_color(text_background);
	m_instructions->use_background(true);

	offset_y += (default_font_height * 2) + border;
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

	offset_y += error_h + border;
	m_mouse_position = m_text_manager->create_text_label(border, offset_y, 1, m_default_font);
	m_mouse_position->set_background_color(text_background);
	m_mouse_position->use_background(true);

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

	label_offset_y += default_font_height + border;
	const int32_t keypad_w = 96;
	const int32_t keypad_h = 4 * default_font_height;
	m_keypad[0] = m_text_manager->create_text_field(offset_x, label_offset_y, keypad_w, keypad_h, m_default_font);
	m_keypad[0]->set_background_color(text_background);
	m_keypad[0]->use_background(true);

	label_offset_y += keypad_h + border;
	m_keypad[1] = m_text_manager->create_text_field(offset_x, label_offset_y, keypad_w, keypad_h, m_default_font);
	m_keypad[1]->set_background_color(text_background);
	m_keypad[1]->use_background(true);

	const int32_t max_text_length_w = offset_x + label_w - border;
	m_instructions->set_width(max_text_length_w);
	m_error->set_width(max_text_length_w);
	m_mouse_position->set_width(max_text_length_w);

	const int32_t window_w = offset_x + label_w + border;
	const int32_t window_h = offset_y + default_font_height + border;
	
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
	
void Debugger::update_instruction(uint16_t instruction, const std::string& description) {
	std::stringstream ss;
	ss << "0x" << std::hex << std::setw(sizeof(uint16_t) * 2) 
		<< std::setfill('0') << std::uppercase 
		<< instruction << std::endl;

	std::string str = "INSTRUCTION " + ss.str();
	str.erase(std::find(str.begin(), str.end(), '\n'), str.end());
	str += " : " + description;

	m_instruction_list[1] = m_instruction_list[0];
	m_instruction_list[0] = str;

	m_instructions->set_text(m_instruction_list[0] + "\n" + m_instruction_list[1]);
}

void Debugger::update_gp_registers(uint8_t* v, size_t size) {
	std::string str = "GP REGISTERS\n";
	for (size_t i = 0; i < size; i++) {
		std::stringstream ss;
		ss << "v[" << std::uppercase 
			<< std::hex << i << "] = " 
			<< "0x" << std::setfill('0') << std::uppercase 
			<< std::setw(sizeof(char) * 2) 
			<< std::hex << uint32_t(v[i]) << std::endl;
		str += ss.str();
	}

	str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
	m_gp_registers->set_text(str);
}

void Debugger::update_index_register(uint16_t i) {
	std::stringstream ss;
	ss << "I      " << "0x" << std::setfill('0') << std::uppercase 
		<< std::setw(sizeof(uint16_t) * 2) 
		<< std::hex << uint32_t(i) << std::endl;
	m_index_register->set_text(ss.str());
}

void Debugger::update_delay_timer(uint8_t dt) {
	std::stringstream ss;
	ss << "DT     " << "  0x" << std::setfill('0') << std::uppercase 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(dt) << std::endl;
	m_delay_timer->set_text(ss.str());
}

void Debugger::update_sound_timer(uint8_t st) {
	std::stringstream ss;
	ss << "ST     " << "  0x" << std::setfill('0') 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(st) << std::endl;
	m_sound_timer->set_text(ss.str());
}

void Debugger::update_program_counter(uint16_t pc) {
	std::stringstream ss;
	ss << "PC     " << "0x" << std::setfill('0') << std::uppercase 
		<< std::setw(sizeof(uint16_t) * 2) 
		<< std::hex << uint32_t(pc) << std::endl;
	m_program_counter->set_text(ss.str());
}

void Debugger::update_stack_pointer(uint8_t sp) {
	std::stringstream ss;
	ss << "SP     " << "  0x" << std::setfill('0') << std::uppercase 
		<< std::setw(sizeof(uint8_t) * 2) 
		<< std::hex << uint32_t(sp) << std::endl;
	m_stack_pointer->set_text(ss.str());
}

void Debugger::update_stack(uint16_t* stack, size_t size) {
	std::string str = "STACK\n";
	for (size_t i = 0; i < size; i++) {
		std::stringstream ss;
		ss << "stack[" << std::setfill('0') 
			<< std::setw(sizeof(uint8_t) * 2) 
			<< i << "] = " 
			<< "0x" << std::setfill('0') << std::uppercase
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

void Debugger::update_keypad(bool* keys_state, size_t num_keys) {
	const std::string keys[2][16] = {
		{"[1]", "[2]", "[3]", "[C]", "[4]", "[5]", "[6]", "[D]", "[7]", "[8]", "[9]", "[E]", "[A]", "[0]", "[B]", "[F]"},
		{"[1]", "[2]", "[3]", "[4]", "[Q]", "[W]", "[E]", "[R]", "[A]", "[S]", "[D]", "[F]", "[Z]", "[X]", "[C]", "[V]"}
	};

	const int32_t keys_state_lookup[16] = {1, 2, 3, 12, 4, 5, 6, 13, 7, 8, 9, 14, 10, 0, 11, 15};

	std::string key_pad[2] = {};
	for (int32_t i = 0; i < num_keys; i++) {
		if (i % 4 == 0 && i != 0) {
			key_pad[0] += "\n";
			key_pad[1] += "\n";
		}

		if (keys_state[keys_state_lookup[i]]) {
			key_pad[0] += "[#]";
			key_pad[1] += "[#]";
		}
		else {
			key_pad[0] += keys[0][i];
			key_pad[1] += keys[1][i];
		}
	}

	m_keypad[0]->set_text(key_pad[0]);
	m_keypad[1]->set_text(key_pad[1]);
}

void Debugger::update_mouse_position(int32_t x, int32_t y, uint32_t display_x, uint32_t display_y, uint32_t display_w, uint32_t display_h, uint32_t display_pixel_scale) {
	if (x >= display_x && y >= display_y &&
		x < display_x + (display_w * display_pixel_scale) && y < display_y + (display_h * display_pixel_scale)) {
		int32_t grid_mouse_x = (x - display_x) / display_pixel_scale;
		int32_t grid_mouse_y = (y - display_y) / display_pixel_scale;
		std::stringstream ss;
		ss << "Mouse position: "  << std::hex << std::setfill('0') << std::uppercase << std::setw(2) <<  grid_mouse_x << 
			", " << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << grid_mouse_y << std::endl;
		
		m_mouse_position->set_text(ss.str());
	}
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