#include "main_window.h"
#include "mein_chip.h"
#include "input.h"

MainWindow::MainWindow(MeinChip* pApp, const std::string& title, int32_t width, int32_t height) 
	: wxFrame(nullptr, wxID_ANY, title.c_str(), wxDefaultPosition, wxDefaultSize) {

	SetClientSize(wxSize(width, height));
	
	m_pPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
	m_pPanel->Bind(wxEVT_KEY_UP, &MainWindow::OnKeyUp, this);
	m_pPanel->Bind(wxEVT_KEY_DOWN, &MainWindow::OnKeyDown, this);

	wxMenu* pMenuFile = new wxMenu;
	pMenuFile->Append(wxID_OPEN);
	pMenuFile->Append(wxID_EXIT);

	wxMenuBar* pMenuBar = new wxMenuBar;
	pMenuBar->Append(pMenuFile, "&File");

	SetMenuBar(pMenuBar);

	Bind(wxEVT_MENU, &MainWindow::OnOpen, this, wxID_OPEN);
	Bind(wxEVT_CLOSE_WINDOW, &MeinChip::OnMainWindowClose, pApp);

	m_pWindow = SDL_CreateWindowFrom(GetHandle());
	if (!m_pWindow) {
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
	}
}

MainWindow::~MainWindow() {
	SDL_DestroyWindow(m_pWindow);
}

uint32_t MainWindow::BindOnOpenHandler(MeinChip* pInstance, Callback callback) {
	m_onOpenHandlers.insert({++m_handlerIds, EventHandler{pInstance, callback}});
	return m_handlerIds;
}

void MainWindow::UnBindOnOpenHandler(uint32_t id) {
	auto iter = m_onOpenHandlers.find(id);
	if (iter != m_onOpenHandlers.end()) {
		m_onOpenHandlers.erase(iter);
	}
}

void MainWindow::OnOpen(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Open ch8 file"), "", "", "ch8 files (*.ch8)|*.ch8", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	std::string path = openFileDialog.GetPath().ToStdString();
	for (auto iter : m_onOpenHandlers) {
		std::invoke(iter.second.m_callback, iter.second.m_pInstance, &path);
	}
}

void MainWindow::OnKeyUp(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
		case WXK_ESCAPE: {
			Close();
		}
		default:
			break;
	}

	event.ResumePropagation(1);
	event.Skip();
}

void MainWindow::OnKeyDown(wxKeyEvent& event) {
	event.ResumePropagation(1);
	event.Skip();
}
