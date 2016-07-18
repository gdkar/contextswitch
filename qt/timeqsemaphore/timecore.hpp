_Pragma("once")

#include <cstdlib>
#include <ctime>
#include <cstdint>

#include <x86intrin.h>

namespace TimeCore {
    class tsc_timer {
        uint64_t    m_start{0};
        uint64_t    m_lap{0};
        uint64_t    m_stop{0};
        uint64_t    m_reps{0};
        uint64_t    m_lreps{0};
        uint32_t    m_aux{0};
    public:
        tsc_timer(const tsc_timer &) = default;
        tsc_timer(tsc_timer &&) noexcept = default;
        tsc_timer &operator = (const tsc_timer &) = default;
        tsc_timer &operator = (tsc_timer &&) noexcept = default;
        tsc_timer() : m_start(__rdtscp(&m_aux)), m_lap(m_start) {}
        double    lap()
        {
            return m_lap - std::exchange(m_lap, __rdtscp(&m_aux)) ;
        }
        double lap(uint64_t reps)
        {
            m_reps += reps;
            return (m_lap - std::exchange(m_lap, __rdtscp(&m_aux))) * ( 1. / std::exchange(m_lreps,reps));
        }
        double stop()
        {
            m_stop  = __rdtscp(&m_aux);
            return duration();
        }
        double stop(uint64_t reps)
        {
            m_stop  = __rdtscp(&m_aux);
            m_reps += reps;
            return iteration();
        }
        double duration() const
        {
            return m_stop - m_start;
        }
        double iteration() const
        {
            return duration() * (1.0 / m_reps);
        }
    };
}
