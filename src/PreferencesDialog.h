#ifndef PREFERENCESDIALOG_H
#define	PREFERENCESDIALOG_H

#include <wx/wx.h>

class PreferencesDialog : public wxDialog {
public:
    PreferencesDialog(wxWindow *parent);
    ~PreferencesDialog();
private:
    void OnScroll(wxScrollEvent& event);
};

#endif	/* PREFERENCESDIALOG_H */

