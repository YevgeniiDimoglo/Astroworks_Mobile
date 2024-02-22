//
// Created by Yujin on 2024/02/22.
//

#ifndef ASTROWORKS_MOBILE_HIGHRESOLUTIONTIMER_H
#define ASTROWORKS_MOBILE_HIGHRESOLUTIONTIMER_H

#include <chrono>

class HighResolutionTimer
{
public:
    HighResolutionTimer() : delta_time(-1.0), stopped(false)
    {
        start_time = std::chrono::high_resolution_clock::now();
        base_time = std::chrono::high_resolution_clock::now();
        last_time = start_time;
    }

    float TimeStamp()
    {
        /*if (stopped)
        {
            return static_cast<float>(((stop_time - paused_time) - base_time) * seconds_per_count);
        }

        else
        {
            return static_cast<float>(((this_time - paused_time) - base_time) * seconds_per_count);
        }*/
        current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta_time_duration = current_time - base_time;
        return delta_time_duration.count();
    }

    float TimeInterval() const
    {
        return static_cast<float>(delta_time);
    }

    void Reset() // Call before message loop.
    {
        /*base_time = this_time;
        last_time = this_time;

        stop_time = 0;
        stopped = false;*/
    }

    void Start() // Call when unpaused
    {
        /*double start_time = 0;

        if (stopped)
        {
            paused_time += (start_time - stop_time);
            last_time = start_time;
            stop_time = 0;
            stopped = false;
        }*/
    }

    void Stop() // Call when paused.
    {
        if (!stopped)
        {
            stopped = true;
        }
    }

    void Tick() // Call every frame.
    {
        if (stopped)
        {
            delta_time = 0.0;
            return;
        }

        this_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta_time_duration = this_time - last_time;
        delta_time = delta_time_duration.count() / 1000;

        if (delta_time < 0.0)
        {
            delta_time = 0.0;
        }
    }

private:
    double seconds_per_count;
    double delta_time;

    std::chrono::steady_clock::time_point base_time;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point paused_time;
    std::chrono::steady_clock::time_point stop_time;
    std::chrono::steady_clock::time_point last_time;
    std::chrono::steady_clock::time_point current_time;
    std::chrono::steady_clock::time_point this_time;

    bool stopped;
};

#endif //ASTROWORKS_MOBILE_HIGHRESOLUTIONTIMER_H
