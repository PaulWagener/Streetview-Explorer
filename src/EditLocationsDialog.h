/* 
 * File:   EditLocationsDialog.h
 * Author: paulwagener
 *
 * Created on 22 april 2011, 18:07
 */

#ifndef EDITLOCATIONSDIALOG_H
#define	EDITLOCATIONSDIALOG_H

#include <wx/wx.h>
#include <wx/listctrl.h>

using namespace std;

/**
 * Dialog box only used to configure the locations in settings.locations
 * 
 * @param frame
 */
class EditLocationsDialog : public wxDialog {
public:
    EditLocationsDialog(wxWindow *frame);
    ~EditLocationsDialog();
private:
    void OnEditItem(wxCommandEvent &event);
    void OnEditFinished(wxListEvent &event);
    void OnSelectionChanges(wxCommandEvent &event);

    void OnAdd(wxCommandEvent &event);
    void OnEdit(wxCommandEvent &event);
    void OnRemove(wxCommandEvent &event);
    void OnUp(wxCommandEvent &event);
    void OnDown(wxCommandEvent &event);

    void OnCloseWindow(wxCloseEvent &event);

    int GetSelected();
    void UpdateButtons();
    void RefillList(unsigned int selected = -1);

    wxString OpenPasteDialog();
    void OnPasteLink(wxCommandEvent &event);
    
    bool editing;

    wxListCtrl *list_locations;

    wxButton *addButton;
    wxButton *editButton;
    wxButton *removeButton;
    wxButton *upButton;
    wxButton *downButton;
};

#endif	/* EDITLOCATIONSDIALOG_H */

