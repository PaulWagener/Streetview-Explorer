#include "MainFrame.h"
#include "GLCanvas.h"
#include "Panorama.h"
#include "EditLocationsDialog.h"
#include "Settings.h"
#include "PreferencesDialog.h"

using namespace std;


#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

const wxEventType myEVT_PANORAMA_START = wxNewEventType();
const wxEventType myEVT_SET_STATUS = wxNewEventType();

MainFrame* MainFrame::mainframe = NULL;

enum {
    COMBOBOX_LOCATIONS = wxID_HIGHEST,
    MENU_BACKTOMAIN,
    BUTTON_EDIT_LOCATIONS,
    BUTTON_LAST_LOCATION,
    TEXTBOX_PASTE_LINK
};

wxPanel *mainPanel;

MainFrame::MainFrame()
: wxFrame(NULL, wxID_ANY, "StreetView Explorer", wxDefaultPosition, wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT)),
isStartingWithPanorama(false) {

    mainframe = this;

    //Set up menu
    wxMenu *fileMenu = new wxMenu();
    fileMenu->Append(MENU_BACKTOMAIN, "&Go to main screen");
    fileMenu->Append(wxID_PREFERENCES, "&Preferences");
    fileMenu->Append(wxID_ABOUT, "&About...");
    fileMenu->Append(wxID_EXIT, "E&xit");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    statusbar = new wxStatusBar(this);
    SetStatusBar(statusbar);

    mainPanel = new wxPanel(this);

    //Event handlers
    Connect(MENU_BACKTOMAIN, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainFrame::OnGoToMainScreen));
    Connect(wxID_PREFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainFrame::OnPreferences));
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainFrame::OnAbout));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainFrame::OnQuit));
    Connect(wxID_ANY, myEVT_PANORAMA_START, wxCommandEventHandler(MainFrame::OnPanoramaStart));
    Connect(wxID_ANY, myEVT_SET_STATUS, wxCommandEventHandler(MainFrame::OnSetStatus));

    ShowMain();

    //Show window
    this->Center();
    this->Show();

    //StartWithPanorama("4R73SfAuIWfCCfnuIcHr3w");
}

void MainFrame::OnSetStatus(wxCommandEvent &event) {
    statusbar->SetStatusText(event.GetString());
}

/**
 * Do the actual work callers expect ShowPanorama() to do. Replace the contents of the frame
 * with an OpenGL window with the actual game.
 */
void MainFrame::OnPanoramaStart(wxCommandEvent &event) {
    glCanvas = new GLCanvas(mainPanel, (const char*) event.GetString());
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(glCanvas, 1, wxEXPAND);
    ReplaceSizer(sizer);
    this->Layout();
}

/**
 * Callback for menu option to go back to the main screen
 * 
 * @param event
 */
void MainFrame::OnGoToMainScreen(wxMenuEvent &event) {
    ShowMain();
}

/**
 * Open up a configuration panel through which users can adjust some parameters
 * @param
 */
void MainFrame::OnPreferences(wxMenuEvent& WXUNUSED(event)) {
    if (glCanvas != NULL)
        glCanvas->disableCapture();

    PreferencesDialog *prefs = new PreferencesDialog(this);
    prefs->ShowModal();
    delete prefs;
}

/**
 * Show information about me
 * @param
 */
void MainFrame::OnAbout(wxMenuEvent& WXUNUSED(event)) {
    wxMessageBox("This application is brought to you by Paul Wagener.\nhttp://code.google.com/p/streetview-explorer\nSpecial thanks to Ariane and the helpful people at #wxwidgets",
            "About StreetView Explorer",
            wxOK | wxICON_INFORMATION, this);
}

/**
 * Close the application upon the exit command
 * @param
 */
void MainFrame::OnQuit(wxMenuEvent& WXUNUSED(event)) {
    Close(true);
}

/**
 * When the user selects a location go directly to the 3D view
 * @param event
 */
void MainFrame::OnSelectLocation(wxCommandEvent &event) {
    const int selection = combobox->GetSelection();

    //Ignore first 'pick a destination' entry
    if (selection == 0)
        return;

    struct pano_location l = settings.locations[selection - 1];
    this->StartWithPanorama(l.pano_id);
}

/**
 * Show a modal dialog where the user can edit his favorite locations
 * @param event
 */
void MainFrame::OnEditLocations(wxCommandEvent &event) {
    //Edit locations dialog
    EditLocationsDialog *editLocations = new EditLocationsDialog(this);
    editLocations->ShowModal();
    delete editLocations;

    //Resynchronize locations in combobox
    RefillLocations();
}

/**
 * Button to start where the user left off the last time he explorer StreetView
 * @param event
 */
void MainFrame::OnLastLocation(wxCommandEvent &event) {
    StartWithPanorama(settings.last_pano);
}

/**
 * Callback for when the user pastes a link into the textfield.
 * When it
 * @param event
 */
void MainFrame::OnLinkPaste(wxCommandEvent &event) {
    //Paste events are usually send multiple times with the same content.
    //Ensure that we only start the panorama once
    if (isStartingWithPanorama)
        return;

    wxString link = event.GetString();

    int g = link.find("&panoid=");
    if (g == -1)
        return;

    wxString pasted_panoID = link.substr(g + strlen("&panoid="), PANOID_LENGTH);

    if (pasted_panoID.length() < PANOID_LENGTH)
        return;

    isStartingWithPanorama = true;
    StartWithPanorama((const char*) pasted_panoID);
}

/**
 * Synchronize the contents of the combobox with the actual locations in the settings
 */
void MainFrame::RefillLocations() {
    combobox->Clear();

    combobox->Append("<Pick a destination>");
    for (unsigned int i = 0; i < settings.locations.size(); i++)
        combobox->Append(settings.locations[i].name);

}

/**
 * Fill the contents of the frame with an OpenGL canvas with the actual application.
 * This method can be safely called from event threads from widgets that are about
 * to be deleted through this call.
 *
 * @param pano_id
 */
void MainFrame::StartWithPanorama(const char* pano_id) {
    wxCommandEvent event(myEVT_PANORAMA_START, GetId());
    event.SetString(pano_id);
    wxPostEvent(this, event);
}
#include "title.png.h"
#include <wx/mstream.h>

/**
 * Fill the frame with the mainscreen controls
 */
void MainFrame::ShowMain() {
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    //Space above
    sizer->AddStretchSpacer(1);

    //Horizontally centered title image
    wxBoxSizer *titleSizer = new wxBoxSizer(wxHORIZONTAL);
    titleSizer->AddStretchSpacer(1);

    wxMemoryInputStream title_png_is(title_png, sizeof (title_png));
    titleSizer->Add(new wxStaticBitmap(mainPanel, wxID_ANY, wxBitmap(title_png_is, wxBITMAP_TYPE_PNG)));
    titleSizer->AddStretchSpacer(1);
    sizer->Add(titleSizer, 0, wxEXPAND);


    //Some space between combobox and title
    sizer->AddSpacer(30);

    //Combobox
    combobox = new wxChoice(mainPanel, COMBOBOX_LOCATIONS, wxDefaultPosition, wxSize(300, -1));
    sizer->Add(combobox, 0, wxALIGN_CENTER | wxALL, 5);
    RefillLocations();

    //Buttons
    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(new wxButton(mainPanel, BUTTON_EDIT_LOCATIONS, "Edit destinations"));

    if (strlen(settings.last_pano) > 0) {
        buttonsSizer->AddSpacer(40);
        buttonsSizer->Add(new wxButton(mainPanel, BUTTON_LAST_LOCATION, "Go to last location"));
    }
    sizer->Add(buttonsSizer, 0, wxALIGN_CENTER | wxALL, 5);

    //TextCtrl user can paste in
    sizer->AddSpacer(50);
    sizer->Add(new wxTextCtrl(mainPanel, TEXTBOX_PASTE_LINK, "Or paste a Google StreetView link here...", wxDefaultPosition, wxSize(400, 50), wxTE_CENTER), 0, wxALIGN_CENTER);

    //Space below
    sizer->AddStretchSpacer(1);

    //Replace the (presumable) OpenGL canvas
    ReplaceSizer(sizer);
    glCanvas = NULL;

    //Event handlers
    Connect(COMBOBOX_LOCATIONS, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::OnSelectLocation));
    Connect(BUTTON_EDIT_LOCATIONS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnEditLocations));
    Connect(BUTTON_LAST_LOCATION, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnLastLocation));
    Connect(TEXTBOX_PASTE_LINK, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::OnLinkPaste));
}

/**
 * Delete the old contents and replace with the new
 * @param sizer
 */
void MainFrame::ReplaceSizer(wxSizer *sizer) {
    wxSizer *oldsizer = mainPanel->GetSizer();
    if (oldsizer != NULL) {
        oldsizer->Clear(true);
    }
    mainPanel->SetSizer(sizer);
    mainPanel->Layout();
}
