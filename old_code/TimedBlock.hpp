// Version: 2.0
// Author: M. Chehadeh
// Date: 29 Dec 2019
// Release note: timed block has timer responsibility within it. Looper.hpp is phased out. TimedBlock is inheriting from Block

#pragma once
#include "common_types.hpp"
#include "common_srv/Timer.hpp"
class TimedBlock {

private:
    block_frequency _frequency;
    Timer internal_timer;
    
public:
    //TODO what to do with runTasks?
    virtual void runTasks(){}
    void tickTimer();
    block_frequency getLoopFrequency();
    void setLoopFrequency(block_frequency);
    int getLoopPeriodMicroSec();
    bool hasLoopTimeElapsed();
    int getLoopRemainingMicroSec();
    TimedBlock();
    TimedBlock(block_frequency);
    ~TimedBlock();
};