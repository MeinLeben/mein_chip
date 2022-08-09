#pragma once

class Debugger;

class Input {
public:
	Input(wxWindow* pWindow);

	void OnKeyUp(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	void handle_debugger(std::unique_ptr<Debugger>& debugger);

	void use_virtual_keypad(bool use) {
		m_use_virtual_keypad = use;
	}

	bool is_using_virtual_keypad() const {
		return m_use_virtual_keypad;
	}

	bool is_key_pressed(int8_t key) {
		if (key > 0xF) {
			return false;
		}

		return m_keys[key].isPressed;
	}

	bool is_any_key_pressed(int8_t& key_pressed) const {
		for (int8_t i = 0; i < kNumberOfKeys; i++) {
			if (m_keys[i].isPressed) {
				key_pressed = i;
				return true;
			}
		}
		return false;
	}

private:
	struct key {
		int32_t keycodes[2] = {};
		bool isPressed = false;
	};

	static const uint8_t kNumberOfKeys = 16;

	key m_keys[kNumberOfKeys] = {};
	bool m_use_virtual_keypad = true;
};