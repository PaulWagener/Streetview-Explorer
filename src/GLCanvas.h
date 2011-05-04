#ifndef GLCANVAS_H
#define	GLCANVAS_H

#include <wx/wx.h>
#include "gl.h"
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
    void OnPaint(wxPaintEvent& evt);

    // events
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);


    void OnTimer(wxTimerEvent& event);
};

#endif	/* GLCANVAS_H */

