#include "dectiger.h"
#include "utils.h"

using namespace std;
using namespace UTILS;

DECTIGER::DECTIGER(int numMachines)
:   ObsProb(0.85)
{
	NumActions = 3;
    NumObservations = 2;
    RewardRange = 101;
    Discount = 0.95;
}

STATE* DECTIGER::Copy(const STATE& state) const
{
    const DECTIGER_STATE& nstate = safe_cast<const DECTIGER_STATE&>(state);
    DECTIGER_STATE* newstate = MemoryPool.Allocate();
    *newstate = nstate;
    return newstate; 
}

void DECTIGER::Validate(const STATE& state) const
{
    const DECTIGER_STATE& nstate = safe_cast<const DECTIGER_STATE&>(state);
    // what to validate?
}

STATE* DECTIGER::CreateStartState() const
{
    DECTIGER_STATE* nstate = MemoryPool.Allocate();
    nstate->SL = true;
    return nstate;
}

void DECTIGER::FreeState(STATE* state) const
{
    DECTIGER_STATE* nstate = safe_cast<DECTIGER_STATE*>(state);
    MemoryPool.Free(nstate);
}

bool DECTIGER::Step(STATE& state, int action, 
    int& observation, double& reward) const
{
    DECTIGER_STATE& nstate = safe_cast<DECTIGER_STATE&>(state);
    reward = 0;
    observation = Bernoulli(ObsProb);

    // TODO
    if (nstate.SL) {
    	if (action == 0) {
    		reward = -50;
    	} else if (action == 1) {
			reward = +9;
    	} else if (action == 2) {
    		reward = -2;
    	}
    } else {
    	if (action == 0) {
    		reward = +9;
    	} else if (action == 1) {
			reward = -50;
    	} else if (action == 2) {
    		reward = -2;
    	}
    }

    return false;
}

void DECTIGER::DisplayBeliefs(const BELIEF_STATE& beliefState, 
    std::ostream& ostr) const
{
}

void DECTIGER::DisplayState(const STATE& state, std::ostream& ostr) const
{
    const DECTIGER_STATE& nstate = safe_cast<const DECTIGER_STATE&>(state);
    ostr << (nstate.SL ? "Tiger behind left" : "Tiger behind right") << endl;
}

void DECTIGER::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const
{
    switch (observation)
    {
    case 0:
        ostr << "Heard noise behind left" << endl;
        break;
    case 1:
        ostr << "Heard noise behind right" << endl;
        break;
    }
}

void DECTIGER::DisplayAction(int action, std::ostream& ostr) const
{
    switch (action)
    {
	case 0:
    	ostr << "Open left" << endl;
    	break;
    case 1:
        ostr << "Open right" << endl;
        break;
    case 2:
    	ostr << "Listen" << endl;
    }
}