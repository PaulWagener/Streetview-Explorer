#include "MainFrame.h"
#include "Application.h"

#if __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif

bool Application::OnInit() {

    wxImage::AddHandler(new wxPNGHandler);
    
#if __WXMAC__
    /**
     * This code serves no purpose other than debugging.
     * A wxWidgets program running outside a bundle is like a ghost,
     * it can't receive input and it can't have focus.
     * Because I don't want to package my app in a bundle every time I test
     * something out I run this code which brings back normal behaviour even without the bundle
     */
    ProcessSerialNumber PSN;
    GetCurrentProcess(&PSN);
    TransformProcessType(&PSN, kProcessTransformToForegroundApplication);
#endif

    //Create the mainframe which creates the rest
    new MainFrame();
    
    return true;     
}

IMPLEMENT_APP(Application)