#include "cpu.h"
#include "debugger.h"
#include "display.h"
#include "input.h"
#include "loader.h"
#include "memory.h"

const SDL_Color background = {31, 31, 31, 255};

class MeinChip {
public:
	static int32_t run(const char* pTitle, int32_t width, int32_t height, int32_t argc, char* argv[]) {
		MeinChip app;
		if (!app.initialize(pTitle, width, height)) {
			return -1;
		}

		if (argc != 2) {
			std::cerr << "Missing rom path as argument." << std::endl;
			return -1;
		}
		
		if (!app.load(argv[1])) {
			return -1;
		}

		return app.run();
	}

private:
	~MeinChip() {
		SDL_DestroyRenderer(m_pRenderer);
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();

		delete[] m_rom.pData;

		delete m_pCPU;
		delete m_pMemory;
		delete m_pInput;
		delete m_pDisplay;

	}

	bool initialize(const char* pTitle, int32_t width, int32_t height) {

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
			std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
			return false;
		}

		if (TTF_Init() < 0) {
			std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
			return false;
		}

		m_pWindow = SDL_CreateWindow(
			pTitle,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			0);

		if (!m_pWindow) {
			std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
			return false;
		}

		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

		if (!m_pRenderer) {
			std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
			return false;
		}

#if USE_DEBUGGER
		m_debugger = Debugger::create(m_pWindow);
#endif
		const uint32_t display_width = 64;
		const uint32_t display_height = 32;
		const uint32_t display_pixel_scale = 10;

		const uint32_t x = width / 2 - display_width * display_pixel_scale / 2;

		m_pDisplay = new Display(x, 20, display_pixel_scale, display_width, display_height);
		m_pInput = new Input;
		m_pMemory = new Memory(4096);
		m_pCPU = new CPU(m_debugger);

		SDL_RaiseWindow(m_pWindow);

		m_initialized = true;

		return true;
	}

	bool load(const char* pPath) {
		if (!m_initialized) {
			return false;
		}

		if (!Loader::load(pPath, &m_rom.pData, m_rom.size)) {
			return false;
		}

		m_pMemory->write(0x0200, m_rom.pData, m_rom.size);

		return true;
	}

	int32_t run() {
		if (!m_initialized) {
			return -1;
		}

		SDL_Event event;

		bool pause = true, step = false;
		uint32_t previous_time = 0;
		while (true) {
			while (SDL_PollEvent(&event)) {
				if (event.window.windowID == SDL_GetWindowID(m_pWindow)) {
					if (event.type == SDL_QUIT) {
						return 0;
					}

					if (event.type == SDL_WINDOWEVENT) {
						switch(event.window.event) {
						case SDL_WINDOWEVENT_CLOSE:
							return 0;
						case SDL_WINDOWEVENT_MINIMIZED:
							if (m_debugger) {
								m_debugger->minimize();
							}
							break;
						case SDL_WINDOWEVENT_SHOWN:
							if (m_debugger) {
								m_debugger->restore();
							}
							break;
						default:
							break;
						}
					}

					if (event.type == SDL_KEYUP) {
						switch(event.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							return 0;
						case SDL_SCANCODE_TAB:
							if (m_debugger) {
								m_debugger->show(!m_debugger->is_visible());
							}
							SDL_RaiseWindow(m_pWindow);
							break;
						case SDL_SCANCODE_N:
							step = pause;
							break;
						case SDL_SCANCODE_SPACE:
							pause = !pause;
							break;
						case SDL_SCANCODE_K:
							m_pInput->use_virtual_keypad(!m_pInput->is_using_virtual_keypad());
							break;
						default:
							break;
						}
					}
				}

				if (m_debugger) {
					m_debugger->handle_event(&event);
				}
				m_pInput->handle_event(&event);
			}

			if (m_debugger) {
				m_debugger->tick();
			}

			uint32_t current_time = SDL_GetTicks();
			float delta = (float)(current_time - previous_time);
			if (delta > 1000 / 60.0f && (!pause || step)) {
				Bus bus = { m_pMemory, m_pDisplay, m_pInput };
				m_pCPU->tick(&bus, m_debugger);
				previous_time = current_time;
				step = false;
			}

			render();
		}

		return 0;
	}

	void render() {
		SDL_SetRenderDrawColor(m_pRenderer, background.r, background.g, background.b, background.a);
		SDL_RenderClear(m_pRenderer);

		m_pDisplay->draw(m_pRenderer);

		SDL_RenderPresent(m_pRenderer);
	}

	bool m_initialized = false;

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	struct Rom {
		uint8_t* pData = nullptr;
		size_t size = 0;
	};

	Rom m_rom = {};

	Display* m_pDisplay = nullptr;
	Input* m_pInput = nullptr;
	Memory* m_pMemory = nullptr;
	CPU* m_pCPU = nullptr;
#if USE_DEBUGGER
	std::unique_ptr<Debugger> m_debugger;
#endif
};

int32_t main(int32_t argc, char* argv[]) {
	return MeinChip::run("Mein Chip8 Interpreter", 720, 405, argc, argv);
}
