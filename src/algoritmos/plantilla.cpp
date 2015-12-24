#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../io.hpp"
#include "../cronometro.hpp"
using namespace std;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
// El algoritmo deberá calcular las estadísticas de ejecución sólo si se
// ha compilado el programa en versión estadísticas. Así se evita
// que el tiempo de ejecución se vea afectado por el cálculo extra.
// Esto se puede hacer por ejemplo como vemos en el main 
// (#if _STATS_ ... #endif)
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    vector<bool> resultado(texto.size());

    Cronometro<> c;
    c.start();
    algoritmo(diccionario, texto, resultado);
    c.stop();

    for (bool b : resultado) cout << b << endl;

    #if _STATS_
    ofstream estadisticas;
    estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
    writeJson(
        {   
            {"tamaño_diccionario", diccionario.size()},
            {"tamaño_texto", texto.size()},
            // poner aquí todas las estadísticas, con su par nombre/valor
            // ejemplo:
            {
                "total_comparaciones_busqueda_fracaso", 
                valor_de_total_comparaciones_busqueda_fracaso
            }
        }
        ,
        estadisticas);
    #else
    ofstream tiempo;
    tiempo.open(argc > 3 ? argv[3] : "tiempo.out");
    tiempo << c.elapsed();
    tiempo.close();
    #endif
}