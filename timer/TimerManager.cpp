#include "TimerManager.h"

int64_t TimerManager::addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& TimerCallback)
{
    Timer* pTimer = new Timer(repeatedCount, interval, TimerCallback);
    m_listTimers.push_back(pTimer);
    return pTimer->getId();
}

bool TimerManager::removeTimer(int64_t timerId)
{
    for (auto iter = m_listTimers.begin(); iter != m_listTimers.end(); ++iter)
    {
        if ((*iter)->getId() == timerId)
        {
            m_listTimers.erase(iter);
            return true;
        }
    }
    return false;
}

void TimerManager::checkAndHandleTimers()
{
    Timer* deletedTimer;
    for (auto iter = m_listTimers.begin(); iter != m_listTimers.end(); )
    {
        if ((*iter)->isExpired())
        {
            (*iter)->run();
            if ((*iter)->getRepeatedTimes() == 0)
            {
                deletedTimer = *iter;
                iter = m_listTimers.erase(iter);
                delete deletedTimer;
                continue;
            }
            else
            {
                ++iter;
            }
        }
    }
}

/*
上述直接使用的链表的方式实现,性能低,每次都需要遍历所有的timer对象
优化方案:
1. 使用时间轮:内存消耗会大点,适用于有很多定时器的场景

2. 使用时间堆(小根堆):插入性能是 O(logn),比较慢,适合于比较少定时器的场景,占用内存小

*/