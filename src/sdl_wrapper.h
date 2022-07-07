#pragma once

namespace SDLWrapper {

template <typename T>
class Base {
public:
	virtual ~Base() = default;

	inline T* get() const {
		return m_pObject;
	}

protected:
	inline void check(const std::string& error) {
		if (!m_pObject) {
			cleanup();
			throw std::runtime_error(error.c_str());
		}
	}

	virtual void cleanup() = 0;

protected:

	T* m_pObject = nullptr;
};

class Window : public Base<SDL_Window>{
public:
	Window(const std::string& title, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t flags) {
		m_pObject = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
		check("Failed to create window.");
	}

	~Window() {
		cleanup();
	}

protected:
	virtual void cleanup() {
		SDL_DestroyWindow(m_pObject);
	}
};

class Renderer : public Base<SDL_Renderer> {
public:
	Renderer(SDL_Window* pWindow, int32_t index, uint32_t flags) {
		m_pObject = SDL_CreateRenderer(pWindow, index, flags);
		check("Failed to create renderer.");
	}

	~Renderer() {
		cleanup();
	}

protected:
	virtual void cleanup() {
		SDL_DestroyRenderer(m_pObject);
	}
};

};