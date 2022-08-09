#include "input.h"

#include "debugger.h"

Input::Input(wxWindow* pWindow) {
	m_keys[0x0].keycodes[0] = '0'; m_keys[0x0].keycodes[1] = 'X';
	m_keys[0x1].keycodes[0] = '1'; m_keys[0x1].keycodes[1] = '1';
	m_keys[0x2].keycodes[0] = '2'; m_keys[0x2].keycodes[1] = '2';
	m_keys[0x3].keycodes[0] = '3'; m_keys[0x3].keycodes[1] = '3';
	m_keys[0x4].keycodes[0] = '4'; m_keys[0x4].keycodes[1] = 'Q';
	m_keys[0x5].keycodes[0] = '5'; m_keys[0x5].keycodes[1] = 'W';
	m_keys[0x6].keycodes[0] = '6'; m_keys[0x6].keycodes[1] = 'E';
	m_keys[0x7].keycodes[0] = '7'; m_keys[0x7].keycodes[1] = 'A';
	m_keys[0x8].keycodes[0] = '8'; m_keys[0x8].keycodes[1] = 'S';
	m_keys[0x9].keycodes[0] = '9'; m_keys[0x9].keycodes[1] = 'D';
	m_keys[0xA].keycodes[0] = 'A'; m_keys[0xA].keycodes[1] = 'Z';
	m_keys[0xB].keycodes[0] = 'B'; m_keys[0xB].keycodes[1] = 'C';
	m_keys[0xC].keycodes[0] = 'C'; m_keys[0xC].keycodes[1] = '4';
	m_keys[0xD].keycodes[0] = 'D'; m_keys[0xD].keycodes[1] = 'R';
	m_keys[0xE].keycodes[0] = 'E'; m_keys[0xE].keycodes[1] = 'F';
	m_keys[0xF].keycodes[0] = 'F'; m_keys[0xF].keycodes[1] = 'V';

	pWindow->Bind(wxEVT_KEY_UP, &Input::OnKeyUp, this);
	pWindow->Bind(wxEVT_KEY_DOWN, &Input::OnKeyDown, this);
}

void Input::OnKeyUp(wxKeyEvent& event) {
	for (int32_t i = 0; i < kNumberOfKeys; i++) {
		if (m_keys[i].keycodes[m_use_virtual_keypad] == event.GetKeyCode()) {
			m_keys[i].isPressed = false;
			break;
		}
	}

	event.Skip();
}

void Input::OnKeyDown(wxKeyEvent& event) {
	for (int32_t i = 0; i < kNumberOfKeys; i++) {
		if (m_keys[i].keycodes[m_use_virtual_keypad] == event.GetKeyCode()) {
			m_keys[i].isPressed = true;
			break;
		}
	}

	event.Skip();
}

void Input::handle_debugger(std::unique_ptr<Debugger>& debugger) {
	if (!debugger) {
		return;
	}

	bool keys_state[16] = {};
	for (int32_t i = 0; i < kNumberOfKeys; i++) {
		keys_state[i] = m_keys[i].isPressed;
	}
	debugger->update_keypad(keys_state, 16);
}
