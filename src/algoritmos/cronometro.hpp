#include <chrono>

template<typename TimeT = std::chrono::microseconds,
    typename ClockT=std::chrono::high_resolution_clock,
    typename DurationT=double>
class Cronometro
{
private:
    std::chrono::time_point<ClockT> _start, _end;
public:
    Cronometro() { start(); }
    void start() { _start = _end = ClockT::now(); }
    DurationT stop() { _end = ClockT::now(); return elapsed();}
    DurationT elapsed() {
        auto delta = std::chrono::duration_cast<TimeT>(_end-_start);
        return delta.count();
    }
};
