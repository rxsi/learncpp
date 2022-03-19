#ifndef __TIMER_H__
#define __TIMER_H__

#include <functional>
#include <mutex>

typedef std::function<void()> TimerCallback;

class Timer
{
public:
    Timer(int32_t repeatedTimes, int64_t interval, const TimerCallback& timerCallback);
    ~Timer();

    int64_t getId()
    {
        return m_id;
    }

    bool isExpired() const;

    int32_t getRepeatedTimes()
    {
        return m_repeatedTimes;
    }

    void run();

public:
    static int64_t generateId();

private:
    int64_t m_id;               // 定时器ID
    time_t m_expiredTime;       // 定时器到期时间
    int32_t m_repeatedTimes;    // 定时器重复触发的次数
    TimerCallback m_callback;   // 定时器触发后的回调函数
    int64_t m_interval;         // 触发时间间隔
    static int64_t s_initialId; // 定时器的ID基准值,初始值为0
    static std::mutex s_mutex;  // 保护 s_initialId 的互斥体对象
};

int64_t Timer::s_initialId{0};
std::mutex Timer::s_mutex{};

#endif 