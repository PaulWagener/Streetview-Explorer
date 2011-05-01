/* 
 * File:   PreferencesDialog.h
 * Author: paulwagener
 *
 * Created on 24 april 2011, 14:56
 */

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

