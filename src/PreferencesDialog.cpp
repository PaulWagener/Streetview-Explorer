#include "PreferencesDialog.h"
#include "Settings.h"

#define VERTICAL_ACC_FASTEST 100
#define VERTICAL_ACC_BEST 1

#define HORIZONTAL_ACC_FASTEST 32
#define HORIZONTAL_ACC_BEST 1

#define NUMBER_OF_PANO_FASTEST 1
#define NUMBER_OF_PANO_BEST 20

enum {
    SLIDER_VERTICAL_ACC = wxID_HIGHEST,
    SLIDER_HORIZONTAL_ACC,
    SLIDER_NUMBER_OF_PANO,
};

/**
 * Create the preferences dialog and fill it with sliders
 * 
 * @param parent
 */
PreferencesDialog::PreferencesDialog(wxWindow *parent)
 : wxDialog(parent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(400, 200), wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT){
    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer *fgs = new wxFlexGridSizer(3, 2, 9, 15);

    fgs->Add(new wxStaticText(this, -1, "Vertical texture accuracy"));
    fgs->Add(new wxSlider(this, SLIDER_VERTICAL_ACC, settings.vertical_accuracy, VERTICAL_ACC_BEST, VERTICAL_ACC_FASTEST, wxDefaultPosition, wxDefaultSize, wxSL_LABELS | wxSL_INVERSE), 1, wxEXPAND);

    fgs->Add(new wxStaticText(this, -1, "Horizontal model accuracy"));
    fgs->Add(new wxSlider(this, SLIDER_HORIZONTAL_ACC, settings.horizontal_accuracy, HORIZONTAL_ACC_BEST, HORIZONTAL_ACC_FASTEST, wxDefaultPosition, wxDefaultSize, wxSL_LABELS | wxSL_INVERSE), 1, wxEXPAND);

    fgs->Add(new wxStaticText(this, -1, "Number of panorama's"));
    fgs->Add(new wxSlider(this, SLIDER_NUMBER_OF_PANO, settings.numPanoramas, NUMBER_OF_PANO_FASTEST, NUMBER_OF_PANO_BEST, wxDefaultPosition, wxDefaultSize, wxSL_LABELS), 1, wxEXPAND);

    //Let sliders fill up all available space in the window
    fgs->AddGrowableCol(1, 1);
    fgs->AddGrowableRow(0, 1);
    fgs->AddGrowableRow(1, 1);
    fgs->AddGrowableRow(2, 1);

    hbox->Add(fgs, 1, wxALL | wxEXPAND, 5);
    this->SetSizer(hbox);

    //Add event handlers
    Connect(SLIDER_VERTICAL_ACC, wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(PreferencesDialog::OnScroll));
    Connect(SLIDER_HORIZONTAL_ACC, wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(PreferencesDialog::OnScroll));
    Connect(SLIDER_NUMBER_OF_PANO, wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(PreferencesDialog::OnScroll));
}

/**
 * Save the settings when the dialog closes
 */
PreferencesDialog::~PreferencesDialog() {
    settings.saveSettings();
}

/**
 * Adjust the settings as the user slides the slider
 * All sliders use this same callback.
 *
 * @param event
 */
void PreferencesDialog::OnScroll(wxScrollEvent& event) {
    switch (event.GetId()) {
        case SLIDER_HORIZONTAL_ACC:
            settings.horizontal_accuracy = event.GetPosition();
            break;

        case SLIDER_VERTICAL_ACC:
            settings.vertical_accuracy = event.GetPosition();
            break;

        case SLIDER_NUMBER_OF_PANO:
            settings.numPanoramas = event.GetPosition();
            break;
    }
}

