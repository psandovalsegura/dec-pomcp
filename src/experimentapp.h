#ifndef EXPERIMENTAPP_H
#define EXPERIMENTAPP_H

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/cmdline.h>
#include "wx/wxprec.h"
#include "wx/grid.h"
#include "experiment.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

using namespace std;

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class ExperimentApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnRun();
    EXPERIMENT *experiment;
    STATE *state;
    bool terminated;
    std::vector<COORD> rockPos;
};

// Define a new frame type: this is going to be our main frame
class wxGrid;
class StateFrame : public wxFrame
{
public:
    StateFrame(const wxString& title, wxSize size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStartExperiment(wxCommandEvent& event);
    void OnStepButton(wxCommandEvent& event);
    void DisplayState(STATE* state);
    void DisplayStateAction(STATE* state, int action);
    void SetCellValue(int x, int y, const wxString& s);
    void SetStatusBarText(const char *s);

    wxGrid *grid;
    wxButton *stepButton;
    ExperimentApp *delegate;

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,
    Minimal_StartExperiment = wxID_SYSTEM_MENU,
    Minimal_StepButton = wxID_HIGHEST + 1
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(StateFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  StateFrame::OnQuit)
    EVT_MENU(Minimal_About, StateFrame::OnAbout)
    EVT_MENU(Minimal_StartExperiment, StateFrame::OnStartExperiment)
    EVT_MENU(Minimal_StepButton, StateFrame::OnStepButton)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. ExperimentApp and
// not wxApp)
IMPLEMENT_APP_NO_MAIN(ExperimentApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool ExperimentApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    // if ( !wxApp::OnInit() )
    //     return false;

    // create the main application window
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    StateFrame *frame = new StateFrame("POMCP/Dec-POMCP Simulator", wxSize(600,600));
    frame->delegate = this;
    frame->Show(true);
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

int ExperimentApp::OnRun()
{
    wxApp::OnRun();
    return 0;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
StateFrame::StateFrame(const wxString& title, wxSize size)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, size)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");
    fileMenu->Append(Minimal_StartExperiment, "Start Experiment", "Begin running the experiment");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Start an experiment within the File menu");
#endif // wxUSE_STATUSBAR

    int gridDimension = 7;
    int cellSize = 60;
    grid = new wxGrid(this, wxID_ANY, wxPoint(0,0), wxSize(gridDimension*cellSize + 1.5 * cellSize, gridDimension*cellSize + cellSize));
    grid->CreateGrid(0, 0);
    grid->AppendRows(gridDimension);
    grid->AppendCols(gridDimension);

    for (int i = 0; i < gridDimension; i++)
    {
        grid->SetRowSize(i, cellSize);
        grid->SetColSize(i, cellSize);
    }

    // Step button
    stepButton = new wxButton(this, Minimal_StepButton, _T("Step"), wxPoint(10, 480), wxSize(60,30), 0);
    stepButton->Bind(wxEVT_BUTTON, &StateFrame::OnStepButton, this);
}

// event handlers

void StateFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void StateFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (  "\n"
                    "This is the POMCP/Dec-POMCP Simulator\n"
                    "built by Pedro Sandoval Segura\n"
                    "running under %s with %s.",
                    wxGetOsDescription(),
                    wxVERSION_STRING
                 ),
                 "About POMCP/Dec-POMCP Simulator",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void StateFrame::DisplayState(STATE* state)
{
    grid->ClearGrid();
    ROCKSAMPLE_STATE* rockstate = safe_cast<ROCKSAMPLE_STATE*>(state);

    // Draw agent
    int gridDimension = grid->GetNumberRows();
    int agentRow = gridDimension - 1 - rockstate->AgentPos.Y; // See later note on axis
    int agentCol = rockstate->AgentPos.X;
    grid->SetCellTextColour(agentRow, agentCol, *wxRED);
    SetCellValue(agentRow, agentCol, wxT("A"));

    // Draw rocks
    std::vector<COORD> rockPos = (this->delegate->rockPos);
    int numRocks = rockstate->Rocks.size();
    for (int i = 0; i < numRocks; i++)
    {
        COORD pos = rockPos[i];
        const ROCKSAMPLE_STATE::ENTRY& entry = rockstate->Rocks[i];
        if (!entry.Collected)
        {
            // rockPos has a weird axis definition
            // (defined in ROCKSAMPLE's DisplayState),
            // so we have to correct for it here
            int row = gridDimension - 1 - pos.Y;
            int col = pos.X;

            SetCellValue(row, col, wxString::Format(wxT("%i"), i));
            if (entry.Valuable)
            {
                grid->SetCellTextColour(row, col, *wxGREEN);
            } else {
                grid->SetCellTextColour(row, col, *wxBLACK);
            }

            // Check if the agent is in the cell with a rock
            if (row == agentRow && col == agentCol)
            {
                grid->SetCellBackgroundColour(row, col, *wxRED);
            } else {
                grid->SetCellBackgroundColour(row, col, *wxWHITE);
            }
        }
    }
}

void StateFrame::DisplayStateAction(STATE* state, int action)
{
    DisplayState(state);

    enum
    {
        E_SAMPLE = 4
    };

    ROCKSAMPLE_STATE* rockstate = safe_cast<ROCKSAMPLE_STATE*>(state);
    int gridDimension = grid->GetNumberRows();


    if (action == E_SAMPLE)
    {
        // Highlight cell which agent is sampling
        int agentRow = gridDimension - 1 - rockstate->AgentPos.Y; // See DisplayState
        int agentCol = rockstate->AgentPos.X;
        grid->SetCellBackgroundColour(agentRow, agentCol, *wxLIGHT_GREY);
    }

    if (action > E_SAMPLE)
    {
        // Highlight cell that is being checked
        int rockIndex = action - E_SAMPLE - 1;
        std::vector<COORD> rockPos = (this->delegate->rockPos);
        COORD pos = rockPos[rockIndex];
        int row = gridDimension - 1 - pos.Y;
        int col = pos.X;
        grid->SetCellBackgroundColour(row, col, *wxCYAN);
    }
}

void StateFrame::SetCellValue(int x, int y, const wxString& s)
{
    grid->SetCellFont(x, y, wxFont(wxFontInfo(15).Bold()));
    grid->SetCellValue(x, y, s);
    grid->SetCellAlignment(x, y, wxALIGN_CENTRE, wxALIGN_CENTRE);
}

void StateFrame::SetStatusBarText(const char *s)
{
    #if wxUSE_STATUSBAR
        SetStatusText(s);
    #endif // wxUSE_STATUSBAR
}

void StateFrame::OnStartExperiment(wxCommandEvent& WXUNUSED(event))
{
    this->delegate->terminated = false;
    this->delegate->state = this->delegate->experiment->StartSteppedSingleRun();

    // Display initial state on UI
    DisplayState(this->delegate->state);
}

void StateFrame::OnStepButton(wxCommandEvent& WXUNUSED(event))
{
    // Check if experiment has not started
    if (this->delegate->state == NULL)
    {
        SetStatusBarText("Cannot step because experiment has not started");
        return;
    }

    if (this->delegate->terminated == true)
    {
        SetStatusBarText("Rollout has terminated. Begin a new one!");
        return;
    }

    SetStatusBarText("Stepped");
    int action;
    bool terminal = this->delegate->experiment->StepSingleRun(this->delegate->state, action);

    // Display new state on UI
    DisplayStateAction(this->delegate->state, action);

    if (terminal)
    {
        this->delegate->terminated = true;
        SetStatusBarText("Rollout reached terminal state");
    }
}

#endif // EXPERIMENTAPP
