#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

Cronometro<> insercion, busqueda;

bool busqueda_binaria(int elem,const VI& texto,int l,int r){
    if (l > r) return false;
    int medio = l + ((r-l)/2);
    if (texto[medio] == elem) return true;
    else if (texto[medio] > elem) return busqueda_binaria(elem,texto,l,medio-1);
    else return busqueda_binaria(elem,texto,medio+1,r);
}

bool comp (int i,int j){return i<j;}
void algoritmo(VI diccionario,const VI& texto, VB& resultado) {
    insercion.iniciar();
    // codigo insercion
    sort(diccionario.begin(),diccionario.end(),comp);
    insercion.finalizar();

    busqueda.iniciar();
    // codigo busqueda
    int size = texto.size();
    int size_d = diccionario.size()-1;
    for (int i = 0; i<size;++i){
        resultado[i] = busqueda_binaria(texto[i],diccionario,0,size_d);
    }
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

    #if _STATS_
        ofstream estadisticas;
        estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
        escribe_json(
            {
                {"tamaño_diccionario", diccionario.size()},
                {"tamaño_texto", texto.size()},

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
