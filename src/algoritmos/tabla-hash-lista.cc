#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class hash_set {
    
private:
    vector<forward_list<int>> T;
    int mascara;

    inline bool dentro(int k, forward_list<int> &l) {
        #if _STATS_
        int comparaciones = 0;
        #endif

        for (int kit : l) {
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

    inline int hash_mod(int k) {
        return k & mascara;
    }

    inline void insertar(int k) {
        int pos = hash_mod(k);
        if (not dentro(k, T[pos])) T[pos].push_front(k);
    }

    inline int siguiente_potencia_2(int x) {
        int p = 1;
        while (p < x) p <<= 1;
        return p;
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

    // avg comparaciones busqueda exito = total comparaciones busqueda exito / exitos
    // avg comparaciones busqueda fracaso = total comparaciones busqueda fracaso / fracasos
    // total comparaciones busqueda = total_comparaciones_busqueda_exito + total_comparaciones_busqueda_fracaso
    // avg comparaciones busqueda = total comparaciones busqueda / tamano texto

    // avg comparaciones creacion = total comparaciones creacion / tamano diccionario
    // total comparaciones = total_comparaciones busqueda + total comparaciones creacion
    // avg comparaciones = total comparaciones / (tamano texto + tamano diccionario)

    // total iteraciones = total comparaciones
    // total accesos memoria = total comparaciones

    #endif

    hash_set() {}
    hash_set(const vector<int>& v) {
        #if _STATS_
        en_creacion = true;
        total_comparaciones_busqueda_fracaso = 
        total_comparaciones_busqueda_exito =
        total_comparaciones_creacion = 0;
        #endif

        int tamano = siguiente_potencia_2(2*v.size());
        T = vector<forward_list<int>>(tamano);
        mascara = tamano - 1;

        for (int k : v) insertar(k);

        #if _STATS_
        tamano_tabla = tamano;
        en_creacion = false;
        #endif
    }

    inline bool contiene(int k) {
        int pos = hash_mod(k);
        return dentro(k, T[pos]); 
    }

    int colisiones() {
        int occupied_buckets = 0;
        int elements = 0;

        for (const forward_list<int>& l : T) {
            occupied_buckets += l.empty() ? 0 : 1;
            for (auto it = l.begin(); it != l.end(); ++it) ++elements; 
        }

        return elements - occupied_buckets;
    }

    int tamano_maximo_bucket() {
        int maximo = 0;

        for (const forward_list<int>& l : T) {
            int elementos = 0;
            for (auto it = l.begin(); it != l.end(); ++it) ++elementos; 
            maximo = max(maximo, elementos);
        }

        return maximo;
    }
};


hash_set diccionario_hash;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    diccionario_hash = hash_set(diccionario);

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
            },
            {
                "tamano_maximo_bucket",
                diccionario_hash.tamano_maximo_bucket()
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