#pragma once

class MeinChip;

class MainWindow : public wxFrame {
public:
	MainWindow(MeinChip* pApp, const std::string& title, int32_t width, int32_t height);
	~MainWindow();

	typedef void (MeinChip::*Callback)(const void* pData);

	uint32_t BindOnOpenHandler(MeinChip* pInstance, Callback callback);
	void UnBindOnOpenHandler(uint32_t id);

	inline SDL_Window* GetSDLWindow() const {
		return m_pWindow;
	}

private:
	struct EventHandler{
		MeinChip* m_pInstance = nullptr;
		Callback m_callback;
	};

	typedef std::unordered_map <uint32_t, EventHandler> EventHandlerContainer;

	void OnOpen(wxCommandEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	wxPanel* m_pPanel = nullptr;

	SDL_Window* m_pWindow = nullptr;

	EventHandlerContainer m_onOpenHandlers;
	EventHandlerContainer m_onExitHandlers;

	uint32_t m_handlerIds = 0;
};