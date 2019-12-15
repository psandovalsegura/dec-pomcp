#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "mcts.h"
#include "simulator.h"
#include "statistic.h"
#include "decrocksample.h"
#include <fstream>

//----------------------------------------------------------------------------

struct RESULTS
{
    void Clear();

    STATISTIC Time;
    STATISTIC Reward;
    STATISTIC DiscountedReturn;
    STATISTIC UndiscountedReturn;
};

inline void RESULTS::Clear()
{
    Time.Clear();
    Reward.Clear();
    DiscountedReturn.Clear();
    UndiscountedReturn.Clear();
}

//----------------------------------------------------------------------------

class EXPERIMENT
{
public:

    struct PARAMS
    {
        PARAMS();

        int NumRuns;
        int NumSteps;
        int SimSteps;
        double TimeOut;
        int MinDoubles, MaxDoubles;
        int TransformDoubles;
        int TransformAttempts;
        double Accuracy;
        int UndiscountedHorizon;
        bool AutoExploration;
    };

    EXPERIMENT(const SIMULATOR& real, const SIMULATOR& simulator,
        const std::string& outputFile,
        EXPERIMENT::PARAMS& expParams, MCTS::PARAMS& searchParams);

    void Run(int n);
    void MultiRun();
    void DiscountedReturn();
    void AverageReward();

    void SingleRun();
    void DisplayParameters();

    STATE* StartSteppedSingleRun();
    bool StepSingleRun(STATE *state, int& action);

private:

    const SIMULATOR& Real;
    const SIMULATOR& Simulator;
    EXPERIMENT::PARAMS& ExpParams;
    MCTS::PARAMS& SearchParams;
    RESULTS Results;

    std::ofstream OutputFile;
    MCTS* steppedMCTS;
    double steppedDiscount;
};

class DECEXPERIMENT
{
public:
    DECEXPERIMENT(const DECROCKSAMPLE& simulator,
        const std::string& outputFile,
        EXPERIMENT::PARAMS& expParams, MCTS::PARAMS& searchParams);

    void DecentralizedRun(int n);

  private:

    const DECROCKSAMPLE& Real;
    std::vector<ROCKSAMPLE> Simulators;
    EXPERIMENT::PARAMS& ExpParams;
    MCTS::PARAMS& SearchParams;
    RESULTS Results;
    std::ofstream OutputFile;
};


//----------------------------------------------------------------------------

#endif // EXPERIMENT_H
