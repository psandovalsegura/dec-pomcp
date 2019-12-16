#ifndef DECROCKSAMPLE_H
#define DECROCKSAMPLE_H

#include "simulator.h"
#include "coord.h"
#include "grid.h"
#include "rocksample.h"

class DECROCKSAMPLE_STATE : public STATE
{
public:
    std::vector<ROCKSAMPLE_STATE> AgentStates;
    struct ENTRY
    {
        bool Valuable;
        bool Collected;
    };
    std::vector<ENTRY> Rocks;
};

class DECROCKSAMPLE : public SIMULATOR
{
public:

    DECROCKSAMPLE(int size, int rocks, int numAgents);

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward) const;
    std::vector<bool> Step(STATE& state, std::vector<bool> terminalStates, std::vector<int> jointAction,
        std::vector<int>& jointObservation, double& reward) const;

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    void DisplayJointObservation(std::vector<int> jointObservation, std::ostream& ostr) const;
    void DisplayJointAction(std::vector<int> jointAction, std::ostream& ostr) const;

    int NumAgents;
    std::vector<ROCKSAMPLE> AgentSimulators;

  protected:

      enum
      {
          E_NONE,
          E_GOOD,
          E_BAD
      };

      enum
      {
          E_SAMPLE = 4
      };

      void InitGeneral();
      void Init_7_8_2();
      void Init_11_11_5();

      GRID<int> Grid;

      std::vector<COORD> RockPos;
      int Size, NumRocks;
      std::vector<COORD> StartPos;

  private:

      mutable MEMORY_POOL<DECROCKSAMPLE_STATE> MemoryPool;
};

#endif
