#include "GLCanvas.h"
#include <wx/wx.h>
#include "statustext.h"

#if __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif

/**
 * Most simple wxThread subclass, all it does is reset the requestThread flag
 * and call the downloadThread.
 * @param explorer
 */
ExplorerHelperThread::ExplorerHelperThread(Explorer& explorer)
: explorer(explorer) {
}

void* ExplorerHelperThread::Entry() {
    explorer.requestDownloadThread = false;
    explorer.downloadThread();

    return NULL;
}

int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16};

/**
 * Set up an OpenGL Canvas which the Explorer class can draw in
 * 
 * @param parent
 * @param pano_id Panorama id of the first panorama to (down)load and display
 */
GLCanvas::GLCanvas(wxWindow* parent, const char* pano_id) :
wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
explorer(pano_id) {
    m_context = new wxGLContext(this);
    timer = new wxTimer(this);
    timer->Start(20);

    capture = false;
    ignoreMouseEvents = false;

    Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(GLCanvas::OnMouseMotion));
    Connect(wxID_ANY, wxEVT_MOUSEWHEEL, wxMouseEventHandler(GLCanvas::OnMouseWheel));
    Connect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(GLCanvas::OnMouseDown));

    Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(GLCanvas::OnKeyDown));
    Connect(wxID_ANY, wxEVT_KEY_UP, wxKeyEventHandler(GLCanvas::OnKeyUp));

    Connect(wxID_ANY, wxEVT_PAINT, wxPaintEventHandler(GLCanvas::OnPaint));
    Connect(wxID_ANY, wxEVT_SIZE, wxSizeEventHandler(GLCanvas::OnResize));
    Connect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(GLCanvas::OnTimer));

    this->SetFocus();
}

GLCanvas::~GLCanvas() {
    timer->Stop();
}

/**
 * Mouse move callback used for camera movement by the user
 * @param event
 */
void GLCanvas::OnMouseMotion(wxMouseEvent& event) {
    //Ingore mouse movements if thee mouse isn't in capture mode (invisible)
    //or there is the possibility that mouse movements may propagate indefinitely
    if (!capture) return;
    if (ignoreMouseEvents) return;

    //Don't move if the mouse is technically in the same place.
    if (event.m_x == trap_x && event.m_y == trap_y)
        return;

    explorer.player.moveMouse(event.m_x - trap_x, event.m_y - trap_y);

#ifdef __WXMAC__
    CGSetLocalEventsSuppressionInterval(0.0);
#endif

    ignoreMouseEvents = true;
    WarpPointer(trap_x, trap_y);
    ignoreMouseEvents = false;
}

/**
 * Capture the mouse if it isn't already
 * @param event
 */
void GLCanvas::OnMouseDown(wxMouseEvent& event) {
    if (!capture) {
        this->SetCursor(wxCURSOR_BLANK);
        capture = true;
        trap_x = event.m_x;
        trap_y = event.m_y;
        //WarpPointer(trap_x, trap_y);
        setStatus("Press <esc> to release the mouse again...");
    }
}

/**
 * Zoom in and out with the camera
 * @param event
 */
void GLCanvas::OnMouseWheel(wxMouseEvent& event) {
    explorer.player.mouseWheel(event.GetWheelRotation());
}

/**
 * Handle player controls
 * @param event
 */
void GLCanvas::OnKeyDown(wxKeyEvent& event) {
    if (event.m_keyCode == 'W' || event.m_keyCode == WXK_UP) explorer.player.keys.forward = true;
    if (event.m_keyCode == 'S' || event.m_keyCode == WXK_DOWN) explorer.player.keys.backward = true;
    if (event.m_keyCode == 'A' || event.m_keyCode == WXK_LEFT) explorer.player.keys.strafe_left = true;
    if (event.m_keyCode == 'D' || event.m_keyCode == WXK_RIGHT) explorer.player.keys.strafe_right = true;
    if (event.m_keyCode == 'Q') explorer.player.keys.rotate_left = true;
    if (event.m_keyCode == 'E') explorer.player.keys.rotate_right = true;

    if (event.m_keyCode == WXK_ESCAPE) {
        disableCapture();
    }
}

void GLCanvas::OnKeyUp(wxKeyEvent& event) {
    if (event.m_keyCode == 'W' || event.m_keyCode == WXK_UP) explorer.player.keys.forward = false;
    if (event.m_keyCode == 'S' || event.m_keyCode == WXK_DOWN) explorer.player.keys.backward = false;
    if (event.m_keyCode == 'A' || event.m_keyCode == WXK_LEFT) explorer.player.keys.strafe_left = false;
    if (event.m_keyCode == 'D' || event.m_keyCode == WXK_RIGHT) explorer.player.keys.strafe_right = false;
    if (event.m_keyCode == 'Q') explorer.player.keys.rotate_left = false;
    if (event.m_keyCode == 'E') explorer.player.keys.rotate_right = false;
}

/**
 * Give mousecursor control back to the user
 */
void GLCanvas::disableCapture() {
    capture = false;
    this->SetCursor(wxCURSOR_DEFAULT);
}

void GLCanvas::OnResize(wxSizeEvent&) {
    Refresh();
}

void GLCanvas::OnTimer(wxTimerEvent&) {
    Refresh();
}

void GLCanvas::OnPaint(wxPaintEvent&) {
    if (!IsShown())
        return;

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    //Check if a download is requested and call downloadThread() in a seperate thread
    //if it is. Note that the below is not a memory leak, detached threads (the default)
    //automatically delete themselves once they finish running
    if (explorer.requestDownloadThread) {
        ExplorerHelperThread *thread = new ExplorerHelperThread(explorer);
        thread->Create();
        thread->Run();
    }

    wxSize size = GetSize();
    explorer.display(size.GetWidth(), size.GetHeight());
    SwapBuffers();
}
