#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class hash_lista {
    
private:
    vector<forward_list<int>> T;

    // mascara que se usa para hacer el módulo del hash con una and bit a bit.
    // Eso implica que el tamaño de la tabla sea potencia de 2, lo cual 
    // satisfacemos en la creadora.
    int mascara_modulo; 

    // Retorna si el entero k esta en la lista l
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

    // En este caso la funcion de hash es el entero mismo, ya que esto produce
    // un hashing perfecto
    inline int posicion(int k) {
        return k & mascara_modulo;
    }

    inline void insertar(int k) {
        int pos = posicion(k);
        if (not dentro(k, T[pos])) T[pos].push_front(k);
    }

    // Devuelve la siguiente potencia de 2 >= x
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

    hash_lista() {}
    hash_lista(const VI& v) {

        #if _STATS_
        en_creacion = true;
        total_comparaciones_busqueda_fracaso = 
        total_comparaciones_busqueda_exito =
        total_comparaciones_creacion = 0;
        #endif

        int tamano = siguiente_potencia_2(2*v.size());
        T = vector<forward_list<int>>(tamano);
        mascara_modulo = tamano - 1;

        for (int k : v) insertar(k);

        #if _STATS_
        tamano_tabla = tamano;
        en_creacion = false;
        #endif

    }

    inline bool contiene(int k) {
        int pos = posicion(k);
        return dentro(k, T[pos]); 
    }

    int colisiones() {
        int listas_no_vacias = 0;
        int elementos = 0;

        for (const forward_list<int>& l : T) {
            listas_no_vacias += l.empty() ? 0 : 1;
            for (auto it = l.begin(); it != l.end(); ++it) ++elementos; 
        }

        return elementos - listas_no_vacias;
    }

    // Devuelve el tamano de la lista mas grande de la tabla de hash
    int tamano_maximo_lista() {
        int maximo = 0;

        for (const forward_list<int>& l : T) {
            int elementos = 0;
            for (auto it = l.begin(); it != l.end(); ++it) ++elementos; 
            maximo = max(maximo, elementos);
        }

        return maximo;
    }
};


hash_lista diccionario_hash_lista;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    diccionario_hash_lista = hash_lista(diccionario);

    for (int i = 0; i < texto.size(); ++i) 
        resultado[i] = diccionario_hash_lista.contiene(texto[i]);
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    VB resultado(texto.size());

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
                    "tamano_maximo_lista",
                    diccionario_hash.tamano_maximo_lista()
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