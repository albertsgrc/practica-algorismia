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

    inline static bool comp(int a, int b) {
        
        #if _STATS_
            ++numero_comparaciones_ordenacion;
        #endif

        return a < b;
    }

public:

    #if _STATS_

    int numero_comparaciones_busqueda_fracaso;
    int numero_iteraciones_busqueda_fracaso;
    int numero_comparaciones_busqueda_exito;
    int numero_iteraciones_busqueda_exito;

    #endif

    busqueda_binaria() {}

    busqueda_binaria(const VI& _v) {
        #if _STATS_

        numero_comparaciones_ordenacion =
        numero_comparaciones_busqueda_exito =
        numero_comparaciones_busqueda_fracaso =
        numero_iteraciones_busqueda_fracaso =
        numero_iteraciones_busqueda_exito = 0;

        #endif

        v = _v;
        sort(v.begin(), v.end(), comp);
    }

    inline bool contiene(int x) {
        int i = 0;
        int d = v.size() - 1;

        #if _STATS_
            int comparaciones = 0;
            int iteraciones = 0;
        #endif

        while (i <= d) {

            #if _STATS_
                ++iteraciones;
            #endif

            int m = (i + d)/2;
            

            if (x < v[m]) d = m - 1;
            else if (x > v[m]) {
                
                #if _STATS_
                    ++comparaciones;
                #endif
                
                i = m + 1;
            }
            else {        
                
                #if _STATS_
                    numero_iteraciones_busqueda_exito += iteraciones;
                    numero_comparaciones_busqueda_exito += comparaciones + iteraciones;
                #endif

                return true;
            }
        }

        #if _STATS_
            numero_iteraciones_busqueda_fracaso += iteraciones;
            numero_comparaciones_busqueda_fracaso += comparaciones + iteraciones;
        #endif

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

    //for (bool b : resultado) cout << b << endl;

    #if _STATS_
        // Calculamos el uso maximo de memoria
        rusage resusage;
        getrusage(RUSAGE_SELF, &resusage);
        
        ofstream estadisticas;
        estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
        escribe_json(
            {
                {"tamaño_diccionario", diccionario.size()},
                {"tamaño_texto", texto.size()},
                {
                    "numero_comparaciones_ordenacion",
                    numero_comparaciones_ordenacion
                },
                {
                    "numero_comparaciones_busqueda_exito",
                    diccionario_busqueda_binaria.numero_comparaciones_busqueda_exito
                },
                {
                    "numero_comparaciones_busqueda_fracaso",
                    diccionario_busqueda_binaria.numero_comparaciones_busqueda_fracaso
                },
                {
                    "numero_iteraciones_busqueda_exito",
                    diccionario_busqueda_binaria.numero_iteraciones_busqueda_exito
                },
                {
                    "numero_iteraciones_busqueda_fracaso",
                    diccionario_busqueda_binaria.numero_iteraciones_busqueda_fracaso
                },
                {
                    "memoria_maxima_kb",
                    resusage.ru_maxrss
                }

            }
        , estadisticas);
        estadisticas.close();
    #else
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
    #endif
}
