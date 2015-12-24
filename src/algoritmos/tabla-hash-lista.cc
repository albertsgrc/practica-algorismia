#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

template<typename Key>
class hash_set {
    
    private:
        vector<forward_list<Key>> T;

        inline bool dentro(const Key &k, forward_list<Key> &l) {
            #if _STATS_
            int comparaciones = 0;
            #endif

            for (Key& kit : l) {
                #if _STATS_
                ++comparaciones;
                #endif

                if (kit == k) {
                    #if _STATS_
                        if (en_creacion) total_comparaciones_creacion += comparaciones;
                        else total_comparaciones_busqueda_exito += comparaciones;
                    #endif

                    return true;
                }
            }

            #if _STATS_
                if (en_creacion) total_comparaciones_creacion += comparaciones;
                else total_comparaciones_busqueda_fracaso += comparaciones;
            #endif

            return false; 
        }

        inline void insertar(const Key& k) {
            int pos = hash<Key>()(k)&(T.size() - 1);
            if (not dentro(k, T[pos])) T[pos].push_front(k);
        }

        bool es_primo (int n) {
            if (n == 0 or n == 1) return false;
            int i = 2;
            while (i*i <= n) {
                if (n%i == 0) return false;
                ++i;
            }
            return true;
        }

    public:
        #if _STATS_

        int total_comparaciones_busqueda_exito;
        int total_comparaciones_busqueda_fracaso;
        int total_comparaciones_creacion;
        int tamano_tabla;

        bool en_creacion;

        // llamadas a insertar = tamaño diccionario
        // llamadas a contiene = tamaño texto 
        // llamadas a hash = llamadas a insertar + llamadas a contiene

        #endif

        hash_set() {}
        hash_set(const vector<Key>& v) {
            #if _STATS_
            en_creacion = true;
            total_comparaciones_busqueda_fracaso = 0;
            total_comparaciones_busqueda_exito = 0;
            total_comparaciones_creacion = 0;
            #endif

            int desiredsize = 3*v.size();
            int size = 1;
            while (size < desiredsize) size <<= 1;
            T = vector<forward_list<Key>>(size);

            for (const Key& k : v) insertar(k);

            #if _STATS_
            tamano_tabla = size;
            en_creacion = false;
            #endif
        }

        inline bool contiene(const Key &k) {
            int pos = hash<Key>()(k)&(T.size() - 1);
            return dentro(k, T[pos]); 
        }

        int colisiones() {
            int occupied_buckets = 0;
            int elements = 0;

            for (const forward_list<Key>& l : T) {
                occupied_buckets += l.empty() ? 0 : 1;
                for (auto it = l.begin(); it != l.end(); ++it) ++elements; 
            }

            return elements - occupied_buckets;
        }
};


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
    c.iniciar();
    algoritmo(diccionario, texto, resultado);
    c.finalizar();

    for (bool b : resultado) cout << b << endl;

    #if _STATS_
    ofstream estadisticas;
    estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
    escribe_json(
        {
            {"tamano_diccionario", diccionario.size()},
            {"tamano_texto", texto.size()},
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
            {
                "tamano_tabla_hash",
                diccionario_hash.tamano_tabla
            },
            {
                "colisiones",
                diccionario_hash.colisiones()
            }
        }
        ,
        estadisticas);
    #else
    ofstream tiempo;
    tiempo.open(argc > 3 ? argv[3] : "tiempo.out");
    tiempo << c.transcurrido();
    tiempo.close();
    #endif
}