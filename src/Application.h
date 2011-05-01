#ifndef APPLICATION_H
#define	APPLICATION_H

#include <wx/wx.h>

/**
 * The application itself, all it does is launch the main window
 */
class Application : public wxApp {
public:
    virtual bool OnInit();
};

#endif	/* APPLICATION_H */

