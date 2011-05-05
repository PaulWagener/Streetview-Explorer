#include "PreferencesDialog.h"
#include "Settings.h"

#define VERTICAL_ACC_FASTEST 40
#define VERTICAL_ACC_BEST 1

#define HORIZONTAL_ACC_FASTEST 32
#define HORIZONTAL_ACC_BEST 1

#define ZOOM_LEVEL_MIN 1
#define ZOOM_LEVEL_MAX 3

enum {
    SLIDER_VERTICAL_ACC = wxID_HIGHEST,
    SLIDER_HORIZONTAL_ACC,
    SLIDER_ZOOM_LEVEL,
    CHECKBOX_MIPMAPPING,
    CHECKBOX_WIREFRAME
};

/**
 * Create the preferences dialog and fill it with sliders
 * 
 * @param parent
 */
PreferencesDialog::PreferencesDialog(wxWindow *parent)
 : wxDialog(parent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(400, 200), wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT){
    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer *fgs = new wxFlexGridSizer(2, 9, 15);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Vertical texture accuracy"));
    fgs->Add(new wxSlider(this, SLIDER_VERTICAL_ACC, settings.vertical_accuracy, VERTICAL_ACC_BEST, VERTICAL_ACC_FASTEST, wxDefaultPosition, wxDefaultSize, wxSL_LABELS | wxSL_INVERSE), 1, wxEXPAND);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Horizontal model accuracy"));
    fgs->Add(new wxSlider(this, SLIDER_HORIZONTAL_ACC, settings.horizontal_accuracy, HORIZONTAL_ACC_BEST, HORIZONTAL_ACC_FASTEST, wxDefaultPosition, wxDefaultSize, wxSL_LABELS | wxSL_INVERSE), 1, wxEXPAND);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Detail level"));
    fgs->Add(new wxSlider(this, SLIDER_ZOOM_LEVEL, settings.zoom_level, ZOOM_LEVEL_MIN, ZOOM_LEVEL_MAX, wxDefaultPosition, wxDefaultSize, wxSL_LABELS), 1, wxEXPAND);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Mipmapping"));
    wxCheckBox *mipmappingCheckbox = new wxCheckBox(this, CHECKBOX_MIPMAPPING, "");
    mipmappingCheckbox->SetValue(settings.mipmapping);
    fgs->Add(mipmappingCheckbox);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Wireframe"));
    wxCheckBox *wireframeCheckbox = new wxCheckBox(this, CHECKBOX_WIREFRAME, "");
    wireframeCheckbox->SetValue(settings.wireframe);
    fgs->Add(wireframeCheckbox);


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
    Connect(SLIDER_ZOOM_LEVEL, wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(PreferencesDialog::OnScroll));
    Connect(CHECKBOX_MIPMAPPING, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PreferencesDialog::OnCheckbox));
    Connect(CHECKBOX_WIREFRAME, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PreferencesDialog::OnCheckbox));
    this->Fit();
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

        case SLIDER_ZOOM_LEVEL:
            settings.zoom_level = event.GetPosition();
            break;
    }
}

void PreferencesDialog::OnCheckbox(wxCommandEvent &event) {
    switch(event.GetId()) {
        case CHECKBOX_MIPMAPPING:
            settings.mipmapping = event.IsChecked();
            break;

        case CHECKBOX_WIREFRAME:
            settings.wireframe = event.IsChecked();
            break;
    }
}

