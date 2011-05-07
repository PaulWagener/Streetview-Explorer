#ifndef PREFERENCESDIALOG_H
#define	PREFERENCESDIALOG_H

#include <wx/wx.h>

class PreferencesDialog : public wxFrame {
public:
    PreferencesDialog(wxWindow *parent);
    ~PreferencesDialog();
    static bool preferencesVisible;
private:
    void OnScroll(wxScrollEvent& event);
    void OnCheckbox(wxCommandEvent &event);
};

#endif	/* PREFERENCESDIALOG_H */

