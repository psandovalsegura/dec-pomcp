#include "experiment.h"
#include "boost/timer.hpp"

using namespace std;

EXPERIMENT::PARAMS::PARAMS()
:   NumRuns(1000),
    NumSteps(100000),
    SimSteps(1000),
    TimeOut(3600),
    MinDoubles(0),
    MaxDoubles(8), // Orignially 20
    TransformDoubles(-4),
    TransformAttempts(1000),
    Accuracy(0.01),
    UndiscountedHorizon(1000),
    AutoExploration(true)
{
}

EXPERIMENT::EXPERIMENT(const SIMULATOR& real,
    const SIMULATOR& simulator, const string& outputFile,
    EXPERIMENT::PARAMS& expParams, MCTS::PARAMS& searchParams)
:   Real(real),
    Simulator(simulator),
    OutputFile(outputFile.c_str()),
    ExpParams(expParams),
    SearchParams(searchParams)
{
    if (ExpParams.AutoExploration)
    {
        if (SearchParams.UseRave)
            SearchParams.ExplorationConstant = 0;
        else
            SearchParams.ExplorationConstant = simulator.GetRewardRange();
    }
    MCTS::InitFastUCB(SearchParams.ExplorationConstant);
}

DECEXPERIMENT::DECEXPERIMENT(const DECROCKSAMPLE& simulator,
    const string& outputFile,
    EXPERIMENT::PARAMS& expParams, MCTS::PARAMS& searchParams)
:   Real(simulator),
    Simulators(simulator.AgentSimulators),
    OutputFile(outputFile.c_str()),
    ExpParams(expParams),
    SearchParams(searchParams)
{
    if (ExpParams.AutoExploration)
    {
        if (SearchParams.UseRave)
            SearchParams.ExplorationConstant = 0;
        else
            SearchParams.ExplorationConstant = simulator.GetRewardRange();
    }
    MCTS::InitFastUCB(SearchParams.ExplorationConstant);
}

void EXPERIMENT::Run(int n)
{
    boost::timer timer;

    MCTS mcts(Simulator, SearchParams);

    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    bool outOfParticles = false;
    int t;

    STATE* state = Real.CreateStartState();
    if (SearchParams.Verbose >= 1)
    {
        cout << "/* In EXPERIMENT::Run " << n << " */" << endl;
        cout << "Initial State:" << endl;
        Real.DisplayState(*state, cout);
        cout << endl;
    }


    for (t = 0; t < ExpParams.NumSteps; t++)
    {
        int observation;
        double reward;
        int action = mcts.SelectAction();
        terminal = Real.Step(*state, action, observation, reward);

        Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();

        if (SearchParams.Verbose >= 1)
        {
            cout << "/* In Experiment::Run, Action, State, Observation, Reward */" << endl;
            Real.DisplayAction(action, cout);
            Real.DisplayState(*state, cout);
            Real.DisplayObservation(*state, observation, cout);
            Real.DisplayReward(reward, cout);
            cout << "/* End of Action, State, Observation, Reward */" << endl;
        }

        if (terminal)
        {
            if (SearchParams.Verbose >= 1)
            {
                cout << "Terminated" << endl;
            }

            break;
        }
        outOfParticles = !mcts.Update(action, observation, reward);
        if (outOfParticles)
            break;

        if (timer.elapsed() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }

    if (outOfParticles)
    {
        cout << "Out of particles, finishing episode with SelectRandom" << endl;
        HISTORY history = mcts.GetHistory();
        while (++t < ExpParams.NumSteps)
        {
            int observation;
            double reward;

            // This passes real state into simulator!
            // SelectRandom must only use fully observable state
            // to avoid "cheating"
            int action = Simulator.SelectRandom(*state, history, mcts.GetStatus());
            terminal = Real.Step(*state, action, observation, reward);

            Results.Reward.Add(reward);
            undiscountedReturn += reward;
            discountedReturn += reward * discount;
            discount *= Real.GetDiscount();

            if (SearchParams.Verbose >= 1)
            {
                Real.DisplayAction(action, cout);
                Real.DisplayState(*state, cout);
                Real.DisplayObservation(*state, observation, cout);
                Real.DisplayReward(reward, cout);
            }

            if (terminal)
            {
                if (SearchParams.Verbose >= 1)
                {
                    cout << "Terminated" << endl;
                }

                break;
            }

            history.Add(action, observation);
        }
    }

    Results.Time.Add(timer.elapsed());
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);

    // Print action/observation history every 500 runs
    if (SearchParams.Verbose >= 1 && n % 500 == 0) {
        cout << "True initial state: "; Real.DisplayState(*state, cout);
        cout << "Action/Observation History:" << endl;
        HISTORY history = mcts.GetHistory();
        history.Display(cout);
    }
    // cout << "Discounted return = " << discountedReturn
    //     << ", average = " << Results.DiscountedReturn.GetMean() << endl;
    // cout << "Undiscounted return = " << undiscountedReturn
    //     << ", average = " << Results.UndiscountedReturn.GetMean() << endl;
}

STATE* EXPERIMENT::StartSteppedSingleRun()
{
    STATE* state = Real.CreateStartState();
    MCTS* mcts = new MCTS(Simulator, SearchParams);
    steppedMCTS = mcts;
    steppedDiscount = 1.0;

    return state;
}

bool EXPERIMENT::StepSingleRun(STATE *state, int& action)
{
    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    bool terminal = false;
    bool outOfParticles = false;
    int t;

    int observation;
    double reward;
    action = steppedMCTS->SelectAction();
    terminal = Real.Step(*state, action, observation, reward);

    Results.Reward.Add(reward);
    undiscountedReturn += reward;
    discountedReturn += reward * steppedDiscount;
    steppedDiscount *= Real.GetDiscount();

    if (SearchParams.Verbose >= 1)
    {
        cout << "/* In Experiment::StepSingleRun, Action, Observation, Reward */" << endl;
        Real.DisplayAction(action, cout);
        Real.DisplayObservation(*state, observation, cout);
        Real.DisplayReward(reward, cout);
        cout << "/* End of Action, Observation, Reward */" << endl;
    }

    if (terminal)
    {
        Results.UndiscountedReturn.Add(undiscountedReturn);
        Results.DiscountedReturn.Add(discountedReturn);

        cout << "Discounted return = " << discountedReturn
            << ", average = " << Results.DiscountedReturn.GetMean() << endl;
        cout << "Undiscounted return = " << undiscountedReturn
            << ", average = " << Results.UndiscountedReturn.GetMean() << endl;
        return terminal;
    }

    outOfParticles = !steppedMCTS->Update(action, observation, reward);
    if (outOfParticles)
    {
        cout << "Out of particles, finishing episode with SelectRandom" << endl;
        HISTORY history = steppedMCTS->GetHistory();

        // This passes real state into simulator!
        // SelectRandom must only use fully observable state
        // to avoid "cheating"
        int action = Simulator.SelectRandom(*state, history, steppedMCTS->GetStatus());
        terminal = Real.Step(*state, action, observation, reward);
        if (SearchParams.Verbose >= 1)
        {
            cout << "/* In Experiment::StepSingleRun, Action, Observation, Reward */" << endl;
            Real.DisplayAction(action, cout);
            Real.DisplayObservation(*state, observation, cout);
            Real.DisplayReward(reward, cout);
            cout << "/* End of Action, Observation, Reward */" << endl;
        }

        Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * steppedDiscount;
        steppedDiscount *= Real.GetDiscount();

        history.Add(action, observation);
    }

    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);

    return false;
}

void EXPERIMENT::SingleRun()
{
    // Run experiment with parameters from initializaiton
    Run(0);

    // Display Results
    cout << "+----------------------------------------+" << endl;
    cout << "Simulations = " << SearchParams.NumSimulations << endl
        << "Runs = " << Results.Time.GetCount() << endl
        << "Undiscounted return = " << Results.UndiscountedReturn.GetMean()
        << " +- " << Results.UndiscountedReturn.GetStdErr() << endl
        << "Discounted return = " << Results.DiscountedReturn.GetMean()
        << " +- " << Results.DiscountedReturn.GetStdErr() << endl
        << "Time = " << Results.Time.GetMean() << endl;
}

void EXPERIMENT::MultiRun()
{
    cout << "Starting " << ExpParams.NumRuns << " runs with "
            << SearchParams.NumSimulations << " simulations... " << endl;
    for (int n = 0; n < ExpParams.NumRuns; n++)
    {
        Run(n);
        if (Results.Time.GetTotal() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << n << " runs in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }
}

void EXPERIMENT::DiscountedReturn()
{
    cout << "Main runs" << endl;
    OutputFile << "Simulations\tRuns\tUndiscounted return\tUndiscounted error\tDiscounted return\tDiscounted error\tTime\n";

    SearchParams.MaxDepth = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.SimSteps = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.NumSteps = Real.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);

    for (int i = ExpParams.MinDoubles; i <= ExpParams.MaxDoubles; i++)
    {
        SearchParams.NumSimulations = 1 << i;
        SearchParams.NumStartStates = 1 << i;
        if (i + ExpParams.TransformDoubles >= 0)
            SearchParams.NumTransforms = 1 << (i + ExpParams.TransformDoubles);
        else
            SearchParams.NumTransforms = 1;
        SearchParams.MaxAttempts = SearchParams.NumTransforms * ExpParams.TransformAttempts;

        Results.Clear();
        MultiRun();

        cout << "+----------------------------------------+" << endl;
        cout << "Simulations = " << SearchParams.NumSimulations << endl
            << "Runs = " << Results.Time.GetCount() << endl
            << "Undiscounted return = " << Results.UndiscountedReturn.GetMean()
            << " +- " << Results.UndiscountedReturn.GetStdErr() << endl
            << "Discounted return = " << Results.DiscountedReturn.GetMean()
            << " +- " << Results.DiscountedReturn.GetStdErr() << endl
            << "Time = " << Results.Time.GetMean() << endl;
        OutputFile << SearchParams.NumSimulations << "\t"
            << Results.Time.GetCount() << "\t"
            << Results.UndiscountedReturn.GetMean() << "\t"
            << Results.UndiscountedReturn.GetStdErr() << "\t"
            << Results.DiscountedReturn.GetMean() << "\t"
            << Results.DiscountedReturn.GetStdErr() << "\t"
            << Results.Time.GetMean() << endl;
    }
}

void EXPERIMENT::AverageReward()
{
    cout << "Main runs" << endl;
    OutputFile << "Simulations\tSteps\tAverage reward\tAverage time\n";

    SearchParams.MaxDepth = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.SimSteps = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);

    for (int i = ExpParams.MinDoubles; i <= ExpParams.MaxDoubles; i++)
    {
        SearchParams.NumSimulations = 1 << i;
        SearchParams.NumStartStates = 1 << i;
        if (i + ExpParams.TransformDoubles >= 0)
            SearchParams.NumTransforms = 1 << (i + ExpParams.TransformDoubles);
        else
            SearchParams.NumTransforms = 1;
        SearchParams.MaxAttempts = SearchParams.NumTransforms * ExpParams.TransformAttempts;

        Results.Clear();
        Run(i);

        cout << "Simulations = " << SearchParams.NumSimulations << endl
            << "Steps = " << Results.Reward.GetCount() << endl
            << "Average reward = " << Results.Reward.GetMean()
            << " +- " << Results.Reward.GetStdErr() << endl
            << "Average time = " << Results.Time.GetMean() / Results.Reward.GetCount() << endl;
        OutputFile << SearchParams.NumSimulations << "\t"
            << Results.Reward.GetCount() << "\t"
            << Results.Reward.GetMean() << "\t"
            << Results.Reward.GetStdErr() << "\t"
            << Results.Time.GetMean() / Results.Reward.GetCount() << endl;
    }
}

void DECEXPERIMENT::DecentralizedRun(int n)
{
    boost::timer timer;
    std::vector<MCTS*> mctsVec;

    for (int i = 0; i < Real.NumAgents; i++)
    {
        // Decentralized Simulators are assumed to contain
        // the AgentSimulators vector which provides an
        // individual agent's state of the world
        MCTS* mcts = new MCTS(Simulators[i], SearchParams);
        mctsVec.push_back(mcts);
    }

    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    std::vector<bool> agentsTerminalState;
    std::vector<bool> outOfParticlesVec;
    for (int i = 0; i < Real.NumAgents; i++)
    {
        agentsTerminalState.push_back(false);
        outOfParticlesVec.push_back(false);
    }
    int t;

    STATE* state = Real.CreateStartState();
    if (SearchParams.Verbose >= 1)
    {
        cout << "/* In DECEXPERIMENT::Run " << n << " */" << endl;
        cout << "Initial State:" << endl;
        Real.DisplayState(*state, cout);
        cout << endl;
    }

    for (t = 0; t < ExpParams.NumSteps; t++)
    {
        double reward;

        // Get joint action and observation
        std::vector<int> jointAction;
        std::vector<int> jointObservation;
        for (int i = 0; i < Real.NumAgents; i++)
        {
            MCTS* mcts = mctsVec[i];

            // If Agent i is out of particles, select
            // a SMART, LEGAL, or random action depending
            // on Simulator Knowledge
            if (outOfParticlesVec[i])
            {
                HISTORY history = mcts->GetHistory();

                // int observation;

                // This passes real state into simulator!
                // SelectRandom must only use fully observable state
                // to avoid "cheating"
                DECROCKSAMPLE_STATE& rockstate = safe_cast<DECROCKSAMPLE_STATE&>(*state);
                int action = Simulators[i].SelectRandom((rockstate.AgentStates[i]), history, mcts->GetStatus());
                // history.Add(action, observation); // TODO: update the history in MCTS tree
                jointAction.push_back(action);
                continue;
            }

            int action = mcts->SelectAction();
            jointAction.push_back(action);
        }

        agentsTerminalState = Real.Step(*state, agentsTerminalState, jointAction, jointObservation, reward);

        if (SearchParams.Verbose >= 1)
        {
            cout << "-----------------" << endl;
            cout << "Joint action:" << endl;
            Real.DisplayJointAction(jointAction, cout);
            cout << "Joint observation:" << endl;
            Real.DisplayJointObservation(jointObservation, cout);
            cout << "Reward: " << reward << endl;
            cout << "New state:" << endl;
            Real.DisplayState(*state, cout);
            cout << endl;
        }

        Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();

        // Check if all agents have reached a terminal state
        bool allTerminal = true;
        for (int i = 0; i < Real.NumAgents; i++)
        {
            allTerminal &= agentsTerminalState[i];
        }

        if (allTerminal)
        {
            if (SearchParams.Verbose >= 1)
            {
                cout << "All agents terminated" << endl;
            }

            break;
        }

        for (int i = 0; i < Real.NumAgents; i++)
        {
            MCTS* mcts = mctsVec[i];
            outOfParticlesVec[i] = !mcts->Update(jointAction[i], jointObservation[i], reward);
        }

        if (timer.elapsed() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }

    Results.Time.Add(timer.elapsed());
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);

    // cout << "Discounted return = " << discountedReturn
    //     << ", average = " << Results.DiscountedReturn.GetMean() << endl;
    // cout << "Undiscounted return = " << undiscountedReturn
    //     << ", average = " << Results.UndiscountedReturn.GetMean() << endl;
}

void DECEXPERIMENT::MultiDecentralizedRun()
{
    cout << "Starting " << ExpParams.NumRuns << " runs with "
            << SearchParams.NumSimulations << " simulations... " << endl;
    for (int n = 0; n < ExpParams.NumRuns; n++)
    {
        DecentralizedRun(n);
        if (Results.Time.GetTotal() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << n << " runs in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }
}

void DECEXPERIMENT::DiscountedReturn()
{
    cout << "Main runs" << endl;
    OutputFile << "Simulations\tRuns\tUndiscounted return\tUndiscounted error\tDiscounted return\tDiscounted error\tTime\n";

    SearchParams.MaxDepth = Simulators[0].GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.SimSteps = Simulators[0].GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.NumSteps = Real.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);

    for (int i = ExpParams.MinDoubles; i <= ExpParams.MaxDoubles; i++)
    {
        SearchParams.NumSimulations = 1 << i;
        SearchParams.NumStartStates = 1 << i;
        if (i + ExpParams.TransformDoubles >= 0)
            SearchParams.NumTransforms = 1 << (i + ExpParams.TransformDoubles);
        else
            SearchParams.NumTransforms = 1;
        SearchParams.MaxAttempts = SearchParams.NumTransforms * ExpParams.TransformAttempts;

        Results.Clear();
        MultiDecentralizedRun();

        cout << "+----------------------------------------+" << endl;
        cout << "Simulations = " << SearchParams.NumSimulations << endl
            << "Runs = " << Results.Time.GetCount() << endl
            << "Undiscounted return = " << Results.UndiscountedReturn.GetMean()
            << " +- " << Results.UndiscountedReturn.GetStdErr() << endl
            << "Discounted return = " << Results.DiscountedReturn.GetMean()
            << " +- " << Results.DiscountedReturn.GetStdErr() << endl
            << "Time = " << Results.Time.GetMean() << endl;
        OutputFile << SearchParams.NumSimulations << "\t"
            << Results.Time.GetCount() << "\t"
            << Results.UndiscountedReturn.GetMean() << "\t"
            << Results.UndiscountedReturn.GetStdErr() << "\t"
            << Results.DiscountedReturn.GetMean() << "\t"
            << Results.DiscountedReturn.GetStdErr() << "\t"
            << Results.Time.GetMean() << endl;
    }
}

void EXPERIMENT::DisplayParameters()
{
    cout << "-----------------------" << endl;
    cout << "Experiment Parameters:" << endl;
    cout << "NumRuns=" << ExpParams.NumRuns << endl;
    cout << "NumSteps=" << ExpParams.NumSteps << endl;
    cout << "SimSteps=" << ExpParams.SimSteps << endl;
    cout << "TimeOut=" << ExpParams.TimeOut << endl;
    cout << "MinDoubles=" << ExpParams.MinDoubles << endl;
    cout << "MaxDoubles=" << ExpParams.MaxDoubles << endl;
    cout << "TransformDoubles=" << ExpParams.TransformDoubles << endl;
    cout << "TransformAttempts=" << ExpParams.TransformAttempts << endl;
    cout << "Accuracy=" << ExpParams.Accuracy << endl;
    cout << "UndiscountedHorizon=" << ExpParams.UndiscountedHorizon << endl;
    cout << "AutoExploration=" << ExpParams.AutoExploration << endl;
    cout << "-----------------------" << endl;
    cout << "Search Parameters:" << endl;
    cout << "Verbose=" << SearchParams.Verbose << endl;
    cout << "MaxDepth=" << SearchParams.MaxDepth << endl;
    cout << "NumSimulations=" << SearchParams.NumSimulations << endl;
    cout << "NumStartStates=" << SearchParams.NumStartStates << endl;
    cout << "UseTransforms=" << SearchParams.UseTransforms << endl;
    cout << "MaxAttempts=" << SearchParams.MaxAttempts << endl;
    cout << "ExpandCount=" << SearchParams.ExpandCount << endl;
    cout << "ExplorationConstant=" << SearchParams.ExplorationConstant << endl;
    cout << "UseRave=" << SearchParams.UseRave << endl;
    cout << "RaveDiscount=" << SearchParams.RaveDiscount << endl;
    cout << "RaveConstant=" << SearchParams.RaveConstant << endl;
    cout << "DisableTree=" << SearchParams.DisableTree << endl;
    cout << "-----------------------" << endl;
}

//----------------------------------------------------------------------------
