#include "pch.h"

#include "loader.h"

class MeinChip {
public:
	static int32_t Run(const char* pTitle, int32_t width, int32_t height, int32_t argc, char* argv[]) {
		MeinChip app;
		if (!app.Initialize(pTitle, width, height, argc, argv)) {
			return -1;
		}

		return app.Run();
	}

private:
	~MeinChip() {
		SDL_DestroyRenderer(m_pRenderer);
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();

		delete[] m_rom.pData;
	}

	bool Initialize(const char* pTitle, int32_t width, int32_t height, int32_t argc, char* argv[]) {
		if (argc != 2) {
			std::cerr << "Missing rom path as argument." << std::endl;
			return false;
		}
		else {
			if (!Loader::load(argv[1], &m_rom.pData, m_rom.size)) {
				return false;
			}
		}

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
			std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
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

		return true;
	}

	int32_t Run() {
		SDL_Event event;

		while (true) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					return 0;
				}
			}

			Render();
		}

		return 0;
	}

	void Render() {
		SDL_SetRenderDrawColor(m_pRenderer, 255, 95, 31, 255);
		SDL_RenderClear(m_pRenderer);
		SDL_RenderPresent(m_pRenderer);
	}

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	struct Rom {
		char* pData = nullptr;
		size_t size = 0;
	};

	Rom m_rom = {};
};

int32_t main(int32_t argc, char* argv[]) {
	return MeinChip::Run("Mein Chip8 Interpreter", 720, 405, argc, argv);
}
