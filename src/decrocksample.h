#ifndef DECROCKSAMPLE_H
#define DECROCKSAMPLE_H

#include "simulator.h"
#include "coord.h"
#include "grid.h"

class DECROCKSAMPLE_STATE : public STATE
{
public:
    std::vector<COORD> AgentPos;
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

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

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
      int Size, NumRocks, NumAgents;
      std::vector<COORD> StartPos;
      double HalfEfficiencyDistance;
      double SmartMoveProb;
      int UncertaintyCount;

  private:

      mutable MEMORY_POOL<DECROCKSAMPLE_STATE> MemoryPool;
};

#endif
