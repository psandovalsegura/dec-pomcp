#include "decrocksample.h"
#include "utils.h"

using namespace std;
using namespace UTILS;

DECROCKSAMPLE::DECROCKSAMPLE(int size, int rocks, int numAgents)
:   Grid(size, size),
    Size(size),
    NumRocks(rocks),
    NumAgents(numAgents)
{
    NumActions = NumRocks + 5;
    NumObservations = 3;
    RewardRange = 20;
    Discount = 1; // Was 0.95

    if (size == 7 && rocks == 8 && numAgents == 2)
        Init_7_8_2();
    else if (size == 11 && rocks == 11 && numAgents == 5)
        Init_11_11_5();
    else
        InitGeneral();
}

void DECROCKSAMPLE::InitGeneral()
{
    for (int i = 0; i < NumAgents; i++)
    {
        StartPos.push_back(COORD(i, Size / 2));
    }
    RandomSeed(0);
    Grid.SetAllValues(-1);
    for (int i = 0; i < NumRocks; ++i)
    {
        COORD pos;
        do
        {
            pos = COORD(Random(Size), Random(Size));
        }
        while (Grid(pos) >= 0);
        Grid(pos) = i;
        RockPos.push_back(pos);
    }

    for (int i = 0; i < NumAgents; ++i)
    {
        ROCKSAMPLE *agent = new ROCKSAMPLE(Size, NumRocks, StartPos[i], RockPos);
        AgentSimulators.push_back(*agent);
    }
}

void DECROCKSAMPLE::Init_7_8_2()
{
    cout << "Using special layout for decrocksample(7, 8, 2)" << endl;

    COORD rocks[] =
    {
        COORD(2, 0),
        COORD(0, 1),
        COORD(3, 1),
        COORD(6, 3),
        COORD(2, 4),
        COORD(3, 4),
        COORD(5, 5),
        COORD(1, 6)
    };

    COORD agentPos[] =
    {
        COORD(0, 3),
        COORD(4, 4)
    };

    Grid.SetAllValues(-1);
    for (int i = 0; i < NumRocks; ++i)
    {
        Grid(rocks[i]) = i;
        RockPos.push_back(rocks[i]);
    }

    for (int i = 0; i < NumAgents; ++i)
    {
        StartPos.push_back(agentPos[i]);
        ROCKSAMPLE *agent = new ROCKSAMPLE(Size, NumRocks, agentPos[i], RockPos);
        AgentSimulators.push_back(*agent);
    }
}

void DECROCKSAMPLE::Init_11_11_5()
{
    cout << "Using special layout for decrocksample(11, 11, 5)" << endl;

    COORD rocks[] =
    {
        COORD(0, 3),
        COORD(0, 7),
        COORD(1, 8),
        COORD(2, 4),
        COORD(3, 3),
        COORD(3, 8),
        COORD(4, 3),
        COORD(5, 8),
        COORD(6, 1),
        COORD(9, 3),
        COORD(9, 9)
    };

    COORD agentPos[] =
    {
        COORD(0, 0),
        COORD(2, 3),
        COORD(4, 4),
        COORD(6, 6),
        COORD(8, 8)
    };

    Grid.SetAllValues(-1);
    for (int i = 0; i < NumRocks; ++i)
    {
        Grid(rocks[i]) = i;
        RockPos.push_back(rocks[i]);
    }

    for (int i = 0; i < NumAgents; ++i)
    {
        StartPos.push_back(agentPos[i]);
        ROCKSAMPLE *agent = new ROCKSAMPLE(Size, NumRocks, agentPos[i], RockPos);
        AgentSimulators.push_back(*agent);
    }
}


STATE* DECROCKSAMPLE::Copy(const STATE& state) const
{
    const DECROCKSAMPLE_STATE& rockstate = safe_cast<const DECROCKSAMPLE_STATE&>(state);
    DECROCKSAMPLE_STATE* newstate = MemoryPool.Allocate();
    *newstate = rockstate;
    return newstate;
}

void DECROCKSAMPLE::Validate(const STATE& state) const
{
    const DECROCKSAMPLE_STATE& rockstate = safe_cast<const DECROCKSAMPLE_STATE&>(state);
    for (int i = 0; i < NumAgents; i++)
    {
        assert(Grid.Inside(rockstate.AgentPos[i]));
    }
}

STATE* DECROCKSAMPLE::CreateStartState() const
{
    DECROCKSAMPLE_STATE* rockstate = MemoryPool.Allocate();
    for (int i = 0; i < NumAgents; i++)
    {
        ROCKSAMPLE agentSimulator = AgentSimulators[i];
        ROCKSAMPLE_STATE agentState = safe_cast<ROCKSAMPLE_STATE&>(*(agentSimulator.CreateStartState()));
        rockstate->AgentStates.push_back(agentState);
    }

    for (int i = 0; i < NumRocks; i++)
    {
        DECROCKSAMPLE_STATE::ENTRY entry;
        entry.Collected = false;
        entry.Valuable = Bernoulli(0.5);
        rockstate->Rocks.push_back(entry);
    }
    return rockstate;
}

void DECROCKSAMPLE::FreeState(STATE* state) const
{
    DECROCKSAMPLE_STATE* rockstate = safe_cast<DECROCKSAMPLE_STATE*>(state);
    MemoryPool.Free(rockstate);
}

bool DECROCKSAMPLE::Step(STATE& state, int action,
    int& observation, double& reward) const
{
    // Placeholder to continue to subclass from SIMULATOR
    // Decentralized problems should really subclass from a
    // different decentralized simulator in the future
    return true;
}

std::vector<bool> DECROCKSAMPLE::Step(STATE& state, std::vector<bool> terminalStates, std::vector<int> jointAction,
    std::vector<int>& jointObservation, double& reward) const
{
    DECROCKSAMPLE_STATE& rockstate = safe_cast<DECROCKSAMPLE_STATE&>(state);
    std::vector<ROCKSAMPLE_STATE> newAgentStates;
    std::vector<bool> newTerminalStates;
    reward = 0;

    for (int i = 0; i < NumAgents; i++)
    {
        ROCKSAMPLE agentSimulator = AgentSimulators[i];
        int action = jointAction[i];        // Action chosen by Agent i
        bool terminal = terminalStates[i];  // Whether Agent i has reached a terminal state
        int observation = E_NONE;           // To be passed by ref into Step
        double currentReward = 0;           // To be passed by ref into Step

        ROCKSAMPLE_STATE agentState = rockstate.AgentStates[i];
        if (!terminal)
        {
            // Only step Agent i forward if not in a terminal state
            terminal = agentSimulator.Step(agentState, action, observation, currentReward);
        }

        newTerminalStates.push_back(terminal);
        jointObservation.push_back(observation);
        newAgentStates.push_back(agentState);
        reward += currentReward;
        assert(currentReward != -100);
    }

    rockstate.AgentStates = newAgentStates;
    return newTerminalStates;
}

void DECROCKSAMPLE::DisplayBeliefs(const BELIEF_STATE& beliefState,
    std::ostream& ostr) const
{
}

void DECROCKSAMPLE::DisplayState(const STATE& state, std::ostream& ostr) const
{
    const DECROCKSAMPLE_STATE& rockstate = safe_cast<const DECROCKSAMPLE_STATE&>(state);
    ostr << endl;
    for (int x = 0; x < Size + 2; x++)
        ostr << "# ";
    ostr << endl;
    for (int y = Size - 1; y >= 0; y--)
    {
        ostr << "# ";
        for (int x = 0; x < Size; x++)
        {
            COORD pos(x, y);
            int rock = Grid(pos);
            const DECROCKSAMPLE_STATE::ENTRY& entry = rockstate.Rocks[rock];

            bool displayAgent = false;
            for (int i = 0; i < NumAgents; i++)
            {
                COORD agentPos = rockstate.AgentStates[i].AgentPos;
                if (agentPos == COORD(x, y))
                {
                    displayAgent = true;
                }
            }

            if (displayAgent)
                ostr << "A ";
            else if (rock >= 0 && !entry.Collected)
                ostr << rock << (entry.Valuable ? "$" : "X");
            else
                ostr << ". ";
        }
        ostr << "#" << endl;
    }
    for (int x = 0; x < Size + 2; x++)
        ostr << "# ";
    ostr << endl;
}


void DECROCKSAMPLE::DisplayJointObservation(std::vector<int> jointObservation, std::ostream& ostr) const
{
    for (int i = 0; i < jointObservation.size(); i++)
    {
        ostr << "Agent " << i << ": ";
        int observation = jointObservation[i];
        switch (observation)
        {
        case E_NONE:
            ostr << "None" << endl;
            break;
        case E_GOOD:
            ostr << "Observed good" << endl;
            break;
        case E_BAD:
            ostr << "Observed bad" << endl;
            break;
        }
    }
    ostr << endl;
}

void DECROCKSAMPLE::DisplayJointAction(std::vector<int> jointAction, std::ostream& ostr) const
{
    for (int i = 0; i < jointAction.size(); i++)
    {
        ostr << "Agent " << i << ": ";
        int action = jointAction[i];
        if (action < E_SAMPLE)
            ostr << COORD::CompassString[action] << endl;
        if (action == E_SAMPLE)
            ostr << "Sample" << endl;
        if (action > E_SAMPLE)
            ostr << "Check " << action - E_SAMPLE - 1 << endl;
    }
    ostr << endl;
}
