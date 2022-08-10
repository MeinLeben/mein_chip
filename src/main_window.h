#pragma once

wxDECLARE_EVENT(EVT_FILE_OPENED, wxCommandEvent);

class MeinChip;

class MainWindow : public wxFrame {
public:
	MainWindow(const std::string& title, int32_t width, int32_t height);
	~MainWindow();

	inline SDL_Window* GetSDLWindow() const {
		return m_pWindow;
	}

private:
	void OnOpen(wxCommandEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	wxPanel* m_pPanel = nullptr;

	SDL_Window* m_pWindow = nullptr;
};