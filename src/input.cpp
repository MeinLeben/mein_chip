#include "input.h"

Input::Input() {
	m_keys[0x0].keycodes[0] = SDLK_0; m_keys[0x0].keycodes[1] = SDLK_x;
	m_keys[0x1].keycodes[0] = SDLK_1; m_keys[0x1].keycodes[1] = SDLK_1;
	m_keys[0x2].keycodes[0] = SDLK_2; m_keys[0x2].keycodes[1] = SDLK_2;
	m_keys[0x3].keycodes[0] = SDLK_3; m_keys[0x3].keycodes[1] = SDLK_3;
	m_keys[0x4].keycodes[0] = SDLK_4; m_keys[0x4].keycodes[1] = SDLK_q;
	m_keys[0x5].keycodes[0] = SDLK_5; m_keys[0x5].keycodes[1] = SDLK_w;
	m_keys[0x6].keycodes[0] = SDLK_6; m_keys[0x6].keycodes[1] = SDLK_e;
	m_keys[0x7].keycodes[0] = SDLK_7; m_keys[0x7].keycodes[1] = SDLK_a;
	m_keys[0x8].keycodes[0] = SDLK_8; m_keys[0x8].keycodes[1] = SDLK_s;
	m_keys[0x9].keycodes[0] = SDLK_9; m_keys[0x9].keycodes[1] = SDLK_d;
	m_keys[0xA].keycodes[0] = SDLK_a; m_keys[0xA].keycodes[1] = SDLK_z;
	m_keys[0xB].keycodes[0] = SDLK_b; m_keys[0xB].keycodes[1] = SDLK_c;
	m_keys[0xC].keycodes[0] = SDLK_c; m_keys[0xC].keycodes[1] = SDLK_4;
	m_keys[0xD].keycodes[0] = SDLK_d; m_keys[0xD].keycodes[1] = SDLK_r;
	m_keys[0xE].keycodes[0] = SDLK_e; m_keys[0xE].keycodes[1] = SDLK_f;
	m_keys[0xF].keycodes[0] = SDLK_f; m_keys[0xF].keycodes[1] = SDLK_v;
}

void Input::handle_event(SDL_Event* pEvent) {
	if (!pEvent) {
		return;
	}

	if (pEvent->type == SDL_KEYUP || pEvent->type == SDL_KEYDOWN) {
		for (int i = 0; i < kNumberOfKeys; i++) {
			if (m_keys[i].keycodes[m_use_virtual_keypad] == pEvent->key.keysym.sym) {
				m_keys[i].isPressed = pEvent->type == SDL_KEYDOWN;
			}
		}
	}
}