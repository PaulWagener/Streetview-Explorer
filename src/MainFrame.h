/* 
 * File:   Frames.h
 * Author: paulwagener
 *
 * Created on 16 april 2011, 22:20
 */

#ifndef FRAMES_H
#define	FRAMES_H

#include "GLCanvas.h"
#include <wx/wx.h>

/**
 * The main frame that houses the OpenGL context and the initial main screen
 */
class MainFrame : public wxFrame {
public:
    MainFrame();
    
private:
    GLCanvas *glCanvas;
    wxChoice *combobox;

    bool isStartingWithPanorama;

    void OnGoToMainScreen(wxMenuEvent &event);
    void OnAbout(wxMenuEvent& WXUNUSED(event));
    void OnPreferences(wxMenuEvent& WXUNUSED(event));
    void OnQuit(wxMenuEvent& WXUNUSED(event));

    void OnSelectLocation(wxCommandEvent &event);
    void OnEditLocations(wxCommandEvent &event);
    void OnLastLocation(wxCommandEvent &event);
    void OnLinkPaste(wxCommandEvent &event);

    void OnPanoramaStart(wxCommandEvent &event);
    
    void StartWithPanorama(const char* pano_id);
    void ShowMain();
    
    void ReplaceSizer(wxSizer *sizer);
    void RefillLocations();
};

#endif	/* FRAMES_H */

