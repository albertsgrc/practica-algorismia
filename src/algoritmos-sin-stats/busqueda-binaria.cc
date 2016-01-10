#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sys/time.h>
#include <sys/resource.h>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

Cronometro<> insercion, busqueda;

int numero_comparaciones_ordenacion;

class busqueda_binaria {

private:
    VI v;

    inline static bool comp(int a, int b) { return a < b; }

public:
    busqueda_binaria() {}

    busqueda_binaria(const VI& _v) {
        v = _v;
        sort(v.begin(), v.end(), comp);
    }

    inline bool contiene(int x) {
        int i = 0;
        int d = v.size() - 1;

        while (i <= d) {
            int m = (i + d)/2;

            if      (x < v[m]) d = m - 1;
            else if (x > v[m]) i = m + 1;
            else               return true;
        }

        return false;
    }

};

busqueda_binaria diccionario_busqueda_binaria;

void algoritmo(VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();
    
    diccionario_busqueda_binaria = busqueda_binaria(diccionario);
    
    insercion.finalizar();

    busqueda.iniciar();

    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_busqueda_binaria.contiene(texto[i]);
    
    busqueda.finalizar();
}


int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    VB resultado(texto.size());

    Cronometro<> total;
    total.iniciar();
    algoritmo(diccionario, texto, resultado);
    total.finalizar();

    for (bool b : resultado) cout << b << endl;

    ofstream tiempo;
    tiempo.open(argc > 3 ? argv[3] : "tiempo.json");
    escribe_json(
        {
            {"tiempo_total", total.transcurrido()},
            {"tiempo_insercion", insercion.transcurrido()},
            {"tiempo_busqueda", busqueda.transcurrido()}
        }
    ,
    tiempo);
    tiempo.close();
}
