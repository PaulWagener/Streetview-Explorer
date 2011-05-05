#include "statustext.h"
#include "MainFrame.h"

void setStatus(const char *newStatus, ...) {
    va_list parms;
    char newStatusPrinted[256] = {0};

    // Try to print in the allocated space.
    va_start(parms, newStatus);
    vsnprintf(newStatusPrinted, sizeof(newStatusPrinted) / sizeof(newStatusPrinted[0]), newStatus, parms);
    va_end(parms);


    if (MainFrame::mainframe != NULL) {
        wxCommandEvent event(myEVT_SET_STATUS, wxID_ANY);
        event.SetString(wxString::FromAscii(newStatusPrinted));
        wxPostEvent(MainFrame::mainframe, event);
    }
}
