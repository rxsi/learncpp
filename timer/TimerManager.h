#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <stdint.h>
#include <list>

#include "Timer.h"

void defaultTimerCallback()
{

}

class TimerManager
{
public:
    TimerManager() = default;
    ~TimerManager() = default;

    int64_t addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback);
    bool removeTimer(int64_t timerId);
    void checkAndHandleTimers();

private:
    std::list<Timer*> m_listTimers;
};

#endif