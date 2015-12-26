#include <chrono>

template<typename TiempoT = std::chrono::microseconds,
    typename RelojT=std::chrono::high_resolution_clock,
    typename DuracionT=unsigned long long>
class Cronometro
{
private:
    std::chrono::time_point<RelojT> _inicio, _fin;
public:
    Cronometro() { iniciar(); }
    void iniciar() { _inicio = _fin = RelojT::now(); }
    DuracionT finalizar() { _fin = RelojT::now(); return transcurrido();}
    DuracionT transcurrido() {
        auto delta = std::chrono::duration_cast<TiempoT>(_fin-_inicio);
        return delta.count();
    }
};
