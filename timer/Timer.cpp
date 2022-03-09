#include "Timer.h"
#include <time.h>

int64_t Timer::generateId()
{
    int64_t tmpId;
    s_mutex.lock();
    ++s_initialId;
    tmpId = s_initialId; // 这里不直接返回s_initialId的原因是避免在unlock之后,线程失去执行时间片,而别的线程将s_initialId递增
    s_mutex.unlock();

    return tmpId;
}

Timer::Timer(int32_t repeatedTimes, int64_t interval, const TimerCallback& timerCallback)
{
    m_repeatedTimes = repeatedTimes;
    m_interval = interval;
    m_expiredTime= (int64_t)time(nullptr) + interval;

    m_callback = timerCallback;
    m_id = Timer::generateId();
}

bool Timer::isExpired() const
{
    int64_t now = time(nullptr);
    return now >= m_expiredTime;
}

void Timer::run()
{
    m_callback();

    if (m_repeatedTimes >= 1)
    {
        --m_repeatedTimes;
    }

    m_expiredTime += m_interval;
}