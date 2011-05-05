#ifndef FRAMES_H
#define	FRAMES_H

#include "GLCanvas.h"
#include <wx/wx.h>

extern const wxEventType myEVT_SET_STATUS;

/**
 * The main frame that houses the OpenGL context and the initial main screen
 */
class MainFrame : public wxFrame {
public:
    MainFrame();

    //Public so that statustext.cpp may reach it.
    static MainFrame *mainframe;
private:
    GLCanvas *glCanvas;
    wxChoice *combobox;
    wxStatusBar *statusbar;
    

    bool isStartingWithPanorama;

    void OnGoToMainScreen(wxMenuEvent &event);
    void OnAbout(wxMenuEvent& WXUNUSED(event));
    void OnPreferences(wxMenuEvent& WXUNUSED(event));
    void OnQuit(wxMenuEvent& WXUNUSED(event));

    void OnSelectLocation(wxCommandEvent &event);
    void OnEditLocations(wxCommandEvent &event);
    void OnLinkPaste(wxCommandEvent &event);

    void OnPanoramaStart(wxCommandEvent &event);
    void OnSetStatus(wxCommandEvent &event);
    
    void StartWithPanorama(const char* pano_id);
    void ShowMain();
    
    void ReplaceSizer(wxSizer *sizer);
    void RefillLocations();
};

#endif	/* FRAMES_H */

