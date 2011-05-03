#include "EditLocationsDialog.h"
#include "Settings.h"

enum {
    WINDOW_EDITLOCATIONS = wxID_HIGHEST,
    LISTBOX_LOCATIONS,
    BUTTON_ADD,
    BUTTON_EDIT,
    BUTTON_REMOVE,
    BUTTON_UP,
    BUTTON_DOWN,
    TEXT_LINK_PASTE
};

/**
 * Create a new Edit Locations dialog. Edits directly the settings.locations vector
 * and saves it upon exit
 * @param frame
 */
EditLocationsDialog::EditLocationsDialog(wxWindow *frame)
: wxDialog(frame, WINDOW_EDITLOCATIONS, "Edit Locations", wxDefaultPosition, wxSize(600, 300), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
editing(false) {

    //GUI controls
    list_locations = new wxListCtrl(this, LISTBOX_LOCATIONS, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
    addButton = new wxButton(this, BUTTON_ADD, "Add location");
    editButton = new wxButton(this, BUTTON_EDIT, "Edit location");
    removeButton = new wxButton(this, BUTTON_REMOVE, "Remove location");
    upButton = new wxButton(this, BUTTON_UP, "Move Up");
    downButton = new wxButton(this, BUTTON_DOWN, "Move Down");

    //Sizer that divides GUI up in left listbox and right buttons
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

    //Listbox
    sizer->Add(list_locations, 1, wxALL | wxEXPAND, 5);

    //Buttons arranged vertically
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);
    buttonSizer->Add(addButton, 0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->Add(editButton, 0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->Add(removeButton, 0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->Add(upButton, 0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->Add(downButton, 0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->AddSpacer(50);
    buttonSizer->Add(new wxStaticText(this, wxID_ANY, "Double click on an item to edit the name", wxDefaultPosition, wxSize(100, 200), wxALIGN_CENTRE), 0, wxEXPAND);
    sizer->Add(buttonSizer, 0, wxALL | wxEXPAND, 5);

    this->SetSizer(sizer);

    //Event handlers
    list_locations->Connect(LISTBOX_LOCATIONS, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxCommandEventHandler(EditLocationsDialog::OnEditItem), NULL, this);
    list_locations->Connect(LISTBOX_LOCATIONS, wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(EditLocationsDialog::OnEditFinished), NULL, this);
    list_locations->Connect(LISTBOX_LOCATIONS, wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(EditLocationsDialog::OnSelectionChanges), NULL, this);
    list_locations->Connect(LISTBOX_LOCATIONS, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(EditLocationsDialog::OnSelectionChanges), NULL, this);

    Connect(BUTTON_ADD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditLocationsDialog::OnAdd));
    Connect(BUTTON_EDIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditLocationsDialog::OnEdit));
    Connect(BUTTON_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditLocationsDialog::OnRemove));
    Connect(BUTTON_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditLocationsDialog::OnUp));
    Connect(BUTTON_DOWN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditLocationsDialog::OnDown));

    Connect(WINDOW_EDITLOCATIONS, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EditLocationsDialog::OnCloseWindow));

    //ListCtrl columns
    wxListItem col0;
    col0.SetId(0);
    col0.SetText("Name");
    col0.SetWidth(300);
    list_locations->InsertColumn(0, col0);

    col0.SetId(1);
    col0.SetText("id");
    col0.SetWidth(200);
    list_locations->InsertColumn(1, col0);

    RefillList();
}

/**
 * Save the settings when the dialog is removed
 * (It is assumed that the caller deletes the dialog as it is closed
 */
EditLocationsDialog::~EditLocationsDialog() {
    settings.saveSettings();
}

/**
 * Update the button enabliness if the selected location changes
 * to prevent the up being on for the topmost and the down being on for the bottommost
 * @param event
 */
void EditLocationsDialog::OnSelectionChanges(wxCommandEvent &event) {
    UpdateButtons();
}

/**
 * Event handler for when the user wants to edit a name by activating an item
 * 
 * @param event
 */
void EditLocationsDialog::OnEditItem(wxCommandEvent &event) {
    list_locations->EditLabel(GetSelected());
    editing = true;
    UpdateButtons();
}

/**
 * Event handler for when the user finished editing a name
 * Copies the namestring given by the user back to the settings
 *
 * @param event
 */
void EditLocationsDialog::OnEditFinished(wxListEvent &event) {
    if(event.GetIndex() < 0 || event.GetIndex() >= (int)settings.locations.size())
        return;

    strncpy((char*) &settings.locations[event.GetIndex()].name, (const char*) event.GetLabel(), LOCATION_LENGTH+1);
    editing = false;
    UpdateButtons();
}

/**
 * Let the user paste a link and add it as a location
 * @param event
 */
void EditLocationsDialog::OnAdd(wxCommandEvent &event) {
    wxString link = OpenPasteDialog();

    //In case the user cancelled
    if (link.length() == 0)
        return;

    //Create new location
    struct pano_location l;
    strncpy((char*) &l.name, "Unnamed Location", LOCATION_LENGTH+1);
    strncpy((char*) &l.pano_id, (const char*) link, PANOID_LENGTH+1);
    settings.locations.push_back(l);

    RefillList();

    //Let user edit name immediately
    list_locations->EditLabel(settings.locations.size()-1);
    editing = true;
    UpdateButtons();
}

/**
 * User wants to edit the pano_id of the location
 *
 * @param event
 */
void EditLocationsDialog::OnEdit(wxCommandEvent &event) {
    wxString link = OpenPasteDialog();
    if (link.length() == 0)
        return;

    //Get the index of the selected item
    const unsigned int selected = GetSelected();

    if(selected < 0 || selected >= settings.locations.size())
        return;

    strncpy(settings.locations[selected].pano_id, (const char*) link, PANOID_LENGTH+1);

    RefillList(selected);
}

void EditLocationsDialog::OnRemove(wxCommandEvent &event) {
    settings.locations.erase(settings.locations.begin() + GetSelected());

    RefillList(GetSelected());
    UpdateButtons();
}

/**
 * Swap the selected item with the one above it
 * @param event
 */
void EditLocationsDialog::OnUp(wxCommandEvent &event) {
    unsigned int i = GetSelected();
    if(i < 0 || i >= settings.locations.size())
        return;

    struct pano_location temp = settings.locations[i - 1];
    settings.locations[i - 1] = settings.locations[i];
    settings.locations[i] = temp;

    RefillList(i - 1);
}

/**
 * Swap the selected item with the one below it
 * @param event
 */
void EditLocationsDialog::OnDown(wxCommandEvent &event) {
    unsigned int i = GetSelected();
    if(i < 0 || i >= settings.locations.size())
        return;

    struct pano_location temp = settings.locations[i + 1];
    settings.locations[i + 1] = settings.locations[i];
    settings.locations[i] = temp;

    RefillList(i + 1);
}

/**
 * Synchronize the list with items that are currently in the vector
 * @param selected
 */
void EditLocationsDialog::RefillList(unsigned int selected) {
    list_locations->DeleteAllItems();

    //Add items
    for (unsigned int n = 0; n < settings.locations.size(); n++) {
        wxListItem item;
        item.SetId(n);

        if (n == selected)
            item.SetState(wxLIST_STATE_SELECTED);

        list_locations->InsertItem(item);
        list_locations->SetItem(item); //Seems necessary for selected state to catch on

        //Fill in text
        list_locations->SetItem(n, 0, settings.locations[n].name);
        list_locations->SetItem(n, 1, settings.locations[n].pano_id);
    }
}

/**
 * Save the settings when the dialog is closed
 * @param event
 */
void EditLocationsDialog::OnCloseWindow(wxCloseEvent &event) {
    settings.saveSettings();
    event.Skip();
}

/**
 * Disable buttons if there is no item selected
 */
void EditLocationsDialog::UpdateButtons() {
    const int selectedItem = list_locations->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (selectedItem == -1 || editing) {
        //No item selected: disable buttons that need an item
        editButton->Enable(false);
        removeButton->Enable(false);
        upButton->Enable(false);
        downButton->Enable(false);
    } else {
        editButton->Enable();
        removeButton->Enable();

        //Also disable up / down if item is unable to move that way
        upButton->Enable(selectedItem > 0);
        downButton->Enable(selectedItem < (int)settings.locations.size() - 1);
    }
}

//These two global variables are only to be used for communication between the two below
//methods. Not sure how to do it another way yet without global variables...
wxString pasted_panoID;
wxDialog *pasteDialog;

/**
 * Open up a dialog in which the user can paste a streetview url
 * Method blocks until user pasted or cancelled, output
 */
wxString EditLocationsDialog::OpenPasteDialog() {
    //Default value for if the dialog fails
    pasted_panoID = wxString("");

    pasteDialog = new wxDialog(this, wxID_ANY, "Paste Google StreetView link", wxDefaultPosition, wxSize(500, 100));
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl *text_link = new wxTextCtrl(pasteDialog, TEXT_LINK_PASTE, "Paste link here");
    text_link->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditLocationsDialog::OnPasteLink), NULL, this);
    sizer->Add(text_link, 1, wxEXPAND | wxALL, 5);

    pasteDialog->SetSizer(sizer);
    pasteDialog->ShowModal(); //Method blocks until user pastes a correct link or cancels
    delete pasteDialog;

    return pasted_panoID;
}

/**
 * Parse the text for a link containing the panorama id.
 * Close the modal dialog if it is found
 * @param event
 */
void EditLocationsDialog::OnPasteLink(wxCommandEvent &event) {
    event.Skip();
    wxString link = event.GetString();

    int g = link.find("&panoid=");
    if (g == -1)
        return;

    wxString id = link.substr(g + strlen("&panoid="), PANOID_LENGTH);

    if(id.length() < PANOID_LENGTH)
        return;

    pasted_panoID = id; //Only to be used again in OpenPasteDialog
    
    pasteDialog->Close(); //Unblock OpenPasteDialog() ShowModal call
}

/**
 * Returns the index of the selected item, or -1 otherwise
 * @return
 */
int EditLocationsDialog::GetSelected() {
    return list_locations->GetNextItem(-1,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
}