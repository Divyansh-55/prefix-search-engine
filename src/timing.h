#ifndef TIMING_H
#define TIMING_H

#include <chrono>
#include <cstddef>
#include <iostream>

using Clock = std::chrono::steady_clock;

// Elapsed milliseconds since `start`, as a real number (no integer truncation).
inline double ms_since(Clock::time_point start)
{
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

inline void report(std::size_t count, double ms)
{
    std::cerr << "\033[32m\n"
              << count << " results in " << ms << " ms.\033[0m\n\n";
}

#endif // TIMING_H
