#include "MainFrame.h"
#include "GLCanvas.h"
#include "Panorama.h"
#include "EditLocationsDialog.h"
#include "Settings.h"
#include "PreferencesDialog.h"
#include "statustext.h"
#include <wx/dir.h>
#include <wx/filename.h>

using namespace std;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

const wxEventType myEVT_PANORAMA_START = wxNewEventType();
const wxEventType myEVT_SET_STATUS = wxNewEventType();

MainFrame* MainFrame::mainframe = NULL;

enum {
    COMBOBOX_LOCATIONS = wxID_HIGHEST,
    MENU_FILE,
    MENU_BACKTOMAIN,
    MENU_CLEARCACHE,
    BUTTON_EDIT_LOCATIONS,
    BUTTON_LAST_LOCATION,
    TEXTBOX_PASTE_LINK
};

/*
 * Recursively clean out the contents of dir.
 */
static bool
clear_dir(wxString path) {
    if (!wxDirExists(path)) {
        return false;
    }

    wxString glob = path + wxString::FromAscii("/*");

    /* Remove all files. */
    for (wxString fname = wxFindFirstFile(glob, wxFILE); !fname.empty(); fname = wxFindNextFile()) {
        if (!wxRemoveFile(fname))
            return false;
    }

    /* Remove all directories. */
    for (wxString fname = wxFindFirstFile(glob, wxDIR); !fname.empty(); fname = wxFindNextFile()) {
        if (!clear_dir(fname) || !wxRmdir(fname))
            return false;
    }
    return true;
}

MainFrame::MainFrame()
: wxFrame(NULL, wxID_ANY, wxString::FromAscii("StreetView Explorer"), wxDefaultPosition, wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT)),
isStartingWithPanorama(false) {

    mainframe = this;

#if __WXMSW__
    SetIcon(wxICON(aa_icon));
#endif

    //Set up menu
    wxMenu *fileMenu = new wxMenu();
    fileMenu->Append(MENU_BACKTOMAIN, wxString::FromAscii("&Go to main screen"));
    fileMenu->Append(MENU_CLEARCACHE, wxString::FromAscii("&Clear cache"));
    fileMenu->Append(wxID_PREFERENCES, wxString::FromAscii("&Preferences"));
    fileMenu->Append(wxID_ABOUT, wxString::FromAscii("&About..."));
    fileMenu->Append(wxID_EXIT, wxString::FromAscii("E&xit"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxString::FromAscii("&File"));
    SetMenuBar(menuBar);

    statusbar = new wxStatusBar(this);
    SetStatusBar(statusbar);

    mainPanel = new wxPanel(this);

    //Event handlers
    Connect(MENU_CLEARCACHE, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainFrame::OnClearCache));
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

void MainFrame::OnClearCache(wxMenuEvent &) {
    double sizeMB = wxDir::GetTotalSize(wxString::FromAscii("cache")).ToDouble() / 1024 / 1024;
    char question[200];
    sprintf(question, "Are you sure you want to delete %.2fMB of Street View cache images?", sizeMB);
    int answer = wxMessageBox(wxString::FromAscii(question), wxString::FromAscii("Clear cache"), wxYES_NO | wxCANCEL, this);

    //Recreate cache directory
    if (answer == wxYES) {
        if (clear_dir(wxString::FromAscii("cache")))
            wxMessageBox(wxString::FromAscii("Cache deleted"));
        else
            wxMessageBox(wxString::FromAscii("Failed to clear cache"));
    }
}

void MainFrame::OnSetStatus(wxCommandEvent &event) {
    statusbar->SetStatusText(event.GetString());
}

/**
 * Do the actual work callers expect ShowPanorama() to do. Replace the contents of the frame
 * with an OpenGL window with the actual game.
 */
void MainFrame::OnPanoramaStart(wxCommandEvent &event) {
    glCanvas = new GLCanvas(mainPanel, event.GetString().char_str());
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
void MainFrame::OnGoToMainScreen(wxMenuEvent &) {
    ShowMain();
}

/**
 * Open up a configuration panel through which users can adjust some parameters
 * @param
 */
void MainFrame::OnPreferences(wxMenuEvent& WXUNUSED(event)) {
    if(PreferencesDialog::preferencesVisible)
        return;

    if (glCanvas != NULL)
        glCanvas->disableCapture();

    PreferencesDialog *prefs = new PreferencesDialog(this);
    prefs->Show();
}

/**
 * Show information about me
 * @param
 */
void MainFrame::OnAbout(wxMenuEvent& WXUNUSED(event)) {
    wxMessageBox(wxString::FromAscii("This application is brought to you by Paul Wagener.\nhttp://code.google.com/p/streetview-explorer\nSpecial thanks to Ariane, Mr. Ropers and the helpful people at #wxwidgets"),
            wxString::FromAscii("About StreetView Explorer 1.0.1"),
            wxOK, this);
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
void MainFrame::OnSelectLocation(wxCommandEvent &) {
    const int selection = combobox->GetSelection();

    //Ignore first 'pick a destination' entry
    if (selection == 0)
        return;

    struct pano_location l = settings.locations[selection - 1];

    /**
     * I have no idea why I need this sleep, but without it some mysterious race condition
     * inside WxWidgets on the mac will pop up and crash the entire program.
     * All I know is that it has something to do with WxMouseEvents
     * If this is fixed in future versions of WxWidgets,
     * then please delete this code AS SOON AS POSSIBLE!
     */
    wxThread::Sleep(100);

    StartWithPanorama(l.pano_id);
}

/**
 * Show a modal dialog where the user can edit his favorite locations
 * @param event
 */
void MainFrame::OnEditLocations(wxCommandEvent &) {
    //Edit locations dialog
    EditLocationsDialog *editLocations = new EditLocationsDialog(this);
    editLocations->ShowModal();
    delete editLocations;

    //Resynchronize locations in combobox
    RefillLocations();
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

    int g = link.find(wxString::FromAscii("&panoid="));
    if (g == -1)
        return;

    wxString pasted_panoID = link.substr(g + strlen("&panoid="), PANOID_LENGTH);

    if (pasted_panoID.length() < PANOID_LENGTH)
        return;

    isStartingWithPanorama = true;
    StartWithPanorama(pasted_panoID.char_str());
}

/**
 * Synchronize the contents of the combobox with the actual locations in the settings
 */
void MainFrame::RefillLocations() {
    combobox->Clear();

    combobox->Append(wxString::FromAscii("<Pick a destination>"));
    for (unsigned int i = 0; i < settings.locations.size(); i++)
        combobox->Append(wxString::FromAscii(settings.locations[i].name));

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
    event.SetString(wxString::FromAscii(pano_id));
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
    titleSizer->Add(new wxStaticBitmap(mainPanel, wxID_ANY, wxBitmap(wxImage(title_png_is, wxBITMAP_TYPE_PNG))));
    titleSizer->AddStretchSpacer(1);
    sizer->Add(titleSizer, 0, wxEXPAND);


    //Some space between combobox and title
    sizer->AddSpacer(30);

    //Combobox
    combobox = new wxChoice(mainPanel, COMBOBOX_LOCATIONS, wxDefaultPosition, wxSize(300, -1));
    sizer->Add(combobox, 0, wxALIGN_CENTER | wxALL, 5);
    RefillLocations();
    combobox->SetSelection(0);

    //Buttons
    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(new wxButton(mainPanel, BUTTON_EDIT_LOCATIONS, wxString::FromAscii("Edit destinations")));

    sizer->Add(buttonsSizer, 0, wxALIGN_CENTER | wxALL, 5);

    //TextCtrl user can paste in
    sizer->AddSpacer(50);
    sizer->Add(new wxTextCtrl(mainPanel, TEXTBOX_PASTE_LINK, wxString::FromAscii("Or paste a Google StreetView link here..."), wxDefaultPosition, wxSize(400, 50), wxTE_CENTER), 0, wxALIGN_CENTER);

    //Space below
    sizer->AddStretchSpacer(1);

    //Replace the (presumable) OpenGL canvas
    ReplaceSizer(sizer);
    glCanvas = NULL;

    isStartingWithPanorama = false;

    //Event handlers
    Connect(COMBOBOX_LOCATIONS, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::OnSelectLocation));
    Connect(BUTTON_EDIT_LOCATIONS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnEditLocations));
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
