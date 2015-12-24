#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../helper.hpp"
#include "hash_set.hpp"
#include <functional>
using namespace std;

hash_set<int> diccionario_hash;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    diccionario_hash = hash_set<int>(diccionario);

    for (int i = 0; i < texto.size(); ++i) 
        resultado[i] = diccionario_hash.contiene(texto[i]);
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
            {
                "total_comparaciones_busqueda_fracaso", 
                diccionario_hash.total_comparaciones_busqueda_fracaso
            },
            {
                "total_comparaciones_busqueda_exito", 
                diccionario_hash.total_comparaciones_busqueda_exito
            },
            {
                "total_comparaciones_creacion", 
                diccionario_hash.total_comparaciones_creacion
            },
        }
        ,
        estadisticas);
    #else
    ofstream tiempo;
    tiempo.open(argc > 3 ? argv[3] : "tiempo.txt");
    tiempo << c.elapsed();
    tiempo.close();
    #endif
}