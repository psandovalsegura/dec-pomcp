#ifndef DECTIGER_H
#define DECTIGER_H

#include "simulator.h"
#include "beliefstate.h"

class DECTIGER_STATE : public STATE
{
public:
	// There are only two states: 
	// If SL is true, the tiger is behind the left door
	// If SL is false, the tiger is behind the right door
    bool SL;
};

class DECTIGER : public SIMULATOR
{
public:

    DECTIGER();

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action, 
        int& observation, double& reward) const;

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState, 
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

private:
    
    mutable MEMORY_POOL<NETWORK_STATE> MemoryPool;
};

#endif // DECTIGER_H