#include "mein_chip.h"
#include "cpu.h"
#include "display.h"
#include "input.h"
#include "loader.h"
#include "main_window.h"
#include "memory.h"

static const std::string APP_TITLE = "Mein Chip8 Interpreter";
static const uint32_t APP_WIDTH = 720;
static const uint32_t APP_HEIGHT = 405;
static const SDL_Color BACKGROUND = {31, 31, 31, 255};

static const uint32_t DISPLAY_WIDTH = 64;
static const uint32_t DISPLAY_HEIGHT = 32;
static const uint32_t DISPLAY_PIXEL_SCALE = 10;
static const uint32_t DISPLAY_X = APP_WIDTH / 2 - DISPLAY_WIDTH * DISPLAY_PIXEL_SCALE / 2;
static const uint32_t DISPLAY_Y = 20;

class UpdateTimer : public wxTimer {
public:
	UpdateTimer(class MeinChip* pMeinChip, int32_t intervalInMilliSeconds)
		: m_pMeinChip(pMeinChip)
		, m_intervalInMilliSeconds(intervalInMilliSeconds) {
	}

	void Notify() {
		m_pMeinChip->Update();
	}

	void Start() {
		wxTimer::Start(m_intervalInMilliSeconds);
	}

private:
	MeinChip* m_pMeinChip = nullptr;
	int32_t m_intervalInMilliSeconds = 0;
};

MeinChip::~MeinChip() {
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();

	delete[] m_rom.pData;

	delete m_pCPU;
	delete m_pMemory;
	delete m_pInput;
	delete m_pDisplay;

}

bool MeinChip::OnInit() {
	std::srand(std::time(nullptr));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() < 0) {
		std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
		return false;
	}

	m_pMainWindow = new MainWindow(APP_TITLE, APP_WIDTH, APP_HEIGHT);
	m_pMainWindow->Show(true);
	m_pMainWindow->Bind(EVT_FILE_OPENED, &MeinChip::OnFileOpen, this);
	m_pMainWindow->Bind(wxEVT_KEY_UP, &MeinChip::OnKeyUp, this);
	m_pMainWindow->Bind(wxEVT_KEY_DOWN, &MeinChip::OnKeyDown, this);
	m_pMainWindow->Bind(wxEVT_CLOSE_WINDOW, &MeinChip::OnMainWindowClose, this);

	m_pRenderer = SDL_CreateRenderer(m_pMainWindow->GetSDLWindow(), -1, SDL_RENDERER_ACCELERATED);
	if (!m_pRenderer) {
		std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pInput = new Input(m_pMainWindow);

	m_pDisplay = new Display(DISPLAY_X, DISPLAY_Y, DISPLAY_PIXEL_SCALE, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	m_pMemory = new Memory(4096);
	m_pCPU = new CPU(m_debugger);

	m_initialized = true;

	m_pUpdateTimer = new UpdateTimer(this, 10);
	m_pUpdateTimer->Start();

	return true;
}

void MeinChip::OnKeyUp(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
		case WXK_SPACE: {
			if (m_romLoaded) {
				m_pause = !m_pause;
			} else {
				m_pause = true;
			}
		} break;
		case 'N': {
			m_step = m_pause;
		} break;
		case 'K': {
			m_pInput->use_virtual_keypad(!m_pInput->is_using_virtual_keypad());
		} break;
		case WXK_BACK: {
			Reset();
		} break;
		default:
			break;
	}

	event.Skip();
}

void MeinChip::OnKeyDown(wxKeyEvent& event) {
	switch(event.GetKeyCode()) {
		case WXK_UP: {
			m_num_cycles_per_tick++;
			UpdateWindowTitle();
		} break;
		case WXK_DOWN: {
			if (m_num_cycles_per_tick > 1) {
				m_num_cycles_per_tick--;
			}
			UpdateWindowTitle();
		} break;
		default:
			break;
	}

	event.Skip();
}

void MeinChip::OnFileOpen(wxCommandEvent& event) {
	const std::string path = event.GetString().ToStdString();
	if (!Loader::load(path.c_str(), &m_rom.pData, m_rom.size)) {
		std::cerr << "Failed to load rom." << std::endl;
		return;
	}

	Reset();

	m_pMemory->write(0x0200, m_rom.pData, m_rom.size);

	m_romLoaded = true;
	m_pause = false;
}

void MeinChip::OnMainWindowClose(wxCloseEvent& event) {
	m_pUpdateTimer->Stop();
	event.Skip();
}

void MeinChip::Update() {
	if (!m_initialized) {
		return;
	}

	UpdateWindowTitle();

	int32_t mouse_x, mouse_y;
	int32_t mouse_bitmask = SDL_GetMouseState(&mouse_x, &mouse_y);

	if (m_romLoaded && (!m_pause || m_step)) {
		for (int i = 0; i < m_num_cycles_per_tick; i++) {
			Bus bus = { m_pMemory, m_pDisplay, m_pInput };
			m_pCPU->Tick(&bus, m_debugger);

			if (m_pause) {
				break;
			}
		}
		m_step = false;
	}

	Render();
}

void MeinChip::Reset() {
	m_pCPU->Reset();
	m_pDisplay->clear();
}

void MeinChip::Render() {
	SDL_SetRenderDrawColor(m_pRenderer, BACKGROUND.r, BACKGROUND.g, BACKGROUND.b, BACKGROUND.a);
	SDL_RenderClear(m_pRenderer);

	m_pDisplay->draw(m_pRenderer);

	SDL_RenderPresent(m_pRenderer);
}

void MeinChip::UpdateWindowTitle() {
	std::string title = APP_TITLE + " - cycles per tick: " + std::to_string(m_num_cycles_per_tick);
	m_pMainWindow->SetTitle(title);
}

int32_t main(int32_t argc, char* argv[]) {
	wxApp::SetInstance(new MeinChip());

	wxEntryStart(argc, argv);

	wxTheApp->CallOnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();

	wxEntryCleanup();
	return 0;
}
