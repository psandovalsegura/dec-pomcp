#include "battleship.h"
#include "mcts.h"
#include "network.h"
#include "pocman.h"
#include "rocksample.h"
#include "decrocksample.h"
#include "tag.h"
#include "dectiger.h"
#include "experiment.h"
#include <boost/program_options.hpp>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/cmdline.h>
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

using namespace std;
using namespace boost::program_options;

void UnitTests()
{
    cout << "Testing UTILS" << endl;
    UTILS::UnitTest();
    cout << "Testing COORD" << endl;
    COORD::UnitTest();
    cout << "Testing MCTS" << endl;
    MCTS::UnitTest();
}

void disableBufferedIO(void)
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setbuf(stderr, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

// int main(int argc, char* argv[])
// {
//     MCTS::PARAMS searchParams;
//     EXPERIMENT::PARAMS expParams;
//     SIMULATOR::KNOWLEDGE knowledge;
//     string problem, outputfile, policy;
//     int size, number, treeknowledge = 1, rolloutknowledge = 1, smarttreecount = 10;
//     double smarttreevalue = 1.0;
//
//     options_description desc("Allowed options");
//     desc.add_options()
//         ("help", "produce help message")
//         ("test", "run unit tests")
//         ("testrun", "run a single instance using a reasonable number of rollouts")
//         ("problem", value<string>(&problem), "problem to run")
//         ("outputfile", value<string>(&outputfile)->default_value("output.txt"), "summary output file")
//         ("policy", value<string>(&policy), "policy file (explicit POMDPs only)")
//         ("size", value<int>(&size), "size of problem (problem specific)")
//         ("number", value<int>(&number), "number of elements in problem (problem specific)")
//         ("timeout", value<double>(&expParams.TimeOut), "timeout (seconds)")
//         ("mindoubles", value<int>(&expParams.MinDoubles), "minimum power of two simulations")
//         ("maxdoubles", value<int>(&expParams.MaxDoubles), "maximum power of two simulations")
//         ("runs", value<int>(&expParams.NumRuns), "number of runs")
//         ("accuracy", value<double>(&expParams.Accuracy), "accuracy level used to determine horizon")
//         ("horizon", value<int>(&expParams.UndiscountedHorizon), "horizon to use when not discounting")
//         ("num steps", value<int>(&expParams.NumSteps), "number of steps to run when using average reward")
//         ("verbose", value<int>(&searchParams.Verbose), "verbosity level")
//         ("autoexploration", value<bool>(&expParams.AutoExploration), "Automatically assign UCB exploration constant")
//         ("exploration", value<double>(&searchParams.ExplorationConstant), "Manual value for UCB exploration constant")
//         ("usetransforms", value<bool>(&searchParams.UseTransforms), "Use transforms")
//         ("transformdoubles", value<int>(&expParams.TransformDoubles), "Relative power of two for transforms compared to simulations")
//         ("transformattempts", value<int>(&expParams.TransformAttempts), "Number of attempts for each transform")
//         ("userave", value<bool>(&searchParams.UseRave), "RAVE")
//         ("ravediscount", value<double>(&searchParams.RaveDiscount), "RAVE discount factor")
//         ("raveconstant", value<double>(&searchParams.RaveConstant), "RAVE bias constant")
//         ("treeknowledge", value<int>(&knowledge.TreeLevel), "Knowledge level in tree (0=Pure, 1=Legal, 2=Smart)")
//         ("rolloutknowledge", value<int>(&knowledge.RolloutLevel), "Knowledge level in rollouts (0=Pure, 1=Legal, 2=Smart)")
//         ("smarttreecount", value<int>(&knowledge.SmartTreeCount), "Prior count for preferred actions during smart tree search")
//         ("smarttreevalue", value<double>(&knowledge.SmartTreeValue), "Prior value for preferred actions during smart tree search")
//         ("disabletree", value<bool>(&searchParams.DisableTree), "Use 1-ply rollout action selection")
//         ;
//
//     variables_map vm;
//     store(parse_command_line(argc, argv, desc), vm);
//     notify(vm);
//
//     if (vm.count("help"))
//     {
//         cout << desc << "\n";
//         return 1;
//     }
//
//     if (vm.count("problem") == 0)
//     {
//         cout << "No problem specified" << endl;
//         return 1;
//     }
//
//     if (vm.count("test"))
//     {
//         cout << "Running unit tests" << endl;
//         UnitTests();
//         return 0;
//     }
//
//     SIMULATOR* real = 0;
//     SIMULATOR* simulator = 0;
//
//     if (problem == "battleship")
//     {
//         real = new BATTLESHIP(size, size, number);
//         simulator = new BATTLESHIP(size, size, number);
//     }
//     else if (problem == "pocman")
//     {
//         real = new FULL_POCMAN;
//         simulator = new FULL_POCMAN;
//     }
//     else if (problem == "network")
//     {
//         real = new NETWORK(size, number);
//         simulator = new NETWORK(size, number);
//     }
//     else if (problem == "rocksample")
//     {
//         real = new ROCKSAMPLE(size, number);
//         simulator = new ROCKSAMPLE(size, number);
//     }
//     else if (problem == "decrocksample")
//     {
//         DECROCKSAMPLE *real = new DECROCKSAMPLE(7,8,2);
//         DECEXPERIMENT decexperiment(*real, outputfile, expParams, searchParams);
//         decexperiment.DecentralizedRun(0);
//         exit(1);
//     }
//     else if (problem == "tag")
//     {
//         real = new TAG(number);
//         simulator = new TAG(number);
//     }
//     else if (problem == "dectiger") {
//         real = new DECTIGER(0);
//         simulator = new DECTIGER(0);
//     }
//     else
//     {
//         cout << "Unknown problem" << endl;
//         exit(1);
//     }
//
//     if (vm.count("testrun"))
//     {
//         simulator->SetKnowledge(knowledge);
//
//         // Configure test run
//         expParams.NumRuns = 1;
//         expParams.NumSteps = 1000;
//
//         searchParams.NumSimulations = 1000000;
//
//         EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
//         experiment.DisplayParameters();
//         experiment.TestRun();
//
//         return 0;
//     }
//
//     simulator->SetKnowledge(knowledge);
//     EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
//     experiment.DiscountedReturn();
//
//     delete real;
//     delete simulator;
//     return 0;
// }

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
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnRun();
    void ProcessArgs();

private:
    wxString Problem;
    int Number;
    int Size;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

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
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    ProcessArgs();

    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    // if ( !wxApp::OnInit() )
    //     return false;

    // create the main application window
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    MyFrame *frame = new MyFrame("Minimal wxWidgets App");
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

void MyApp::ProcessArgs()
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },

        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "s", "size",    "size of problem (problem specific)", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "n", "number",  "number of elements in problem (problem specific)", wxCMD_LINE_VAL_NUMBER },

        { wxCMD_LINE_PARAM,  NULL, NULL, "problem", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser (cmdLineDesc, argc, argv);
    parser.Parse();

    // That’s it! Now do stuff with the args:

    cout << endl << "Number of params: " << parser.GetParamCount() << endl;
    cout << "Param 1: " << parser.GetParam(0).mb_str() << endl;
    cout << "Param 2: " << parser.GetParam(1).mb_str() << endl;
    cout << "Param 3: " << parser.GetParam(2).mb_str() << endl;
    cout << "Switch v: " << parser.Found( wxT("v") ) << endl;

    long size = 0;
    long number = 0;
    if (parser.Found(wxT("s"), &size) && parser.Found(wxT("n"), &number))
    {
        Size = int(size);
        Number = int(number);
    }
    Problem = parser.GetParam(0); // Problem is the first argument
}

int MyApp::OnRun()
{
    MCTS::PARAMS searchParams;
    EXPERIMENT::PARAMS expParams;
    SIMULATOR::KNOWLEDGE knowledge;
    string problem, policy;
    string outputfile = "output.txt";
    int size, number, treeknowledge = 1, rolloutknowledge = 1, smarttreecount = 10;
    double smarttreevalue = 1.0;

    if (Problem.IsNull())
    {
        cout << "Error: No problem specified" << endl;
        return 1;
    }

    SIMULATOR* real = 0;
    SIMULATOR* simulator = 0;

    if (Problem == wxT("test"))
    {
        real = new ROCKSAMPLE(Size, Number);
        simulator = new ROCKSAMPLE(Size, Number);
        simulator->SetKnowledge(knowledge);

        // Configure test run
        expParams.NumRuns = 1;
        expParams.NumSteps = 1000;

        searchParams.NumSimulations = 1000000;

        EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
        experiment.DisplayParameters();
        experiment.TestRun();

        return 0;
    }
    else if (Problem == wxT("rocksample"))
    {
        real = new ROCKSAMPLE(Size, Number);
        simulator = new ROCKSAMPLE(Size, Number);
    }

    simulator->SetKnowledge(knowledge);
    EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
    experiment.DiscountedReturn();

    delete real;
    delete simulator;
    return 0;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

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
    SetStatusText("Welcome to wxWidgets, Pedro!");
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the minimal wxWidgets sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}
