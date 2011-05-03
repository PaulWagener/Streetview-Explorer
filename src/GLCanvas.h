/* 
 * File:   GLCanvas.h
 * Author: paulwagener
 *
 * Created on 16 april 2011, 22:16
 */

#ifndef GLCANVAS_H
#define	GLCANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "Explorer.h"

class ExplorerHelperThread : public wxThread {
public:
    ExplorerHelperThread(Explorer &explorer);

protected:
    void* Entry();

private:
    Explorer &explorer;
};

class GLCanvas : public wxGLCanvas {
    wxGLContext* m_context;

    int trap_x, trap_y;

public:
    wxTimer *timer;
    GLCanvas(wxWindow* parent, const char *pano_id);
    ~GLCanvas();
    bool capture;
    void disableCapture();
    bool ignoreMouseEvents;

    Explorer explorer;

    void OnResize(wxSizeEvent& evt);
    void onPaint(wxPaintEvent& evt);

    // events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void keyDown(wxKeyEvent& event);
    void keyUp(wxKeyEvent& event);


    void OnTimer(wxTimerEvent& event);
};

#endif	/* GLCANVAS_H */

