#include "GLCanvas.h"
#include <wx/wx.h>

#if __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif

/**
 * Most simple wxThread subclass, all it does is reset the requestThread flag
 * and call the downloadThread.
 * @param explorer
 */
ExplorerHelperThread::ExplorerHelperThread(Explorer& explorer)
:explorer(explorer) {
}

void* ExplorerHelperThread::Entry() {
    explorer.requestDownloadThread = false;
    explorer.downloadThread();

    return NULL;
}

const int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16};


/**
 * Set up an OpenGL Canvas which the Explorer class can draw in
 * @param parent
 * @param pano_id
 */
GLCanvas::GLCanvas(wxWindow* parent, const char* pano_id) :
wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
explorer(pano_id) {
    m_context = new wxGLContext(this);
    timer = new wxTimer(this);
    timer->Start(20);
    
    capture = false;
    ignoreMouseEvents = false;
}

GLCanvas::~GLCanvas() {
    timer->Stop();
    delete timer;
    delete m_context;
}

/**
 * Mouse move callback used for camera movement by the user
 * @param event
 */
void GLCanvas::mouseMoved(wxMouseEvent& event) {
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
void GLCanvas::mouseDown(wxMouseEvent& event) {
    if (!capture) {
        this->SetCursor(wxCURSOR_BLANK);
        capture = true;
        trap_x = event.m_x;
        trap_y = event.m_y;
        WarpPointer(trap_x, trap_y);
    }
}

/**
 * Zoom in and out with the camera
 * @param event
 */
void GLCanvas::mouseWheelMoved(wxMouseEvent& event) {
    //TODO: ZOOM
}

/**
 * Handle player controls
 * @param event
 */
void GLCanvas::keyDown(wxKeyEvent& event) {
    if (event.m_keyCode == 'W') explorer.player.keys.forward = true;
    if (event.m_keyCode == 'S') explorer.player.keys.backward = true;
    if (event.m_keyCode == 'A') explorer.player.keys.strafe_left = true;
    if (event.m_keyCode == 'D') explorer.player.keys.strafe_right = true;
    if (event.m_keyCode == 'Q') explorer.player.keys.rotate_left = true;
    if (event.m_keyCode == 'E') explorer.player.keys.rotate_right = true;

    if (event.m_keyCode == WXK_ESCAPE) {
        disableCapture();
    }
}

void GLCanvas::keyUp(wxKeyEvent& event) {
    if (event.m_keyCode == 'W') explorer.player.keys.forward = false;
    if (event.m_keyCode == 'S') explorer.player.keys.backward = false;
    if (event.m_keyCode == 'A') explorer.player.keys.strafe_left = false;
    if (event.m_keyCode == 'D') explorer.player.keys.strafe_right = false;
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

void GLCanvas::OnResize(wxSizeEvent& evt) {
    Refresh();
}

void GLCanvas::OnTimer(wxTimerEvent& event) {
    Refresh();
}

bool fffirst = true;

void GLCanvas::onPaint(wxPaintEvent& evt) {
    if (!IsShown())
        return;

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    //Check if a download is requested and call downloadThread() in a seperate thread
    //if it is. Note that the below is not a memory leak, detached threads (the default)
    //automatically delete themselves once they finish running
    if(explorer.requestDownloadThread) {
        ExplorerHelperThread *thread = new ExplorerHelperThread(explorer);
        thread->Create();
        thread->Run();
    }

    


    if (fffirst) {
        explorer.init();
        

        fffirst = false;
    }

    wxSize size = GetSize();
    explorer.display(size.GetWidth(), size.GetHeight());
    SwapBuffers();
}

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_MOTION(GLCanvas::mouseMoved)
EVT_LEFT_DOWN(GLCanvas::mouseDown)
EVT_SIZE(GLCanvas::OnResize)
EVT_KEY_DOWN(GLCanvas::keyDown)
EVT_KEY_UP(GLCanvas::keyUp)
EVT_MOUSEWHEEL(GLCanvas::mouseWheelMoved)
EVT_PAINT(GLCanvas::onPaint)

EVT_TIMER(wxID_ANY, GLCanvas::OnTimer)
END_EVENT_TABLE()